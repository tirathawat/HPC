#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "matrix.h"

char *MATRIX_A;
char *MATRIX_B;
char *MATRIX_ANSWER;

void create_all_matrix_parallel(int number_processes, int process_rank, Matrix *matrix_a, Matrix *matrix_b, Matrix *matrix_answer)
{
    if (process_rank == 0)
    {
        create_matrix_from_file(MATRIX_A, matrix_a);
    }
    else if (process_rank == 1)
    {
        create_matrix_from_file(MATRIX_B, matrix_b);
    }
}

void run_one_process()
{
    Matrix matrix_a, matrix_b, matrix_answer;

    create_matrix_from_file(MATRIX_A, &matrix_a);
    create_matrix_from_file(MATRIX_B, &matrix_b);

    double **answer_data = allocate_matrix_memory(matrix_a.rows_count, matrix_b.columns_count);
    matrix_answer = new_matrix(matrix_a.rows_count, matrix_b.columns_count, answer_data);

    matrix_answer.data = multiply_matrix(&matrix_a, &matrix_b);

    write_matrix_file(MATRIX_ANSWER, &matrix_answer);
}

void broadcast_matrix(Matrix *matrix, int root)
{
    MPI_Bcast(&matrix->rows_count, 1, MPI_INT, root, MPI_COMM_WORLD);
    MPI_Bcast(&matrix->columns_count, 1, MPI_INT, root, MPI_COMM_WORLD);
    MPI_Bcast(&matrix->data[0][0], matrix->rows_count * matrix->columns_count, MPI_DOUBLE, root, MPI_COMM_WORLD);
}

void calculate_sending_rows_size(int *sending_rows_size, int rows_count, int number_processes, int process_rank)
{
    if (process_rank == 0)
    {
        int i;
        int sending_rows_block = rows_count / number_processes;
        int remaining_size = rows_count % number_processes;

        for (i = 0; i < number_processes; i++)
        {
            sending_rows_size[i] = sending_rows_block;
        }

        for (i = 0; i < remaining_size; i++)
        {
            sending_rows_size[i]++;
        }
    }
}

void calculate_size(int *sending_rows_size, int *displacement, int *matrix_size, int columns_count, int number_processes, int process_rank)
{
    if (process_rank == 0)
    {
        int i, temp_size;

        for (i = 0, temp_size = 0; i < number_processes; i++)
        {
            matrix_size[i] = sending_rows_size[i] * columns_count;
            displacement[i] = temp_size;
            temp_size += matrix_size[i];
        }
    }
}

void gather_answers_from_processes(int number_processes, int process_rank)
{
    int i, temp_size;
    int *sending_rows_size, *displacement, *re_matrix_size;
}

void write_file(int process_rank, Matrix *matrix_answer)
{
    if (process_rank == 0)
    {
        write_matrix_file(MATRIX_ANSWER, matrix_answer);
    }
}

void run_many_processes(int number_processes, int process_rank)
{
    Matrix matrix_a, matrix_b, matrix_answer, partial_matrix;

    int *sending_rows_size = calloc(number_processes, sizeof(int));

    int *sending_displacement = calloc(number_processes, sizeof(int));
    int *sending_matrix_size = calloc(number_processes, sizeof(int));

    int *receiving_matrix_size = calloc(number_processes, sizeof(int));
    int *receiving_displacement = calloc(number_processes, sizeof(int));

    check_matrix_size_from_file(MATRIX_A, &matrix_a.rows_count, &matrix_a.columns_count);
    check_matrix_size_from_file(MATRIX_B, &matrix_b.rows_count, &matrix_b.columns_count);

    matrix_a.data = allocate_matrix_memory(matrix_a.rows_count, matrix_a.columns_count);
    matrix_b.data = allocate_matrix_memory(matrix_b.rows_count, matrix_b.columns_count);

    double **answer_data = allocate_matrix_memory(matrix_a.rows_count, matrix_b.columns_count);
    matrix_answer = new_matrix(matrix_a.rows_count, matrix_b.columns_count, answer_data);

    create_all_matrix_parallel(number_processes, process_rank, &matrix_a, &matrix_b, &matrix_answer);

    broadcast_matrix(&matrix_b, 1);

    calculate_sending_rows_size(sending_rows_size, matrix_a.rows_count, number_processes, process_rank);
    calculate_size(sending_rows_size, sending_displacement, sending_matrix_size, matrix_a.columns_count, number_processes, process_rank);
    calculate_size(sending_rows_size, receiving_displacement, receiving_matrix_size, matrix_b.columns_count, number_processes, process_rank);

    int rows_count = sending_rows_size[0];

    MPI_Scatter(sending_rows_size, 1, MPI_INT, &rows_count, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&matrix_a.columns_count, 1, MPI_INT, 0, MPI_COMM_WORLD);

    double **data = allocate_matrix_memory(rows_count, matrix_a.columns_count);
    int size = rows_count * matrix_a.columns_count;
    MPI_Scatterv(&matrix_a.data[0][0], sending_matrix_size, sending_displacement, MPI_DOUBLE, &data[0][0], size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    partial_matrix = new_matrix(rows_count, matrix_a.columns_count, data);
    double **result_data = multiply_matrix(&partial_matrix, &matrix_b);
    size = partial_matrix.rows_count * matrix_b.columns_count;
    MPI_Gatherv(&result_data[0][0], size, MPI_DOUBLE, &matrix_answer.data[0][0], receiving_matrix_size, receiving_displacement, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    write_file(process_rank, &matrix_answer);
}

void handle_processes(int number_processes, int process_rank)
{
    if (number_processes == 1)
    {
        run_one_process();
    }
    else
    {
        run_many_processes(number_processes, process_rank);
    }
}

void init_files_names(char **argv)
{
    MATRIX_A = argv[1];
    MATRIX_B = argv[2];
    MATRIX_ANSWER = argv[3];
}

int main(int argc, char *argv[])
{
    int number_processes;
    int process_rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &number_processes);

    init_files_names(argv);

    handle_processes(number_processes, process_rank);

    MPI_Finalize();

    return 0;
}