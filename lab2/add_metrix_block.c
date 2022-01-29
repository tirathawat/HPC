#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MATRIX_A_SMALL "matAsmall.txt"
#define MATRIX_B_SMALL "matBsmall.txt"
#define MATRIX_ANSWER_SMALL "answerSmall.txt"

typedef struct matrix_t
{
    int rows_count;
    int columns_count;
    float **data;
} Matrix;

float **allocate_matrix_memory(int row_count, int column_count)
{
    int i;

    float **matrix = malloc(row_count * sizeof(float));

    for (i = 0; i < row_count; i++)
    {
        matrix[i] = malloc(column_count * sizeof(float));
    }

    return matrix;
}

void create_matrix_from_file(char *filename, Matrix *matrix)
{
    int i, j;

    FILE *fp = fopen(filename, "r");
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

int calculate_block_size(int rows_count, int columns_count, int number_processes)
{
    return (rows_count * columns_count) / number_processes;
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

float *get_vector_from_matrix(Matrix *matrix, int current, int block_size)
{
    return &matrix->data[0][0] + block_size * current;
}

void separate_data_to_processes(int number_processes, Matrix *matrixa, Matrix *matrixb, int block_size, int remaining_block_size)
{
    int i;

    for (i = 0; i < number_processes; i++)
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

    for (i = 0; i < number_processes; i++)
    {
        int sending_size = calculate_sending_size(i, number_processes, block_size, remaining_block_size);

        MPI_Recv(&answer->data[0][0] + block_size * i, sending_size, MPI_FLOAT, i, 0, MPI_COMM_WORLD, &status);
    }
}

void run_sender(int number_processes)
{
    Matrix *matrixa, *matrixb, *answer;

    create_matrix_from_file(MATRIX_A_SMALL, matrixa);
    create_matrix_from_file(MATRIX_B_SMALL, matrixb);

    answer->rows_count = matrixa->rows_count;
    answer->columns_count = matrixa->columns_count;
    answer->data = allocate_matrix_memory(matrixa->rows_count, matrixa->columns_count);

    int block_size = calculate_block_size(matrixa->rows_count, matrixa->columns_count, number_processes);
    int remaining_block_size = calculate_remaining_block_size(block_size, matrixa->rows_count, matrixa->columns_count);

    separate_data_to_processes(number_processes, matrixa, matrixb, block_size, remaining_block_size);

    receive_answer_from_processes(number_processes, answer, block_size, remaining_block_size);

    write_matrix_file(MATRIX_ANSWER_SMALL, answer);
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

    answer = plus_vector(vectora, vectora, size);

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