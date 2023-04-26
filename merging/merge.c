#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <limits.h>

#include "../utilities/data-gen.h"
#include "../utilities/types.h"
#include "../utilities/utils.h"

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

    if (size <= limiter) {
        general_seq_merge_sort(array, size, split);
        return NULL;
    }
    if (size < split) {
        general_seq_merge_sort(array, size, size);
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

    for(int i = 0; i < split; i++) pthread_create(pids + i, NULL, general_par_merge_sort, proxy_general_args(arrays[i], sizes[i], split, limiter));
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

void seq_merge_sort(uint64* array, uint64 size) {
    if (size <= 1) return;
    uint64 l_size = size / 2;
    uint64* l_array = (uint64*)malloc(sizeof(uint64) * l_size);
    memcpy(l_array, array, sizeof(uint64) * l_size);

    uint64 r_size = size - l_size;
    uint64* r_array = (uint64*)malloc(sizeof(uint64) * r_size);
    memcpy(r_array, array + l_size, sizeof(uint64) * r_size);

    seq_merge_sort(l_array, l_size);
    seq_merge_sort(r_array, r_size);

    uint64 i, l, r;
    i = 0; l = 0; r = 0;
    while(l < l_size && r < r_size) {
        if (l_array[l] < r_array[r]) {
            array[i] = l_array[l];
            l++; i++;
        }
        else {
            array[i] = r_array[r];
            r++; i++;
        }
    }
    if (l < l_size) {
        memcpy(array + i, l_array + l, sizeof(uint64) * (l_size - l));
    }
    if (r < r_size) {
        memcpy(array + i, r_array + r, sizeof(uint64) * (r_size - r));
    }

    free(l_array);
    free(r_array);
}

void* proxy_args(uint64* array, uint64 size, int limiter) {
    void* data = malloc(20);
    *(uint64**)data = array;
    *(uint64*)(data+8) = size;
    *(int*)(data+16) = limiter;
    return data;
}

void* par_merge_sort(void* data) {
    uint64* array = *(uint64**)data;
    uint64 size = *(uint64*)(data+8);
    int limiter = *(int*)(data+16);
    if (size <= limiter) {
        seq_merge_sort(array, size);
        return NULL;
    };

    uint64 l_size = size / 2;
    uint64* l_array = (uint64*)malloc(sizeof(uint64) * l_size);
    memcpy(l_array, array, sizeof(uint64) * l_size);

    uint64 r_size = size - l_size;
    uint64* r_array = (uint64*)malloc(sizeof(uint64) * r_size);
    memcpy(r_array, array + l_size, sizeof(uint64) * r_size);

    pthread_t l_pid;
    pthread_t r_pid;
    pthread_create(&l_pid, NULL, par_merge_sort, proxy_args(l_array, l_size, limiter));
    pthread_create(&r_pid, NULL, par_merge_sort, proxy_args(r_array, r_size, limiter));
    pthread_join(l_pid, NULL);
    pthread_join(r_pid, NULL);

    uint64 i, l, r;
    i = 0; l = 0; r = 0;
    while(l < l_size && r < r_size) {
        if (l_array[l] < r_array[r]) {
            array[i] = l_array[l];
            l++; i++;
        }
        else {
            array[i] = r_array[r];
            r++; i++;
        }
    }
    if (l < l_size) {
        memcpy(array + i, l_array + l, sizeof(uint64) * (l_size - l));
    }
    if (r < r_size) {
        memcpy(array + i, r_array + r, sizeof(uint64) * (r_size - r));
    }

    free(data);
    free(l_array);
    free(r_array);
    return NULL;
}

int main() {
    uint64 problem_size = 100000;

    uint64* array;
    generate_data(&array, problem_size);
  
    // array_print(array, size);
    // par_merge_sort(proxy_args(array, size, 4));
    // general_par_merge_sort(proxy_general_args(array, size, 4, 100));
    par_merge_sort(proxy_args(array, problem_size, 10));

    if (is_sorted(array, problem_size)) printf("true\n");
    else printf("false\n");

    // array_print(array, size);

    return 0;
}