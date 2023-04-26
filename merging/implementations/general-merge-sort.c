#include <limits.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include "../../utilities/types.h"
#include "./general-merge-sort.h"

void general_seq_merge_sort(uint64* array, uint64 size, int split) {
    if (size == 1) return;
    if (size < split) {
        general_seq_merge_sort(array, size, size);
        return;
    }

    uint64 sizes[split];
    uint64* arrays[split];

    uint64 normal_size = size / split;
    for(int i = 0; i < split-1; i++) sizes[i] = normal_size;
    sizes[split-1] = size - ((split-1)*normal_size);
    uint64 offset = 0;
    for(int i = 0; i < split; i++) {
        arrays[i] = (uint64*) malloc(sizeof(uint64) * sizes[i]);
        memcpy(arrays[i], array + offset, sizeof(uint64)*sizes[i]);
        offset += sizes[i];
    }

    for(int i = 0; i < split; i++) general_seq_merge_sort(arrays[i], sizes[i], split);
    uint64 merge_indecies[split];
    for(int i = 0; i < split; i++) merge_indecies[i] = 0;
    uint64 idx = 0;
    while(1) {
        uint64 guess = ULLONG_MAX;
        int guess_from = -1;
        for(int i = 0; i < split; i++) {
            if (merge_indecies[i] < sizes[i] && arrays[i][merge_indecies[i]] < guess) { 
                guess = arrays[i][merge_indecies[i]]; 
                guess_from = i;
            }
        }
        if (guess_from == -1) break;
        array[idx++] = guess;
        merge_indecies[guess_from]++;
        // guess_from = -1;
    } 

    for(int i = 0; i < split; i++) free(arrays[i]);
}

void* proxy_general_args(uint64* array, uint64 size, int split, int limiter) {
    void* data = malloc(24);
    *(uint64**)data = array;
    *(uint64*)(data+8) = size;
    *(int*)(data+16) = split;
    *(int*)(data+20) = limiter;
    return data;
}

void* general_par_merge_sort(void* data) {
    uint64* array = *(uint64**)data;
    uint64 size = *(uint64*)(data+8);
    int split = *(int*)(data+16);
    int limiter = *(int*)(data+20);

    if (!limiter) {
        general_seq_merge_sort(array, size, split);
        free(data);
        return NULL;
    }
    if (size < split) {
        general_seq_merge_sort(array, size, size);
        free(data);
        return NULL;
    }

    uint64 sizes[split];
    uint64* arrays[split];
    pthread_t pids[split];
    uint64 merge_indecies[split];

    uint64 normal_size = size / split;
    for(int i = 0; i < split-1; i++) sizes[i] = normal_size;
    sizes[split-1] = size - ((split-1)*normal_size);
    uint64 offset = 0;
    for(int i = 0; i < split; i++) {
        arrays[i] = (uint64*)malloc(sizeof(uint64) * sizes[i]);
        memcpy(arrays[i], array + offset, sizeof(uint64)*sizes[i]);
        offset += sizes[i];
    }

    for(int i = 0; i < split; i++) pthread_create(pids + i, NULL, general_par_merge_sort, proxy_general_args(arrays[i], sizes[i], split, limiter-1));
    for(int i = 0; i < split; i++) merge_indecies[i] = 0;
    for(int i = 0; i < split; i++) pthread_join(pids[i], NULL);

    int idx = 0;
    while(1) {
        uint64 guess = INT_MAX;
        int guess_from = -1;
        for(int i = 0; i < split; i++) {
            if (merge_indecies[i] == sizes[i]) continue;
            else if (arrays[i][merge_indecies[i]] < guess) { 
                guess = arrays[i][merge_indecies[i]]; 
                guess_from = i;
            }
        }
        if (guess_from == -1) break;
        array[idx++] = guess;
        merge_indecies[guess_from]++;
        guess_from = -1;
    } 

    for(int i = 0; i < split; i++) free(arrays[i]);
    free(data);
    return NULL;
}