#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../utilities/utils.h"
#include "./implementations/merge-sort.h"

void generateData(uint64** array, uint64 size) {
    srand(time(NULL)); 
    for (uint64 i = 0; i < size; i++) {
        (*array)[i] = rand();
    }
}

int isSorted(uint64* array, uint64 size) {
    uint64 current = array[0];
    for(int i = 1; i < size; i++) {
        if (array[i] >= current) continue;
        else return 0;
    }
    return 1;
}

int main() {
    int i = 100000000;
    uint64 *array = (uint64*)malloc(sizeof(uint64) * i);


    struct timespec start, finish;
    long elapsed_time_ms;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    generateData(&array, i);
    clock_gettime(CLOCK_MONOTONIC_RAW, &finish);
    elapsed_time_ms = (finish.tv_sec - start.tv_sec) * 1000 + (finish.tv_nsec - start.tv_nsec) / 1000000;
    printf("data elapsed: %lu,\n", elapsed_time_ms);

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    better_par_merge_sort(proxy_args(&array, i, 4, 1));;
    clock_gettime(CLOCK_MONOTONIC_RAW, &finish);
    elapsed_time_ms = (finish.tv_sec - start.tv_sec) * 1000 + (finish.tv_nsec - start.tv_nsec) / 1000000;
    printf("sort elapsed: %lu,\n", elapsed_time_ms);

    printf("is sorted: %d\n", isSorted(array, i));
    free(array);
    return 0;
}
