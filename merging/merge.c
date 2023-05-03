#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../utilities/data-gen.h"
#include "../utilities/types.h"
#include "../utilities/utils.h"

#include "./implementations/merge-sort.h"
#include "./implementations/merge-sort.h"
#include "./implementations/general-merge-sort.h"


int main() {

    uint64 max_problem_size = 30000000;
    uint64 increment = 1000000;
    uint64 starter = 1000000;
    uint64* array;

    struct timespec start, finish;
    long elapsed_time_ms;

    printf("old clean:\n");
    for(uint64 i = starter; i <= max_problem_size; i += increment) {
        array = (uint64*)malloc(sizeof(uint64) * i);
        generate_merging_data(&array, i);

        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        par_merge_sort(proxy_args(&array, i, 2, 1));
        clock_gettime(CLOCK_MONOTONIC_RAW, &finish);
        elapsed_time_ms = (finish.tv_sec - start.tv_sec) * 1000 + (finish.tv_nsec - start.tv_nsec) / 1000000;
        printf("%lu,\n", elapsed_time_ms);

        free(array);
    }

    printf("new clean:\n");
    for(uint64 i = starter; i <= max_problem_size; i += increment) {
        array = (uint64*)malloc(sizeof(uint64) * i);
        generate_merging_data(&array, i);

        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        better_par_merge_sort(proxy_args(&array, i, 2, 1));
        clock_gettime(CLOCK_MONOTONIC_RAW, &finish);
        elapsed_time_ms = (finish.tv_sec - start.tv_sec) * 1000 + (finish.tv_nsec - start.tv_nsec) / 1000000;
        printf("%lu,\n", elapsed_time_ms);

        free(array);
    }

    printf("old expand:\n");
    for(uint64 i = starter; i <= max_problem_size; i += increment) {
        array = (uint64*)malloc(sizeof(uint64) * i);
        generate_merging_data(&array, i);

        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        par_merge_sort(proxy_args(&array, i, 2, 0));
        clock_gettime(CLOCK_MONOTONIC_RAW, &finish);
        elapsed_time_ms = (finish.tv_sec - start.tv_sec) * 1000 + (finish.tv_nsec - start.tv_nsec) / 1000000;
        printf("%lu,\n", elapsed_time_ms);

        free(array);
    }

    printf("new expand:\n");
    for(uint64 i = starter; i <= max_problem_size; i += increment) {
        array = (uint64*)malloc(sizeof(uint64) * i);
        generate_merging_data(&array, i);

        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        better_par_merge_sort(proxy_args(&array, i, 2, 0));
        clock_gettime(CLOCK_MONOTONIC_RAW, &finish);
        elapsed_time_ms = (finish.tv_sec - start.tv_sec) * 1000 + (finish.tv_nsec - start.tv_nsec) / 1000000;
        printf("%lu,\n", elapsed_time_ms);

        free(array);
    }

    printf("old general:\n");
    for(uint64 i = starter; i <= max_problem_size; i += increment) {
        array = (uint64*)malloc(sizeof(uint64) * i);
        generate_merging_data(&array, i);

        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        general_par_merge_sort(proxy_general_args(array, i, 2, 2));
        clock_gettime(CLOCK_MONOTONIC_RAW, &finish);
        elapsed_time_ms = (finish.tv_sec - start.tv_sec) * 1000 + (finish.tv_nsec - start.tv_nsec) / 1000000;
        printf("%lu,\n", elapsed_time_ms);

        free(array);
    }

    printf("new general:\n");
    for(uint64 i = starter; i <= max_problem_size; i += increment) {
        array = (uint64*)malloc(sizeof(uint64) * i);
        generate_merging_data(&array, i);

        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        better_general_par_merge_sort(proxy_general_args(array, i, 2, 2));
        clock_gettime(CLOCK_MONOTONIC_RAW, &finish);
        elapsed_time_ms = (finish.tv_sec - start.tv_sec) * 1000 + (finish.tv_nsec - start.tv_nsec) / 1000000;
        printf("%lu,\n", elapsed_time_ms);

        free(array);
    }

    return 0;
}