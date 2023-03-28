#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "../utilities/types.h"
#include "../utilities/utils.h"
#include "../utilities/data-gen.h"
#include "../partitioning_methods/count-then-move.h"
#include "../partitioning_methods/concurrent.h"

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
    struct partition_info countmove_info = partition_count_then_move(data, problem_size, 16, partition_count);
    print_partition_statistic(concurrent_info, partition_count);
    print_partition_statistic(countmove_info, partition_count);
    printf("# correct results\n");
    print_correct_stats(data, partition_count, problem_size);
    free(data);
}

void run_benchmarks(char *dateString, uint64 problem_size) {
    uint64* data;
    struct timespec start, finish;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    generate_data(&data, problem_size);
    clock_gettime(CLOCK_MONOTONIC_RAW, &finish);
    long elapsed_time_ms = (finish.tv_sec - start.tv_sec) * 1000 + (finish.tv_nsec - start.tv_nsec) / 1000000;
    printf("Data generation elapsed time: %lu ms\n", elapsed_time_ms);

    char ctm_path[100] = "../benchmark_data/";
    strcat(ctm_path, dateString);
    strcat(ctm_path, "/COUNT_THEN_MOVE");
    strcat(ctm_path, "/timing.csv");

    char concu_path[100] = "../benchmark_data/";
    strcat(concu_path, dateString);
    strcat(concu_path, "/CONCURRENT_OUTPUT");
    strcat(concu_path, "/timing.csv");

    printf("benching concurrent output\n");
    benchmark_all_combinations(CONCURRENT_OUTPUT, data, problem_size, concu_path, 2, 18, 32);
    printf("benching count-then-move\n");
    benchmark_all_combinations(COUNT_THEN_MOVE, data, problem_size, ctm_path, 2, 18, 32);
    free(data);
}

int main(int argc, char **argv) {
    uint64 problem_size = 16777216;
    
    // method thread_count partition_count
    if (argc > 2) {
        uint64* data;
        generate_data(&data, problem_size);
        int method = atoi(argv[1]);
        int thread_count = atoi(argv[2]);
        int partition_count = atoi(argv[3]);
        if (method == COUNT_THEN_MOVE){
            init_utils(partition_count);
            partition_count_then_move(data, problem_size, thread_count, partition_count);
        } else {
            partition_concurrent_output(data, problem_size, thread_count, partition_count);
        }
        free(data);
    } else {
        run_benchmarks(argv[1], problem_size);
        // algorithm_correctness_check();
    }
    return 0;
}
