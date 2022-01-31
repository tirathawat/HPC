#ifndef PARALLELHELPER_H
#define PARALLELHELPER_H

int calculate_block_size(int rows_count, int columns_count, int number_processes);

int calculate_remaining_block_size(int block_size, int rows_count, int columns_count);

int calculate_sending_size(int current, int number_processes, int block_size, int remaining_block_size);

#endif