#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include "../../utilities/types.h"
#include "./merge-sort.h"

void seq_merge_sort(uint64** array, uint64 size, char cleaning) {
    if (size <= 1) return;
    uint64 l_size = size / 2;
    uint64* l_array = (uint64*)malloc(sizeof(uint64) * l_size);
    memcpy(l_array, *array, sizeof(uint64) * l_size);

    uint64 r_size = size - l_size;
    uint64* r_array = (uint64*)malloc(sizeof(uint64) * r_size);
    memcpy(r_array, (*array) + l_size, sizeof(uint64) * r_size);

    if (cleaning) free(*array);
    seq_merge_sort(&l_array, l_size, cleaning);
    seq_merge_sort(&r_array, r_size, cleaning);
    if (cleaning) *array = (uint64*)malloc(sizeof(uint64) * size);

    uint64 i, l, r;
    i = 0; l = 0; r = 0;
    while(l < l_size && r < r_size) {
        if (l_array[l] < r_array[r]) {
            (*array)[i] = l_array[l];
            l++; i++;
        }
        else {
            (*array)[i] = r_array[r];
            r++; i++;
        }
    }
    if (l < l_size) {
        memcpy((*array) + i, l_array + l, sizeof(uint64) * (l_size - l));
    }
    if (r < r_size) {
        memcpy((*array) + i, r_array + r, sizeof(uint64) * (r_size - r));
    }

    free(l_array);
    free(r_array);
}

void* proxy_args(uint64** array, uint64 size, int limiter, int arity, char cleaning) {
    void* data = malloc(25);
    *(uint64***)data = array;
    *(uint64*)(data+8) = size;
    *(int*)(data+16) = limiter;
    *(int*)(data+20) = arity;
    *(char*)(data+24) = cleaning;
    return data;
}

void* par_merge_sort(void* data) {
    uint64** array = *(uint64***)data;
    uint64 size = *(uint64*)(data+8);
    int limiter = *(int*)(data+16);
    int arity = *(int*)(data+20);
    char cleaning = *(char*)(data+24);

    if (!limiter) {
        seq_merge_sort(array, size, cleaning);
        free(data);
        return NULL;
    };
    if (size <= 1) return NULL;

    uint64 l_size = size / 2;
    uint64* l_array = (uint64*)malloc(sizeof(uint64) * l_size);
    memcpy(l_array, *array, sizeof(uint64) * l_size);

    uint64 r_size = size - l_size;
    uint64* r_array = (uint64*)malloc(sizeof(uint64) * r_size);
    memcpy(r_array, (*array) + l_size, sizeof(uint64) * r_size);

    if (cleaning) free(*array);
    pthread_t l_pid;
    pthread_t r_pid;
    pthread_create(&l_pid, NULL, par_merge_sort, proxy_args(&l_array, l_size, limiter-1, arity, cleaning));
    pthread_create(&r_pid, NULL, par_merge_sort, proxy_args(&r_array, r_size, limiter-1, arity, cleaning));
    pthread_join(l_pid, NULL);
    pthread_join(r_pid, NULL);
    if (cleaning) *array = (uint64*)malloc(sizeof(uint64) * size);

    uint64 i, l, r;
    i = 0; l = 0; r = 0;
    while(l < l_size && r < r_size) {
        if (l_array[l] < r_array[r]) {
            (*array)[i] = l_array[l];
            l++; i++;
        }
        else {
            (*array)[i] = r_array[r];
            r++; i++;
        }
    }
    if (l < l_size) {
        memcpy((*array) + i, l_array + l, sizeof(uint64) * (l_size - l));
    }
    if (r < r_size) {
        memcpy((*array) + i, r_array + r, sizeof(uint64) * (r_size - r));
    }

    free(data);
    free(l_array);
    free(r_array);
    return NULL;
}

void* better_par_merge_sort(void* data) {
    uint64** array = *(uint64***)data;
    uint64 size = *(uint64*)(data+8);
    int limiter = *(int*)(data+16);
    int arity = *(int*)(data+20);
    char cleaning = *(char*)(data+24);

    if (!limiter) {
        seq_merge_sort(array, size, cleaning);
        free(data);
        return NULL;
    };
    if (size <= 1) return NULL;

    uint64* sizes = (uint64*)malloc(sizeof(uint64) * arity);
    uint64 total_size = 0;
    for (int i = 0; i < arity; i++) {
        sizes[i] = size / arity;
        total_size += sizes[i];
    }
    sizes[arity-1] += size - total_size;

    uint64** arrays = (uint64**)malloc(sizeof(uint64*) * arity);
    for (int i = 0; i < arity; i++) {
        arrays[i] = (uint64*)malloc(sizeof(uint64) * sizes[i]);
        memcpy(arrays[i], (*array) + (i * sizes[i]), sizeof(uint64) * sizes[i]);
    }

    if (cleaning) free(*array);

    pthread_t child_pids[arity];
    for (int i = 0; i < arity-1; i++) {
        pthread_create(&child_pids[i], NULL, better_par_merge_sort, proxy_args(&arrays[i], sizes[i], limiter-1, arity, cleaning));
    }
    better_par_merge_sort(proxy_args(&arrays[arity-1], sizes[arity-1], limiter-1, arity, cleaning));
    for (int i = 0; i < arity-1; i++) {
        pthread_join(child_pids[i], NULL);
    }

    if (cleaning) *array = (uint64*)malloc(sizeof(uint64) * size);

    uint64* indices = (uint64*)malloc(sizeof(uint64) * arity);
    memset(indices, 0, sizeof(uint64) * arity);

    uint64 i = 0;
    while (i < size) {
        // uint64 min_val = UINT64_MAX;
        uint64 min_val = 230584300921369395;
        int min_index = -1;
        for (int j = 0; j < arity; j++) {
            if (indices[j] < sizes[j] && arrays[j][indices[j]] < min_val) {
                min_val = arrays[j][indices[j]];
                min_index = j;
            }
        }
        (*array)[i] = min_val;
        indices[min_index]++;
        i++;
    }

    free(data);
    free(sizes);
    for (int i = 0; i < arity; i++) {
        free(arrays[i]);
    }
    free(arrays);
    free(indices);

    return NULL;
}
