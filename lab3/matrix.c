#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"

Matrix new_matrix(int rows_count, int columns_count, double **data)
{
    Matrix matrix;
    matrix.rows_count = rows_count;
    matrix.columns_count = columns_count;
    matrix.data = data;
    return matrix;
}

double **allocate_matrix_memory(int row_count, int column_count)
{
    int i;
    int size = column_count * row_count;

    double *data = malloc(size * sizeof(double));
    double **matrix = malloc(size * sizeof(double));

    for (i = 0; i < row_count; i++)
    {
        matrix[i] = &(data[column_count * i]);
    }

    return matrix;
}

void create_matrix_from_file(char *filename, Matrix *matrix)
{
    int i, j;

    FILE *fp = fopen(filename, "r");

    if (fp == NULL)
    {
        printf("Error: could not open file %s\n", filename);
        exit(1);
    }

    fscanf(fp, "%d %d", &matrix->rows_count, &matrix->columns_count);

    matrix->data = allocate_matrix_memory(matrix->rows_count, matrix->columns_count);

    for (i = 0; i < matrix->rows_count; i++)
    {
        for (j = 0; j < matrix->columns_count; j++)
        {
            fscanf(fp, "%lf", &matrix->data[i][j]);
        }
    }

    fclose(fp);
}

void write_matrix_file(char *filename, Matrix *matrix)
{
    int i, j;

    FILE *fp = fopen(filename, "w");

    if (fp == NULL)
    {
        printf("Error: could not open file %s\n", filename);
        exit(1);
    }

    fprintf(fp, "%d %d\n", matrix->rows_count, matrix->columns_count);

    for (i = 0; i < matrix->rows_count; i++)
    {
        for (j = 0; j < matrix->columns_count; j++)
        {
            fprintf(fp, "%.1f ", matrix->data[i][j]);
        }

        fputs("\n", fp);
    }

    fclose(fp);
}

double **multiply_matrix(Matrix *matrix1, Matrix *matrix2)
{
    double **result = allocate_matrix_memory(matrix1->rows_count, matrix2->columns_count);

    for (int i = 0; i < matrix1->rows_count; i++)
    {
        for (int j = 0; j < matrix2->columns_count; j++)
        {
            result[i][j] = 0;

            for (int k = 0; k < matrix2->rows_count; k++)
            {
                result[i][j] += matrix1->data[i][k] * matrix2->data[k][j];
            }
        }
    }

    return result;
}
