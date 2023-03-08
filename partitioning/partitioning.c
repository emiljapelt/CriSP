#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/random.h>
#include <sys/wait.h>
#include <pthread.h>
#include <stdatomic.h>
#include <unistd.h>
#include <time.h>

#include "utils.h"

void count_then_move(uint64 thread_id, uint64 thread_count, uint64* count_reports, uint64* input, uint64* output, uint64 start_index, uint64 work_size, uint64 partition_count, _Atomic uint64* counted) {

// Count partition contributions
    for(uint64 i = 0; i < partition_count; i++) count_reports[(thread_id*partition_count)+i] = 0;
    for(uint64 i = 0; i < work_size; i++) {
        count_reports[(thread_id*partition_count) + hash(input[start_index+(2*i)])]++;
    }

// Thread synchronization
    (*counted)++;
    while (*counted != -1);

// Calculate partition starting indecies
    uint64 partition_starts[partition_count];
    partition_starts[0] = 0;
    for(uint64 p = 1; p < partition_count; p++) {
        partition_starts[p] = partition_starts[p-1];
        for(uint64 t = 0; t < thread_count; t++) 
            partition_starts[p] += 2 * count_reports[(t*partition_count)+(p-1)];
    }

// Calculate initial partition offsets
    uint64 partition_offsets[partition_count];
    for(uint64 o = 0; o < partition_count; o++) partition_offsets[o] = 0;
    for(uint64 p = 0; p < partition_count; p++) {
        for(uint64 t = 0; t < thread_id; t++) partition_offsets[p] += 2*count_reports[(t*partition_count)+p];
    }

// Move data to output
    for(uint64 i = 0; i < work_size; i++) {
        uint64 h = hash(input[start_index+(2*i)]);
        output[partition_starts[h]+partition_offsets[h]] = input[start_index+(2*i)];
        output[partition_starts[h]+partition_offsets[h]+1] = input[start_index+(2*i)+1];
        partition_offsets[h] += 2;
    }
}

uint64* create_args(uint64 thread_id, uint64 thread_count, uint64* count_reports, uint64* input, uint64* output, uint64 start_index, uint64 work_size, uint64 partition_count, _Atomic uint64* counted) {
    uint64* alloc = malloc(9 * 8);
    
    alloc[0] = thread_id;
    alloc[1] = thread_count;
    alloc[2] = (uint64)count_reports;
    alloc[3] = (uint64)input;
    alloc[4] = (uint64)output;
    alloc[5] = start_index;
    alloc[6] = work_size;
    alloc[7] = partition_count;
    alloc[8] = (uint64)counted;

    return alloc;
}

void* call_count_then_move(void* data) {
    uint64* args = (uint64*)data;

    uint64 thread_id = *(uint64*)(args);
    uint64 thread_count = *(uint64*)(args+1);
    uint64* count_reports = *(uint64**)(args+2);
    uint64* input = *(uint64**)(args+3);
    uint64* output = *(uint64**)(args+4);
    uint64 start_index = *(uint64*)(args+5);
    uint64 work_size = *(uint64*)(args+6);
    uint64 partition_count = *(uint64*)(args+7);
    _Atomic uint64* counted = *(_Atomic uint64**)(args+8);

    count_then_move(thread_id, thread_count, count_reports, input, output, start_index, work_size, partition_count, counted);
    free(data);

    return NULL;
}

void partition_count_then_move(uint64* input, uint64** output, uint64 input_size, uint64 threads, uint64 partition_count) {

    uint64* partitions = malloc((sizeof(uint64) * partition_count) + ((2 * sizeof(uint64)) * input_size));
    uint64* count_reports = malloc(sizeof(uint64) * threads * partition_count);
    _Atomic uint64 counted = 0;

// Start worker threads
    uint64 work_pr_thread = input_size / threads;
    pthread_t thread_pids[threads];
    for(int t = 0; t < threads; t++) {
        uint64 start_index = t * work_pr_thread;
        uint64 work_size;
        
        if(t == threads - 1) work_size = input_size - start_index;
        else work_size = work_pr_thread;

        pthread_create(&thread_pids[t], NULL, call_count_then_move, create_args(t, threads, count_reports, input, partitions+partition_count, start_index, work_size, partition_count, &counted));
    }

// Await full counting phase completion
    while(counted != threads);
    counted = -1;

    for(int t = 0; t < threads; t++) {
        pthread_join(thread_pids[t], NULL);
    }

// Write full partition sizes
    for(uint64 p = 0; p < partition_count; p++) {
        partitions[p] = 0;
        for(uint64 t = 0; t < threads; t++)
            partitions[p] += count_reports[(t*partition_count)+p];
    }
    
    free(count_reports);

    *output = partitions;
}

void generate_data_linear(uint64** target, uint64 problem_size) {
    uint64* data = malloc((2 * sizeof(uint64)) * problem_size);
    char buffer[8];
    for(int i = 0; i < problem_size; i++) {
        getrandom(&buffer, 8, 0);
        data[2*i] = *(uint64*)&buffer;
        data[(2*i)+1] = i;
    }
    *target = data;
}

void generate_data_random(uint64** target, uint64 problem_size) {
    uint64 bytes_count = (2 * sizeof(uint64)) * problem_size;
    *target = (uint64*)malloc(bytes_count);
    getrandom(*(char**)target, bytes_count, 0);
}

long time_run(uint64* partitions, uint64 *data, int problem_size, int b, int thread_count) {
    struct timespec start, finish;
    clock_gettime(CLOCK_MONOTONIC, &start);
    uint64 partition_count = 1llu << b;
    partition_count_then_move(data, &partitions, problem_size, thread_count, partition_count);
    clock_gettime(CLOCK_MONOTONIC, &finish);
    long elapsed_time_ms = (finish.tv_sec - start.tv_sec) * 1000 + (finish.tv_nsec - start.tv_nsec) / 1000000;
    return elapsed_time_ms;
}

int main() {
// Setup
    int problem_size = 16777216;
    uint64 b = 4;
    uint64 partition_count = 1llu << b;

    init_utils(partition_count);

// Generate data
    uint64* data;
    struct timespec start, finish;
    clock_gettime(CLOCK_MONOTONIC, &start);
    generate_data_random(&data, problem_size);
    clock_gettime(CLOCK_MONOTONIC, &finish);
    long elapsed_time_ms = (finish.tv_sec - start.tv_sec) * 1000 + (finish.tv_nsec - start.tv_nsec) / 1000000;
    printf("Data generation elapsed time: %lu ms\n", elapsed_time_ms);

    for (int bruh = 1; bruh <= 18; bruh++) {
    	uint64* partitions;
     	uint64 partition_count = 1llu << bruh;
        init_utils(partition_count);
     	print_partition_distribution(partitions, partition_count, 100);
        long time = time_run(partitions, data, problem_size, bruh, 32);
        printf("%i %ld\n", bruh, time);
    }

// // Calculate partitions
//     uint64* partitions;
//     partition_count_then_move(data, &partitions, problem_size, 1, partition_count);

// // Print
//     // print_partitions(partitions, partition_count, 1);
//     print_partition_distribution(partitions, partition_count, 100);
//     print_partition_statistic(partitions, partition_count);

    free(data);
}
