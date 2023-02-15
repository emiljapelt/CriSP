#include "types.h"

void init_utils(uint64 partitions);

uint64 hash(uint64 input);

void print_partitions(struct partition_info info, int partition_count, char with_contents);