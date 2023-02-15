#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/random.h>
#include <sys/wait.h>
#include <pthread.h>
#include <unistd.h>

#include "utils.h"

void partition_sequential(struct tuple* input, struct partition_info* output, uint64 start_index, uint64 work_size, uint64 partition_count) {
    uint64 hashes[work_size];
    int* partition_sizes = malloc(sizeof(int) * partition_count);
    for(int i = 0; i < partition_count; i++) partition_sizes[i] = 0;
    for(int i = 0; i < work_size; i++) {
        hashes[i] = hash(input[start_index+i].fst);
        partition_sizes[hashes[i]]++;
    }

    struct tuple** partitions = malloc(8 * partition_count);
    for(int i = 0; i < partition_count; i++) {
        partitions[i] = malloc(16 * partition_sizes[i]);
    }

    int partition_indecies[partition_count];
    for(int i = 0; i < partition_count; i++) partition_indecies[i] = 0;

    for(int i = 0; i < work_size; i++) {
        uint64 hash = hashes[i];
        int partition_index = partition_indecies[hash];
        partitions[hash][partition_index].fst = input[start_index+i].fst;
        partitions[hash][partition_index].snd = input[start_index+i].snd;
        partition_indecies[hash]++;
    }

    
    (*output).partitions = partitions;
    (*output).partition_sizes = partition_sizes;


}

uint64* create_args(struct tuple* input, struct partition_info* output, uint64 start_index, uint64 work_size, uint64 partition_count) {
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
    partition_sequential(*(struct tuple**)args, *(struct partition_info**)(args+1), *(uint64*)(args+2), *(uint64*)(args+3), *(uint64*)(args+4));
    free(data);
    return NULL;
}

void partition_individual_output(struct tuple input[], struct partition_info* output, uint64 input_size, uint64 threads, uint64 partition_count) {
    uint64 work_pr_thread = input_size / threads;
    struct partition_info thread_buffers[threads];
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
    

    int* partition_sizes = malloc(sizeof(int) * partition_count);
    struct tuple** partitions = malloc(8 * partition_count);
    for(int i = 0; i < partition_count; i++) {
        partition_sizes[i] = 0;
        for(int t = 0; t < threads; t++) partition_sizes[i] += thread_buffers[t].partition_sizes[i];
        partitions[i] = malloc(16 * partition_sizes[i]);
    }

    int partition_indecies[partition_count];
    for(int i = 0; i < partition_count; i++) partition_indecies[i] = 0;

    for(int t = 0; t < threads; t++) {
        for(int i = 0; i < partition_count; i++) {
            int partition_size = thread_buffers[t].partition_sizes[i];
            for(int s = 0; s < partition_size; s++) {
                int partition_index = partition_indecies[i];

                partitions[i][partition_index].fst = thread_buffers[t].partitions[i][s].fst;
                partitions[i][partition_index].snd = thread_buffers[t].partitions[i][s].snd;
                partition_indecies[i]++;
            }
            free(thread_buffers[t].partitions[i]);
        }
        free(thread_buffers[t].partitions);
        free(thread_buffers[t].partition_sizes);
    }

    (*output).partitions = partitions;
    (*output).partition_sizes = partition_sizes;
}

int main() {
// Setup
    int problem_size = 1000000;
    uint64 b = 8;
    uint64 partition_count = 1llu << b;

    init_utils(partition_count);

// Generate data
    struct tuple* data = malloc(16 * problem_size);
    char buffer[8];
    for(int i = 0; i < problem_size; i++) {
        getrandom(&buffer, 8, 0);
        data[i].fst = *(uint64*)&buffer;
        data[i].snd = i;
    }
    
// Calculate partitions
    struct partition_info partitions = { NULL,NULL };
    // partition_sequential(data, &partitions, 0, problem_size, partition_count);
    partition_individual_output(data, &partitions, problem_size, 2, partition_count);

// Print
    // printf("Input:\n");
    // for(int i = 0; i < problem_size; i++) {
    //     printf("    %llu -> %llu\n", data[i].fst, data[i].snd);
    // }

    printf("Output:\n");
    print_partitions(partitions, partition_count, 0);
}