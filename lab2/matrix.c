#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"

float **allocate_matrix_memory(int row_count, int column_count)
{
    int i;
    int size = column_count * row_count;

    float *data = malloc(size * sizeof(float));
    float **matrix = malloc(size * sizeof(float));

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
            fscanf(fp, "%f", &matrix->data[i][j]);
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

float *plus_vector(float *vector1, float *vector2, int size)
{
    int i;

    float *result = (float *)malloc(size * sizeof(float));

    for (i = 0; i < size; i++)
    {
        result[i] = vector1[i] + vector2[i];
    }

    return result;
}

float *get_vector_from_matrix(Matrix *matrix, int offset)
{
    return &matrix->data[0][0] + offset;
}
