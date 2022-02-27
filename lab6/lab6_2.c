#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    int i;
    int number_of_row, number_of_column, number_of_threads;
    float *matrix_a, *matrix_b, *matrix_out;
    clock_t start_time, end_time;
    FILE *file_a, *file_b, *file_out;

    start_time = clock();

    sscanf(argv[1], "%d", &number_of_threads);
    omp_set_dynamic(0);
    omp_set_num_threads(number_of_threads);

    file_a = fopen(argv[2], "r");
    file_b = fopen(argv[3], "r");
    file_out = fopen(argv[4], "w+");

    fscanf(file_a, "%d %d", &number_of_row, &number_of_column);
    fscanf(file_b, "%d %d", &number_of_row, &number_of_column);
    matrix_a = (float *)malloc(number_of_row * number_of_column * sizeof(float));
    matrix_b = (float *)malloc(number_of_row * number_of_column * sizeof(float));
    matrix_out = (float *)malloc(number_of_row * number_of_column * sizeof(float));
    for (i = 0; i < number_of_row * number_of_column; i++)
    {
        fscanf(file_a, "%f", &matrix_a[i]);
        fscanf(file_b, "%f", &matrix_b[i]);
    }
    fclose(file_a);
    fclose(file_b);

#pragma omp parallel for
    for (i = 0; i < number_of_row * number_of_column; i++)
    {
        matrix_out[i] = matrix_a[i] + matrix_b[i];
    }

    fprintf(file_out, "%d %d\n", number_of_row, number_of_column);
    for (i = 0; i < number_of_row * number_of_column; i++)
    {
        fprintf(file_out, "%.1f ", matrix_out[i]);
        if (i % number_of_column == number_of_column - 1)
        {
            fprintf(file_out, "\n");
        }
    }
    fclose(file_out);

    end_time = clock();

    printf("Time: %lf\n", (double)(end_time - start_time) / CLOCKS_PER_SEC);
    return 0;
}