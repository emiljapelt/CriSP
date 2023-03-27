#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "types.h"

uint64 hash_cardinality;

void init_utils(uint64 partition_count) {
    hash_cardinality = partition_count;
}

uint64 hash(uint64 input) {
    return input % hash_cardinality;
}

void print_partitions(struct partition_info info, int partition_count, char with_contents) {
    uint64 offset = 0;
    for(int p = 0; p < partition_count; p++) {
        uint64 partition_size = info.partition_sizes[p];
        printf("# partition %d: [%lld]\n", p, partition_size);
        if (with_contents) for(int s = 0; s < partition_size; s++) {
            printf("    %llu -> %lld\n",  info.partitions[offset+2*s], info.partitions[offset+(2*s)+1]);
        }

        offset += 2 * partition_size;
        while(info.partitions[offset] == 0) offset += 2;
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

void print_partition_distribution(struct partition_info info, int partition_count, int levels) {
    printf("# distribution\n");

    // int pc = partition_count;
    int max_digits = decimal_digits(partition_count-1);

    uint64 max_partition_size = info.partition_sizes[0];
    for(int i = 1; i < partition_count; i++) if (info.partition_sizes[i] > max_partition_size) max_partition_size = info.partition_sizes[i];
    double level_size = ((double)max_partition_size) / levels;
    for(int p = 0; p < partition_count; p++) {
        uint64 partition_level = (uint64)(info.partition_sizes[p] / level_size);
        int extra_spacing = max_digits - decimal_digits(p);
        printf("%i", p);
        printf(": ");
        while (extra_spacing > 0) { printf(" "); extra_spacing--; }
        while (partition_level > 0) { printf("-"); partition_level--; }
        printf("\n");
    }
}

void print_correct_stats(uint64 *data, uint64 partition_count, uint64 problem_size) {
    uint64 counts[partition_count];
    for (int i = 0; i < partition_count; i++) counts[i] = 0;
    for (int i = 0; i < problem_size; i++) {
        uint64 hash = data[2*i] % partition_count;
        counts[hash]++;
    }

    uint64 max_size = 0;
    uint64 min_size = 1 << 30;

    for (int i = 0; i < partition_count; i++) {
        if (counts[i] > max_size) max_size = counts[i];
        if (counts[i] < min_size) min_size = counts[i];
    }
    printf("max: %lli\n", max_size);
    printf("min: %lli\n", min_size);
}

void print_partition_statistic(struct partition_info info, int partition_count) {
    printf("# statistics\n");

    uint64 max_partition_size = info.partition_sizes[0];
    uint64 min_partition_size = info.partition_sizes[0];
    for(int i = 1; i < partition_count; i++) {
        if (info.partition_sizes[i] > max_partition_size) max_partition_size = info.partition_sizes[i];
        if (info.partition_sizes[i] < min_partition_size) min_partition_size = info.partition_sizes[i];
    }
    uint64 size_sum = 0;
    for(int i = 0; i < partition_count; i++) size_sum += info.partition_sizes[i];
    double expected_average = (double)size_sum/partition_count;
    double deviation_sum = 0;
    for(int i = 0; i < partition_count; i++) deviation_sum += abs(expected_average - info.partition_sizes[i]);

    printf("sum: %lld\n", size_sum);
    printf("max: %lld\n", max_partition_size);
    printf("min: %lld\n", min_partition_size);
    printf("exp-avg: %f\n", expected_average);
    printf("avg-dev: %.3f%%\n", ((deviation_sum/partition_count)/expected_average)*100);
}