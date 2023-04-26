#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../utilities/data-gen.h"
#include "../utilities/types.h"
#include "../utilities/utils.h"

#include "./implementations/expanding-merge-sort.h"
#include "./implementations/cleaning-merge-sort.h"
#include "./implementations/general-merge-sort.h"


int main() {
    uint64 problem_size = 10000000;
    uint64* base_array;
    uint64* array = (uint64*)malloc(sizeof(uint64) * problem_size);

    struct timespec start, finish;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    generate_merging_data(&base_array, problem_size);
    clock_gettime(CLOCK_MONOTONIC_RAW, &finish);
    long elapsed_time_ms = (finish.tv_sec - start.tv_sec) * 1000 + (finish.tv_nsec - start.tv_nsec) / 1000000;
    // printf("Data generation of %llu, elapsed time: %lu ms\n", problem_size, elapsed_time_ms);

    for(int limit = 1; limit < 4; limit++) {
        memcpy(array, base_array, sizeof(uint64) * problem_size);
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        general_seq_merge_sort(array, problem_size, 2);
        clock_gettime(CLOCK_MONOTONIC_RAW, &finish);
        elapsed_time_ms = (finish.tv_sec - start.tv_sec) * 1000 + (finish.tv_nsec - start.tv_nsec) / 1000000;
        printf("G: %lu\n", elapsed_time_ms);
        if (!is_sorted(array, problem_size)) {printf("Failure!\n");return-1;}
    }
    for(int limit = 1; limit < 4; limit++) {
        memcpy(array, base_array, sizeof(uint64) * problem_size);
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        general_seq_merge_sort(array, problem_size, 6);
        clock_gettime(CLOCK_MONOTONIC_RAW, &finish);
        elapsed_time_ms = (finish.tv_sec - start.tv_sec) * 1000 + (finish.tv_nsec - start.tv_nsec) / 1000000;
        printf("G: %lu\n", elapsed_time_ms);
        if (!is_sorted(array, problem_size)) {printf("Failure!\n");return-1;}
    }
    for(int limit = 1; limit < 4; limit++) {
        memcpy(array, base_array, sizeof(uint64) * problem_size);
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        expanding_seq_merge_sort(array, problem_size);
        clock_gettime(CLOCK_MONOTONIC_RAW, &finish);
        elapsed_time_ms = (finish.tv_sec - start.tv_sec) * 1000 + (finish.tv_nsec - start.tv_nsec) / 1000000;
        printf("E: %lu\n", elapsed_time_ms);
        if (!is_sorted(array, problem_size)) {printf("Failure!\n");return-1;}
    }
    for(int limit = 1; limit < 4; limit++) {
        memcpy(array, base_array, sizeof(uint64) * problem_size);
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        cleaning_seq_merge_sort(&array, problem_size);
        clock_gettime(CLOCK_MONOTONIC_RAW, &finish);
        elapsed_time_ms = (finish.tv_sec - start.tv_sec) * 1000 + (finish.tv_nsec - start.tv_nsec) / 1000000;
        printf("C: %lu\n", elapsed_time_ms);
        if (!is_sorted(array, problem_size)) {printf("Failure!\n");return-1;}
    }   


    // for(int limit = 1; limit < 8; limit++) {
    //     memcpy(array, base_array, sizeof(uint64) * problem_size);
    //     clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    //     par_merge_sort(proxy_args(&array, problem_size, limit));
    //     clock_gettime(CLOCK_MONOTONIC_RAW, &finish);
    //     elapsed_time_ms = (finish.tv_sec - start.tv_sec) * 1000 + (finish.tv_nsec - start.tv_nsec) / 1000000;
    //     printf("P: %i, %lu\n", limit, elapsed_time_ms);
    //     if (!is_sorted(array, problem_size)){printf("Failure!\n"); return 2;}
    // }

// General sequential
    // for(int split = 2; split <= 16; split++) {
    //     memcpy(array, base_array, sizeof(uint64) * problem_size);
    //     clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    //     general_seq_merge_sort(array, problem_size, split);
    //     clock_gettime(CLOCK_MONOTONIC_RAW, &finish);
    //     elapsed_time_ms = (finish.tv_sec - start.tv_sec) * 1000 + (finish.tv_nsec - start.tv_nsec) / 1000000;
    //     printf("GS @ %i: %lu ms\n", split, elapsed_time_ms);
    // }

// General parallel
    // for(int limit = 1; limit <= 8; limit++) 
    //     for(int split = 2; split <= 16; split++) {
    //         uint64 time_sum_ms = 0;
    //         uint64 repeats = 4;
    //         for(int i = 0; i < repeats; i++) {
    //             memcpy(array, base_array, sizeof(uint64) * problem_size);
    //             clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    //             general_par_merge_sort(proxy_general_args(array, problem_size, split, limit));
    //             clock_gettime(CLOCK_MONOTONIC_RAW, &finish);
    //             time_sum_ms = (finish.tv_sec - start.tv_sec) * 1000 + (finish.tv_nsec - start.tv_nsec) / 1000000;
    //         }
    //         printf("%i, %i, %i, %llu,\n", split, limit, thread_count(limit, split), time_sum_ms/repeats);
    //         // printf("GP @ %i %i: %lu ms\n", split, limit, elapsed_time_ms);
    //     }
    free(array);
    free(base_array);
    return 0;
}