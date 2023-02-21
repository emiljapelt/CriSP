#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/random.h>
#include <sys/wait.h>
#include <pthread.h>
#include <unistd.h>

#include "utils.h"

void partition_sequential(uint64* input, uint64** output, uint64 start_index, uint64 work_size, uint64 partition_count) {

    uint64* partitions = malloc((sizeof(uint64) * partition_count) + ((2 * sizeof(uint64)) * work_size));

    uint64 hashes[work_size];
    for(int i = 0; i < partition_count; i++) partitions[i] = 0;
    for(uint64 i = 0; i < work_size; i++) {
        hashes[i] = hash(input[start_index+(2*i)]);
        partitions[hashes[i]]++;
    }

    int partition_indices[partition_count];
    for(int i = 0; i < partition_count; i++) partition_indices[i] = 0;

    for(uint64 i = 0; i < work_size; i++) {
        uint64 hash = hashes[i];
        uint64 section_offset = partition_count;
        for(int h = 0; h < hash; h++) section_offset += 2 * partitions[h];
        uint64 partition_index = 2 * partition_indices[hash]++;
        partitions[section_offset + partition_index] = input[start_index+(2*i)];
        partitions[section_offset + partition_index + 1] = input[start_index+(2*i)+1];
    }

    *output = partitions;
}

uint64* create_args(uint64* input, uint64** output, uint64 start_index, uint64 work_size, uint64 partition_count) {
    uint64* alloc = malloc(5 * 8);
    alloc[0] = (uint64)input;
    alloc[1] = (uint64)output;
    alloc[2] = start_index;
    alloc[3] = work_size;
    alloc[4] = partition_count;
    return alloc;
}

void* call_partition_sequential(void* data) {
    uint64* args = (uint64*)data;

    uint64* input = *(uint64**)args;
    uint64** output = *(uint64***)(args+1);
    uint64 start_index = *(uint64*)(args+2);
    uint64 work_size = *(uint64*)(args+3);
    uint64 partition_count = *(uint64*)(args+4);

    partition_sequential(input, output, start_index, work_size, partition_count);
    free(data);

    return NULL;
}

void partition_individual_output(uint64* input, uint64** output, uint64 input_size, uint64 threads, uint64 partition_count) {

    uint64* partitions = malloc((sizeof(uint64) * partition_count) + ((2 * sizeof(uint64)) * input_size));

    uint64 work_pr_thread = input_size / threads;
    uint64* thread_buffers[threads];
    pthread_t thread_pids[threads];
    for(int t = 0; t < threads; t++) {
        uint64 start_index = t * work_pr_thread;
        uint64 work_size;
        
        if(t == threads - 1) work_size = input_size - start_index;
        else work_size = work_pr_thread;

        pthread_create(&thread_pids[t], NULL, call_partition_sequential, create_args(input, thread_buffers+t, start_index, work_size, partition_count));
    }

    for(int t = 0; t < threads; t++) {
        pthread_join(thread_pids[t], NULL);
    }
    
    for(int i = 0; i < partition_count; i++) {
        partitions[i] = 0;
        for(int t = 0; t < threads; t++) partitions[i] += thread_buffers[t][i];
    }

    int partition_indices[partition_count];
    for(int i = 0; i < partition_count; i++) partition_indices[i] = 0;

    for(int t = 0; t < threads; t++) {
        for(int i = 0; i < partition_count; i++) {
            uint64 thread_partition_size = thread_buffers[t][i];
            uint64 thread_partition_offset = partition_count;
                for(int p = 0; p < i; p++) thread_partition_offset += 2 * thread_buffers[t][p];
            uint64 section_offset = partition_count;
                for(int p = 0; p < i; p++) section_offset += 2 * partitions[p];

            for(int s = 0; s < thread_partition_size; s++) {
                int partition_index = 2 * partition_indices[i]++;
                partitions[section_offset + partition_index] = thread_buffers[t][thread_partition_offset + (2*s)];
                partitions[section_offset + partition_index + 1] = thread_buffers[t][thread_partition_offset + (2*s) + 1];
            }
        }
        free(thread_buffers[t]);
    }

    *output = partitions;
}

int main() {
// Setup
    int problem_size = 100000;
    uint64 b = 4;
    uint64 partition_count = 1llu << b;

    init_utils(partition_count);

// Generate data
    uint64*  data = malloc((2 * sizeof(uint64)) * problem_size);
    char buffer[8];
    for(int i = 0; i < problem_size; i++) {
        getrandom(&buffer, 8, 0);
        data[2*i] = *(uint64*)&buffer;
        data[(2*i)+1] = i;
    }
    
// Calculate partitions
    uint64* partitions;
    partition_sequential(data, &partitions, 0, problem_size, partition_count);
    //partition_individual_output(data, &partitions, problem_size, 4, partition_count);

// Print
    // print_partitions(partitions, partition_count, 1);
    print_partition_distribution(partitions, partition_count, 150);
    print_partition_statistic(partitions, partition_count);
}