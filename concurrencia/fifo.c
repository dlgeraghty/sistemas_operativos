#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>
#include <asm-generic/errno.h>
#include <linux/semaphore.h>
#include <linux/kfifo.h>

#define MAX_BYTES 64
#define MAX_CHARS_KBUF 64

MODULE_LICENSE("GPL");

static struct proc_dir_entry *proc_entry;
//buffer circular de bytes(struct kfifo) (almacenamiento temporal asociado al FIFO , establecer 64 bytes maximo)
struct kfifo cbuffer;         //para el bufffer circular

//dos contadores para registrasr el numero de procesos que han abierto el FIFO:
int prod_count = 0;           //para el numero de procesos que ABRIERON la entrada /proc para ESCRITURA (productores)
int cons_count = 0;           //para el numero de procesos que ABRIERON la entrada /proc para LECTURA (cosnumidores)

//un mutex para proteger el buffer y los contadores:
struct semaphore mtx;         //mutex para protegwer el buffer y los contadores, emulado con un semaforo a 1

//dos "variables condicion" para bloquear al productor y al consumidor (para emular cada variable consdicion se usara:)
//semaforo inicializado a 0: (cola de espera)
struct semaphore sem_prod;    //COLA DE ESPERA para productor(es)
struct semaphore sem_cons;    //COLA DE ESPERA para los consumidor(es)

//contador que registra numero de procesos esperando (0 o 1):
int nr_prod_waiting = 0;      //lleva la cuenta de los procesos productores que estan esperando
int nr_cons_waiting = 0;      //lleva la cuenta de los procesos consumidores que estan esperando

                              //los semaforos se utilizan como colas de espera como en el ejemplo ProdCons2

static int fifoproc_open(struct inode * inode, struct file *file ){

	//lock:
	if(down_interruptible(&mtx)) return -EINTR;
	  //distinguier entre productor y consumidor consuiltando el campo f_mode de la estructura struct file:
	if(file->f_mode & FMODE_READ){

		//un consumidor abrio el fifo

		cons_count++;
		while(prod_count <= 0){
			//cond_wait(cons, mtx):

			nr_cons_waiting++;
			up(&mtx);

			if(down_interruptible(&sem_cons)){
				down(&mtx);
				nr_cons_waiting--;
				up(&mtx);
				return -EINTR;
			}
			if(down_interruptible(&mtx)) return -EINTR;
		}

		//cond_signal(prod);
		if(nr_prod_waiting > 0){
			up(&sem_prod);
			nr_prod_waiting--;
		}

  }else{

		//un productor abrio el fifo
		prod_count++;
		while(cons_count <= 0){
			//cond_wait(prod, mtx):

			nr_prod_waiting++;
			up(&mtx);

			if(down_interruptible(&sem_prod)){
				down(&mtx);
				nr_prod_waiting--;
				up(&mtx);
				return -EINTR;
			}
			if(down_interruptible(&mtx)) return -EINTR;
		}
		//cond_signal(cons);
		if(nr_cons_waiting > 0){
			up(&sem_cons);
			nr_cons_waiting--;
		}

  }

  up(&mtx);

  return 0;
};

static int fifoproc_release(struct inode * inode, struct file * file){

	//lock(mtx)
	if(down_interruptible(&mtx)) return -EINTR;

	//if(lectura)
	if(file->f_mode & FMODE_READ){
		cons_count--;

		//cond_signal(prod);

		if(nr_prod_waiting > 0){
			up(&sem_prod);
			nr_prod_waiting--;
		}

	} else{
		prod_count--;

		//cond_signal(cons);

		if(nr_cons_waiting > 0){
			up(&sem_cons);
			nr_cons_waiting--;
		}

	}

	if(cons_count + prod_count == 0)
		kfifo_reset(&cbuffer);

	up(&mtx);

  return 0;
}

