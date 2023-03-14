#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "types.h"
#include "utils.h"
#include "data-gen.h"
#include "count-then-move.h"
#include "concurrent.h"
#include "concurrent_mutex.h"

enum Algorithm {
    COUNT_THEN_MOVE,
    CONCURRENT_OUTPUT,
    CONCURRENT_MUTEX
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
        case CONCURRENT_MUTEX:
            clock_gettime(CLOCK_MONOTONIC_RAW, &start);
            result = partition_concurrent_output_mutex(data, problem_size, thread_count, partition_count);
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
        printf("b: %i\n", b);
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

void algorithm_correctness_check() {
    int problem_size = 16777216;
    uint64 b = 4;
    uint64* data;
    uint64 partition_count = 1llu << b;
    init_utils(partition_count);
    generate_data(&data, problem_size);

    struct partition_info concurrent_info = partition_concurrent_output(data, problem_size, 16, partition_count);
    // struct partition_info countmove_info = partition_count_then_move(data, problem_size, 16, partition_count);
    struct partition_info countmove_info = partition_concurrent_output_mutex(data, problem_size, 16, partition_count);
    print_partition_statistic(concurrent_info, partition_count);
    print_partition_statistic(countmove_info, partition_count);
    printf("# correct results\n");
    print_correct_stats(data, partition_count, problem_size);
    free(data);
}

void run_benchmarks() {
    int problem_size = 16777216;
    uint64 b = 4;
    uint64 partition_count = 1llu << b;
    init_utils(partition_count);

    uint64* data;
    struct timespec start, finish;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    generate_data(&data, problem_size);
    clock_gettime(CLOCK_MONOTONIC_RAW, &finish);
    long elapsed_time_ms = (finish.tv_sec - start.tv_sec) * 1000 + (finish.tv_nsec - start.tv_nsec) / 1000000;
    printf("Data generation elapsed time: %lu ms\n", elapsed_time_ms);

    printf("benching concurrent output\n");
    benchmark_all_combinations(CONCURRENT_OUTPUT, data, problem_size, "./benchmark_data/speed_atomic.csv", 2, 18, 32);
    printf("benching count-then-move\n");
    // benchmark_all_combinations(COUNT_THEN_MOVE, data, problem_size, "./benchmark_data/10-3_count-then-move.csv", 8, 18, 32);
    // benchmark_all_combinations(CONCURRENT_MUTEX, data, problem_size, "./benchmark_data/speed_mutex.csv", 2, 18, 32);
    free(data);
}

int main() {
    // run_benchmarks();
    algorithm_correctness_check();
    return 0;
}
