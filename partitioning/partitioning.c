#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "types.h"
#include "utils.h"
#include "data-gen.h"
#include "count-then-move.h"
#include "concurrent.h"

enum Algorithm {
    COUNT_THEN_MOVE,
    CONCURRENT_OUTPUT
};

long time_run(int algorithm, uint64 *data, int problem_size, uint64 partition_count, int thread_count) {
    struct timespec start, finish;
    struct partition_info result;

    switch (algorithm) {
        case CONCURRENT_OUTPUT:
            clock_gettime(CLOCK_MONOTONIC_RAW, &start);
            result = partition_concurrent_output(data, problem_size, thread_count, partition_count);
            clock_gettime(CLOCK_MONOTONIC_RAW, &finish);
            break;
        case COUNT_THEN_MOVE:
            init_utils(partition_count);
            clock_gettime(CLOCK_MONOTONIC_RAW, &start);
            result = partition_count_then_move(data, problem_size, thread_count, partition_count);
            clock_gettime(CLOCK_MONOTONIC_RAW, &finish);
            break;
        default:
            break;
    }

    long elapsed_time_ms = (finish.tv_sec - start.tv_sec) * 1000 + (finish.tv_nsec - start.tv_nsec) / 1000000;
    free(result.partition_sizes);
    free(result.partitions);
    return elapsed_time_ms;
}

void benchmark_all_combinations(int algorithm, uint64 *data, int problem_size, char *out_file_name, int repetitions, int up_to_hashbits, int up_to_threads) {
    FILE *fp;
    fp = fopen(out_file_name, "w");

    char top_row[30] = " ";
    for (int i = 1; i <= up_to_threads; i *= 2) {
        char as_string[4];
        sprintf(as_string, ",%i", i);
        strcat(top_row, as_string);
    }
    fprintf(fp, "%s\n", top_row);

    for (int b = 1; b <= up_to_hashbits; b++) {
        char gathered_data[1024] = "";
        sprintf(gathered_data, "%i", b);
        for (int t = 1; t <= up_to_threads; t *= 2) {
            long sum_time = 0;
            strcat(gathered_data, ",");
            for (int i = 0; i < repetitions; i++) {
                uint64 partition_count = 1llu << b;
                long time = time_run(algorithm, data, problem_size, partition_count, t);
                sum_time += time;
            }
            char as_string[10];
            sprintf(as_string, "%ld", (sum_time / repetitions));
            strcat(gathered_data, as_string);
        }
        strcat(gathered_data, "\n");
        fprintf(fp, "%s", gathered_data);
    }
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

    benchmark_all_combinations(CONCURRENT_OUTPUT, data, problem_size, "test_raw.csv", 2, 10, 16);
    // benchmark_all_combinations(COUNT_THEN_MOVE, data, problem_size, "lots_of_data_countmove.csv", 4, 18, 32);
// // Time count-then-move
//     long elapsed_time;
//     for (int run = 1; run <= 18; run++) {
//         elapsed_time = time_run(COUNT_THEN_MOVE, data, problem_size, partition_count, run);
//         printf("count-then-move #%i: %li ms\n", run, elapsed_time);
//     }

// // Time concurrent
//     for (int run = 1; run <= 18; run++) {
//         elapsed_time = time_run(CONCURRENT_OUTPUT, data, problem_size, partition_count, run);
//         printf("concurrent_output #%i: %li ms\n", run, elapsed_time);
//     }


// // Calculate partitions
//     uint64* partitions;
//     partition_count_then_move(data, &partitions, problem_size, 1, partition_count);

// // Print
//     // print_partitions(partitions, partition_count, 1);
//     print_partition_distribution(partitions, partition_count, 100);
//     print_partition_statistic(partitions, partition_count);

    free(data);
}
