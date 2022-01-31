all: program

program: matrix matrix_block_adder matrix_non_block_adder

matrix: matrix.c matrix.h
	gcc -c matrix.c -o matrix

matrix_block_adder: matrix_block_adder.c matrix.h
	mpicc -o matrix_block_adder matrix_block_adder.c matrix.c

matrix_non_block_adder: matrix_non_block_adder.c matrix.h
	mpicc -o matrix_non_block_adder matrix_non_block_adder.c matrix.c