#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "matrix.h"

void separate_data_to_processes(int number_processes, Matrix *matrixa, Matrix *matrixb, int block_size, int remaining_block_size)
{
    int i;

    for (i = 1; i < number_processes; i++)
    {
        int sending_size = calculate_sending_size(i, number_processes, block_size, remaining_block_size);
        float *vectora = get_vector_from_matrix(matrixa, i, block_size);
        float *vectorb = get_vector_from_matrix(matrixb, i, block_size);

        MPI_Send(&sending_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        MPI_Send(vectora, sending_size, MPI_FLOAT, i, 1, MPI_COMM_WORLD);
        MPI_Send(vectorb, sending_size, MPI_FLOAT, i, 2, MPI_COMM_WORLD);
    }
}

void receive_answer_from_processes(int number_processes, Matrix *answer, int block_size, int remaining_block_size)
{
    int i;
    MPI_Status status;

    for (i = 1; i < number_processes; i++)
    {
        int sending_size = calculate_sending_size(i, number_processes, block_size, remaining_block_size);

        MPI_Recv(&answer->data[0][0] + block_size * i, sending_size, MPI_FLOAT, i, 0, MPI_COMM_WORLD, &status);
    }
}

void process_own_data(Matrix *matrixa, Matrix *matrixb, Matrix *answer, int block_size)
{
    float *vector = plus_vector(&matrixa->data[0][0], &matrixb->data[0][0], block_size);
    memcpy(&answer->data[0][0], vector, sizeof(float) * block_size);
}

void run_sender(int number_processes)
{
    double start_time, end_time;
    Matrix matrixa, matrixb, answer;

    printf("Running...\n");

    create_matrix_from_file(MATRIX_A, &matrixa);
    create_matrix_from_file(MATRIX_B, &matrixb);

    answer.rows_count = matrixa.rows_count;
    answer.columns_count = matrixa.columns_count;
    answer.data = allocate_matrix_memory(matrixa.rows_count, matrixa.columns_count);

    int block_size = calculate_block_size(matrixa.rows_count, matrixa.columns_count, number_processes);
    int remaining_block_size = calculate_remaining_block_size(block_size, matrixa.rows_count, matrixa.columns_count);

    start_time = MPI_Wtime();

    separate_data_to_processes(number_processes, &matrixa, &matrixb, block_size, remaining_block_size);
    receive_answer_from_processes(number_processes, &answer, block_size, remaining_block_size);
    process_own_data(&matrixa, &matrixb, &answer, block_size);

    end_time = MPI_Wtime();

    printf("Calculation completed with timing %1.2f sec\n", end_time - start_time);

    printf("Writing answer file...\n");

    write_matrix_file(MATRIX_ANSWER, &answer);

    printf("Writing answer file completed\n");
}

void run_receiver()
{
    int size;
    float *answer;
    MPI_Status status;

    MPI_Recv(&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    float *vectora = (float *)malloc(size * sizeof(float));
    float *vectorb = (float *)malloc(size * sizeof(float));

    MPI_Recv(vectora, size, MPI_FLOAT, 0, 1, MPI_COMM_WORLD, &status);
    MPI_Recv(vectorb, size, MPI_FLOAT, 0, 2, MPI_COMM_WORLD, &status);

    answer = plus_vector(vectora, vectorb, size);

    MPI_Send(answer, size, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
}

void handle_processes(int number_processes, int process_rank)
{
    if (process_rank == 0)
    {
        run_sender(number_processes);
    }
    else
    {
        run_receiver();
    }
}

int main(int argc, char *argv[])
{
    int number_processes;
    int process_rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &number_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

    handle_processes(number_processes, process_rank);

    MPI_Finalize();

    return 0;
}