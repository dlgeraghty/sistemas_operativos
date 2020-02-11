if [ "$#" -eq 1 ]; then					# comprobar si el numero de parametros ($#) es igual a 1
	for i in *.c ;					# bucle para recorrer todos los .c del directorio
	do
		compiled=${i/.c}-sin-optimizar.o	# variable que "recorta" el .c y "pega" una nueva terminacion
		gcc -O0 $i -o ${compiled}		# compila
		if [ $? -ne 0 ]				# Comprueba errores en la compilacion ($? contiene el valor de retorno del ultimo proceso ejecutado)
		then
			echo "error en la compilacion (sin optimizar) del archivo: $i"
		else
			echo "la compilacion (sin optimizar) del archivo $i fue satisfactoria, reportando informacion sobre el tiempo: "
			chmod +x $compiled		# otorga permisos de ejecucion
			time(./$compiled $1)		# ejecuta con el parametro de entrada del script
		fi
		compiled=${i/.c}-optimizado.o
		gcc -O3 $i -o ${compiled}
		if [ $? -ne 0 ]
		then
			echo "error en la compilacion (optimizado) del archivo: $i"
		else
			echo "la compilacion (optimizada) del archivo $i fue satisfactoria, reportando informacion sobre el tiempo: "
			chmod +x $compiled
			time(./$compiled $1)
		fi
	done
else
	echo "I need one numeric argument please"
fi

# Ejemplo de uso del script: (situarnos en el directorio en el que esten los archivos .c, tener el script ahi y darle permisos de ejecucion chmod +x...) ./practica0.sh 10000
