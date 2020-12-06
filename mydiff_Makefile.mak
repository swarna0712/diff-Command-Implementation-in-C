mydiff: mydiff.o mydiff_func.o
	gcc -Wall mydiff.o mydiff_func.o -o mydiff
mydiff.o: mydiff_func_header.h mydiff.c
	gcc -Wall -c mydiff.c
mydiff_func.o: mydiff_func_header.h mydiff_func.c
	gcc -Wall -c mydiff_func.c