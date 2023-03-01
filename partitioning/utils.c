#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

uint64 hash_cardinality;

void init_utils(uint64 partition_count) {
    hash_cardinality = partition_count;
}

uint64 hash(uint64 input) {
    return input % hash_cardinality;
}

void print_partitions(uint64* partitions, int partition_count, char with_contents) {
    uint64 offset = partition_count;
    for(int p = 0; p < partition_count; p++) {
        uint64 partition_size = partitions[p];
        printf("# partition %d: [%lld]\n", p, partition_size);
        if (with_contents) for(int s = 0; s < partition_size; s++) {
            printf("    %llu -> %lld\n",  partitions[offset+2*s], partitions[offset+(2*s)+1]);
        }

        offset += 2 * partition_size;
    }
}

int decimal_digits(int number) {
    int digits;
    if (number <= 0) digits = 1;
    else digits = 0;

    while (number != 0) {
        number = number / 10;
        digits++;
    }
    
    return digits;
}

void print_partition_distribution(uint64* partitions, int partition_count, int levels) {
    printf("# distribution\n");

    // int pc = partition_count;
    int max_digits = decimal_digits(partition_count-1);

    uint64 max_partition_size = partitions[0];
    for(int i = 1; i < partition_count; i++) if (partitions[i] > max_partition_size) max_partition_size = partitions[i];
    double level_size = ((double)max_partition_size) / levels;
    for(int p = 0; p < partition_count; p++) {
        uint64 partition_level = (uint64)(partitions[p] / level_size);
        int extra_spacing = max_digits - decimal_digits(p);
        printf("%i", p);
        printf(": ");
        while (extra_spacing > 0) { printf(" "); extra_spacing--; }
        while (partition_level > 0) { printf("-"); partition_level--; }
        printf("\n");
    }
}

void print_partition_statistic(uint64* partitions, int partition_count) {
    printf("# statistics\n");

    uint64 max_partition_size = partitions[0];
    for(int i = 1; i < partition_count; i++) if (partitions[i] > max_partition_size) max_partition_size = partitions[i];
    uint64 min_partition_size = partitions[0];
    for(int i = 1; i < partition_count; i++) if (partitions[i] < min_partition_size) min_partition_size = partitions[i];
    uint64 size_sum = 0;
    for(int i = 0; i < partition_count; i++) size_sum += partitions[i];
    double expected_average = (double)size_sum/partition_count;
    double deviation_sum = 0;
    for(int i = 0; i < partition_count; i++) deviation_sum += abs(expected_average - partitions[i]);

    printf("sum: %lld\n", size_sum);
    printf("max: %lld\n", max_partition_size);
    printf("min: %lld\n", min_partition_size);
    printf("exp-avg: %f\n", expected_average);
    printf("avg-dev: %.3f%%\n", ((deviation_sum/partition_count)/expected_average)*100);
}