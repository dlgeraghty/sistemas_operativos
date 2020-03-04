/* print files in current directory in reverse order */
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

int
main(int argc, int *argv[])
{
    struct dirent **namelist;
    int n;

    if(argc == 2){
   n = scandir(argv[1], &namelist, NULL, alphasort);
    if (n < 0)
        perror("scandir");
    else {
        while (n--) {
	    struct stat sb;
	    if(stat(namelist[n]->d_name, &sb)) perror("stat");
	    else{
	    	printf("Name: %s, i-node number: %ld, mode: %lo (octal), size: %lld\n", namelist[n]->d_name, (long) sb.st_ino, (unsigned long) sb.st_mode, (long)sb.st_size);
	    }
            free(namelist[n]);
        }
        free(namelist);
    }
    }
    else{
    	printf("wrong arguments\n");
    }
}
