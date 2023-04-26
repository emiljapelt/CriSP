#include "types.h"

void init_utils(uint64 partitions);

uint64 hash(uint64 input);

void print_partitions(struct partition_info info, int partition_count, char with_contents);
void print_partition_distribution(struct partition_info info, int partition_count, int levels);
void print_partition_statistic(struct partition_info info, int partition_count);
void print_correct_stats(uint64 *data, uint64 partition_count, uint64 problem_size);

void array_print(uint64* array, uint64 size);
char is_sorted(uint64* array, uint64 size);