static ssize_t fifoproc_read(struct file *filp, char __user *buf, size_t len, loff_t *off){

	char kbuf[MAX_CHARS_KBUF+1]; //de prueba

	int nr_bytes;

	//lock(&mtx);

	if(down_interruptible(&mtx)) return -EINTR;

	while(prod_count > 0 && kfifo_len(&cbuffer) < len){
		//cond_wait(cons, mtx):

		nr_cons_waiting++;
		up(&mtx);
		if(down_interruptible(&sem_cons)){
			down(&mtx);
			nr_cons_waiting--;
			up(&mtx);
			return -EINTR;
		}

		if(down_interruptible(&mtx)) return -EINTR;
	}

	kfifo_out(&cbuffer, kbuf, len);



	if(prod_count == 0 && kfifo_is_empty(&cbuffer)){
		//unlock(&mtx):

		up(&mtx);

		return 0;
	}
	 //cons_signal(prod):

	if(nr_prod_waiting > 0){
		up(&sem_prod);
		nr_prod_waiting--;
	}
	//unlock
	up(&mtx);

	if(copy_to_user(buf, kbuf, len)) return -EFAULT;
	// DEVOLVER LEN *
	return len;
};

static ssize_t fifoproc_write(struct file *filp, const char __user *buf, size_t len, loff_t *off) {

	char kbuffer[MAX_CHARS_KBUF];

	if(len > MAX_BYTES || len > MAX_CHARS_KBUF) return -ENOMEM;

	if(copy_from_user(kbuffer, buf, len)) return -EFAULT;

	//lock(mtx)

	if(down_interruptible(&mtx)) return -EINTR;

	while(kfifo_avail(&cbuffer) < len && cons_count > 0){

		//cond_wait(prod, mtx);

		nr_prod_waiting++;
		up(&mtx);

		if(down_interruptible(&sem_prod)){
			down(&mtx);
			nr_prod_waiting--;
			up(&sem_prod);
			return -EINTR;
		}

		if(down_interruptible(&mtx)) return -EINTR;

	}

	if(cons_count == 0){

		//unlock(mtx);

		up(&mtx);

		return -EPIPE;

	}

	kfifo_in(&cbuffer, kbuffer, len);

	//cond_signal(cons);

	if(nr_cons_waiting > 0){
		up(&sem_cons);
		nr_cons_waiting--;
	}

	//unlock(&mtx);

	up(&mtx);

	return len;

}

static const struct file_operations proc_entry_fops = {
						       .read = fifoproc_read,
						       .write = fifoproc_write,
						       .open = fifoproc_open,
						       .release = fifoproc_release,
};

int init_fifoproc_module(void){
  int ret = 0;

	//inicializacion del buffer circular
  ret = kfifo_alloc(&cbuffer, MAX_BYTES, GFP_KERNEL);

  if(ret) return -ENOMEM;

  //inicializacion a 0 de los semaforos usados como colas de espera:
  sema_init(&sem_prod, 0);
  sema_init(&sem_cons, 0);

  //inicializacion a 1 del semaforo que permite acceso en exclusion mutua:
  sema_init(&mtx, 1);

  //los contadores de los procesos que estan esperando, ya se han inicliazado a 0 previamente.

  proc_entry = proc_create("fifoproc" , 0666, NULL, &proc_entry_fops);

  if(proc_entry == NULL){
  	kfifo_free(&cbuffer);
    ret = -ENOMEM;
    printk(KERN_DEBUG "Fifoproc: Cant create /proc entry\n");
  }else{
    printk(KERN_DEBUG "Fifoproc: Module loaded\n");
  }

  return ret;
}

void exit_fifoproc_module(void){

  remove_proc_entry("fifoproc" , NULL);
  kfifo_free(&cbuffer);
  printk(KERN_DEBUG "Fifoproc: Module unloaded \n");
}

module_init(init_fifoproc_module);
module_exit(exit_fifoproc_module);
