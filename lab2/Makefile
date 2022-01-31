all: program

program: matrix parallel_helper matrix_blocking_adder matrix_non_blocking_adder

matrix: matrix.c matrix.h
	gcc -c matrix.c -o matrix

parallel_helper: parallel_helper.c parallel_helper.h
	gcc -c parallel_helper.c -o parallel_helper

matrix_blocking_adder: matrix_blocking_adder.c matrix.h parallel_helper.h
	mpicc -o matrix_blocking_adder matrix_blocking_adder.c matrix.c parallel_helper.c

matrix_non_blocking_adder: matrix_non_blocking_adder.c matrix.h parallel_helper.h
	mpicc -o matrix_non_blocking_adder matrix_non_blocking_adder.c matrix.c parallel_helper.c