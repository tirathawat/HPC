#ifndef MATRIX_H
#define MATRIX_H

#define MATRIX_A "matAlarge.txt"
#define MATRIX_B "matBlarge.txt"
#define MATRIX_ANSWER "answer.txt"

typedef struct matrix_t
{
    int rows_count;
    int columns_count;
    float **data;
} Matrix;

float **allocate_matrix_memory(int row_count, int column_count);

void create_matrix_from_file(char *filename, Matrix *matrix);

void write_matrix_file(char *filename, Matrix *matrix);

float *plus_vector(float *vector1, float *vector2, int size);

float *get_vector_from_matrix(Matrix *matrix, int current, int block_size);

int calculate_block_size(int rows_count, int columns_count, int number_processes);

int calculate_remaining_block_size(int block_size, int rows_count, int columns_count);

int calculate_sending_size(int current, int number_processes, int block_size, int remaining_block_size);

#endif