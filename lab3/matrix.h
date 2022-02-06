#ifndef MATRIX_H
#define MATRIX_H

typedef struct matrix_t
{
    int rows_count;
    int columns_count;
    double **data;
} Matrix;

Matrix new_matrix(int rows_count, int columns_count, double **data);

double **allocate_matrix_memory(int row_count, int column_count);

void create_matrix_from_file(char *filename, Matrix *matrix);

void write_matrix_file(char *filename, Matrix *matrix);

double **multiply_matrix(Matrix *matrix1, Matrix *matrix2);

#endif