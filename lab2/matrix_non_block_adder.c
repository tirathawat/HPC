#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "matrix.h"

void separate_data_to_processes(int number_processes, Matrix *matrixa, Matrix *matrixb, int block_size, int remaining_block_size)
{
    int i;
    MPI_Request request;

    for (i = 1; i < number_processes; i++)
    {
        int sending_size = calculate_sending_size(i, number_processes, block_size, remaining_block_size);
        float *vectora = get_vector_from_matrix(matrixa, i, block_size);
        float *vectorb = get_vector_from_matrix(matrixb, i, block_size);

        MPI_Isend(&sending_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
        MPI_Isend(vectora, sending_size, MPI_FLOAT, i, 1, MPI_COMM_WORLD, &request);
        MPI_Isend(vectorb, sending_size, MPI_FLOAT, i, 2, MPI_COMM_WORLD, &request);
    }
}

void receive_answer_from_processes(int number_processes, Matrix *answer, int block_size, int remaining_block_size)
{
    int i;
    MPI_Status status;
    MPI_Request request[number_processes];

    for (i = 1; i < number_processes; i++)
    {
        int sending_size = calculate_sending_size(i, number_processes, block_size, remaining_block_size);

        MPI_Irecv(&answer->data[0][0] + block_size * i, sending_size, MPI_FLOAT, i, 0, MPI_COMM_WORLD, &request[i]);
    }

    for (i = 1; i < number_processes; i++)
    {
        MPI_Wait(&request[i], &status);
    }
}

void process_own_data(Matrix *matrixa, Matrix *matrixb, Matrix *answer, int block_size)
{
    float *vector = plus_vector(&matrixa->data[0][0], &matrixb->data[0][0], block_size);
    memcpy(&answer->data[0][0], vector, sizeof(float) * block_size);
}

void run_sender(int number_processes)
{
    Matrix matrixa, matrixb, answer;

    create_matrix_from_file(MATRIX_A, &matrixa);
    create_matrix_from_file(MATRIX_B, &matrixb);

    answer.rows_count = matrixa.rows_count;
    answer.columns_count = matrixa.columns_count;
    answer.data = allocate_matrix_memory(matrixa.rows_count, matrixa.columns_count);

    int block_size = calculate_block_size(matrixa.rows_count, matrixa.columns_count, number_processes);
    int remaining_block_size = calculate_remaining_block_size(block_size, matrixa.rows_count, matrixa.columns_count);

    separate_data_to_processes(number_processes, &matrixa, &matrixb, block_size, remaining_block_size);
    receive_answer_from_processes(number_processes, &answer, block_size, remaining_block_size);
    process_own_data(&matrixa, &matrixb, &answer, block_size);

    write_matrix_file(MATRIX_ANSWER, &answer);
}

void run_receiver()
{
    int size;
    float *answer;
    MPI_Status status;
    MPI_Request request_size, request_vectora, request_vectorb, request_answer;

    MPI_Irecv(&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &request_size);
    MPI_Wait(&request_size, &status);

    float *vectora = (float *)malloc(size * sizeof(float));
    float *vectorb = (float *)malloc(size * sizeof(float));

    MPI_Irecv(vectora, size, MPI_FLOAT, 0, 1, MPI_COMM_WORLD, &request_vectora);
    MPI_Irecv(vectorb, size, MPI_FLOAT, 0, 2, MPI_COMM_WORLD, &request_vectorb);

    MPI_Wait(&request_vectora, &status);
    MPI_Wait(&request_vectorb, &status);

    answer = plus_vector(vectora, vectorb, size);

    MPI_Isend(answer, size, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &request_answer);
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
    double start_time, end_time;

    int number_processes;
    int process_rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &number_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

    printf("Process rank %d is running...\n", process_rank);

    start_time = MPI_Wtime();

    handle_processes(number_processes, process_rank);

    end_time = MPI_Wtime();

    MPI_Finalize();

    printf("Process rank %d is running completed with timing %f sec\n", process_rank, end_time - start_time);

    return 0;
}