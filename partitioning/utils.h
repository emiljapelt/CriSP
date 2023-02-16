#include "types.h"

void init_utils(uint64 partitions);

uint64 hash(uint64 input);

void print_partitions(uint64* partitions, int partition_count, char with_contents);
void print_partition_distribution(uint64* partitions, int partition_count, int levels);
void print_partition_statistic(uint64* partitions, int partition_count);