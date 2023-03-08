#include <pthread.h>
#include <stdatomic.h>
#include <stdlib.h>

#include "types.h"
#include "utils.h"
#include "count-then-move.h"

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

struct partition_info partition_count_then_move(uint64* input, uint64 input_size, uint64 threads, uint64 partition_count) {

    uint64* partitions = malloc((2 * sizeof(uint64)) * input_size);
    uint64* partition_sizes = malloc(sizeof(uint64) * partition_count);
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

        pthread_create(&thread_pids[t], NULL, call_count_then_move, create_args(t, threads, count_reports, input, partitions, start_index, work_size, partition_count, &counted));
    }

// Await full counting phase completion
    while(counted != threads);
    counted = -1;

    for(int t = 0; t < threads; t++) {
        pthread_join(thread_pids[t], NULL);
    }

// Write full partition sizes
    for(uint64 p = 0; p < partition_count; p++) {
        partition_sizes[p] = 0;
        for(uint64 t = 0; t < threads; t++)
            partition_sizes[p] += count_reports[(t*partition_count)+p];
    }
    
    free(count_reports);

    struct partition_info result = { partitions, partition_sizes };
    return result;
}
