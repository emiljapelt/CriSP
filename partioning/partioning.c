#include <stdio.h>
#include <stdlib.h>
#include <sys/random.h>

#include "utils.h"

struct partition_info partition_sequential(struct tuple input[], uint64 input_size, uint64 hash_cardinality) {
    uint64 hashes[input_size];
    int partition_sizes[hash_cardinality];
    for(int i = 0; i < hash_cardinality; i++) partition_sizes[i] = 0;
    for(int i = 0; i < input_size; i++) {
        hashes[i] = mult_hash(input[i].fst, hash_cardinality);
        partition_sizes[hashes[i]]++;
    }

    struct tuple* partitions[hash_cardinality];
    for(int i = 0; i < hash_cardinality; i++) {
        partitions[i] = malloc(16 * partition_sizes[i]);
    }

    int partition_indecies[hash_cardinality];
    for(int i = 0; i < hash_cardinality; i++) partition_indecies[i] = 0;

// printf("inital indecies\n");
//     for(int i = 0; i < hash_cardinality; i++) printf("%i\n", partition_indecies[i]);
// printf("partition sizes\n");
//     for(int i = 0; i < hash_cardinality; i++) printf("%i = %i\n", i, partition_sizes[i]);
// printf("hashes\n");
//     for(int i = 0; i < input_size; i++) printf("%llu\n", hashes[i]);

    for(int i = 0; i < input_size; i++) {
        uint64 hash = hashes[i];
        int partition_size = partition_sizes[hash];
        int partition_index = partition_indecies[hash];

        printf("%llu, %i, %i\n", hash, partition_size, partition_index);

        partitions[hash][partition_index].fst = input[i].fst;
        partitions[hash][partition_index].snd = input[i].snd;
        partition_indecies[hash]++;
    }
    

    struct partition_info result = { partitions, partition_sizes };
    return result;
}


int main() {
    init_utils();
    int problem_size = 100;
    uint64 hash_cardinality = 10;

    // Generate data
    struct tuple data[problem_size];
    for(int i = 0; i < problem_size; i++) {
        char buffer[8];
        getrandom(&buffer, 8, 0);
        data[i].fst = *(uint64*)&buffer;
        data[i].snd = i;
    }
    
    // Calculate partitions
    struct partition_info info = partition_sequential(data, problem_size, hash_cardinality);

    // Print
    printf("Input:\n");
    for(int i = 0; i < problem_size; i++) {
        printf("    %llu -> %llu\n", data[i].fst, data[i].snd);
    }

    printf("Output:\n");
    for(int i = 0; i < hash_cardinality; i++) {
        int partition_size = info.partition_sizes[i];
        struct tuple* partition = info.partitions[i];
        printf("Partition %i:\n", i);
        for(int o = 0; o < partition_size; o++) {
            printf("    %llu -> %llu\n", partition[o].fst, partition[o].snd);
        }
    }
}