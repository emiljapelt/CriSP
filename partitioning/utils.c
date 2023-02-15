#include <stdio.h>

#include "utils.h"

uint64 hash_cardinality;

void init_utils(uint64 partition_count) {
    hash_cardinality = partition_count;
}

uint64 hash(uint64 input) {
    return input % hash_cardinality;
}

void print_partitions(struct partition_info info, int partition_count, char with_contents) {
    for(int i = 0; i < partition_count; i++) {
        int partition_size = info.partition_sizes[i];
        struct tuple* partition = info.partitions[i];
        printf("Partition %i: %i\n", i, partition_size);
        if (with_contents) for(int o = 0; o < partition_size; o++) {
            printf("    %llu -> %llu\n", partition[o].fst, partition[o].snd);
        }
    }
}