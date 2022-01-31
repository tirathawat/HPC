#include <stdio.h>
#include <stdlib.h>

#include "parallel_helper.h"

int calculate_block_size(int rows_count, int columns_count, int number_processes)
{
    return (rows_count * columns_count) / (number_processes);
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