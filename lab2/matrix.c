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

float *get_vector_from_matrix(Matrix *matrix, int current, int block_size)
{
    return &matrix->data[0][0] + block_size * (current - 1);
}

int calculate_block_size(int rows_count, int columns_count, int number_processes)
{
    return (rows_count * columns_count) / (number_processes - 1);
}

int calculate_remaining_block_size(int block_size, int rows_count, int columns_count)
{
    return (rows_count * columns_count) % block_size;
}

int calculate_sending_size(int current, int number_processes, int block_size, int remaining_block_size)
{
    int is_lasted_process = current == number_processes - 1;

    if (is_lasted_process)
    {
        return block_size;
    }
    else
    {
        return block_size + remaining_block_size;
    }
}