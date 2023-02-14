#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/random.h>

#include "utils.h"

struct partition_info partition_sequential(struct tuple input[], uint64 input_size, uint64 partition_count) {
    uint64 hashes[input_size];
    int partition_sizes[partition_count];
    for(int i = 0; i < partition_count; i++) partition_sizes[i] = 0;
    for(int i = 0; i < input_size; i++) {
        hashes[i] = hash(input[i].fst);
        partition_sizes[hashes[i]]++;
    }

    struct tuple* partitions[partition_count];
    for(int i = 0; i < partition_count; i++) {
        partitions[i] = malloc(16 * partition_sizes[i]);
    }

    int partition_indecies[partition_count];
    for(int i = 0; i < partition_count; i++) partition_indecies[i] = 0;

    for(int i = 0; i < input_size; i++) {
        uint64 hash = hashes[i];
        int partition_size = partition_sizes[hash];
        int partition_index = partition_indecies[hash];

        partitions[hash][partition_index].fst = input[i].fst;
        partitions[hash][partition_index].snd = input[i].snd;
        partition_indecies[hash]++;
    }
    

    struct partition_info result = { partitions, partition_sizes };
    return result;
}

int main() {
    int problem_size = 100;
    uint64 b = 2;
    uint64 partition_count = 1 << b;

    init_utils(partition_count);

    // Generate data
    struct tuple data[problem_size];
    for(int i = 0; i < problem_size; i++) {
        char buffer[8];
        getrandom(&buffer, 8, 0);
        data[i].fst = *(uint64*)&buffer;
        data[i].snd = i;
    }
    
    // Calculate partitions
    struct partition_info info = partition_sequential(data, problem_size, partition_count);

    // Print
    printf("Input:\n");
    for(int i = 0; i < problem_size; i++) {
        printf("    %llu -> %llu\n", data[i].fst, data[i].snd);
    }

    printf("Output:\n");
    for(int i = 0; i < partition_count; i++) {
        int partition_size = info.partition_sizes[i];
        struct tuple* partition = info.partitions[i];
        printf("Partition %i:\n", i);
        for(int o = 0; o < partition_size; o++) {
            printf("    %llu -> %llu\n", partition[o].fst, partition[o].snd);
        }
    }
}