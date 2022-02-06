all: program

program: matrix matrix_multiplier

matrix: matrix.c matrix.h
	gcc -c matrix.c -o matrix

matrix_multiplier: matrix_multiplier.c matrix.h
	mpicc -o matrix_multiplier matrix_multiplier.c matrix.c