#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include "../../utilities/types.h"
#include "./cleaning-merge-sort.h"

void cleaning_seq_merge_sort(uint64** array, uint64 size) {
    if (size <= 1) return;
    uint64 l_size = size / 2;
    uint64* l_array = (uint64*)malloc(sizeof(uint64) * l_size);
    memcpy(l_array, *array, sizeof(uint64) * l_size);

    uint64 r_size = size - l_size;
    uint64* r_array = (uint64*)malloc(sizeof(uint64) * r_size);
    memcpy(r_array, (*array) + l_size, sizeof(uint64) * r_size);

    free(*array);
    cleaning_seq_merge_sort(&l_array, l_size);
    cleaning_seq_merge_sort(&r_array, r_size);
    *array = (uint64*)malloc(sizeof(uint64) * size);

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

void* proxy_cleaning_args(uint64** array, uint64 size, int limiter) {
    void* data = malloc(20);
    *(uint64***)data = array;
    *(uint64*)(data+8) = size;
    *(int*)(data+16) = limiter;
    return data;
}

void* cleaning_par_merge_sort(void* data) {
    uint64** array = *(uint64***)data;
    uint64 size = *(uint64*)(data+8);
    int limiter = *(int*)(data+16);
    if (!limiter) {
        cleaning_seq_merge_sort(array, size);
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

    free(*array);
    pthread_t l_pid;
    pthread_t r_pid;
    pthread_create(&l_pid, NULL, cleaning_par_merge_sort, proxy_cleaning_args(&l_array, l_size, limiter-1));
    pthread_create(&r_pid, NULL, cleaning_par_merge_sort, proxy_cleaning_args(&r_array, r_size, limiter-1));
    pthread_join(l_pid, NULL);
    pthread_join(r_pid, NULL);
    *array = (uint64*)malloc(sizeof(uint64) * size);

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

void* better_cleaning_par_merge_sort(void* data) {
    uint64** array = *(uint64***)data;
    uint64 size = *(uint64*)(data+8);
    int limiter = *(int*)(data+16);
    if (!limiter) {
        cleaning_seq_merge_sort(array, size);
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

    free(*array);
    pthread_t child_pid;
    pthread_create(&child_pid, NULL, cleaning_par_merge_sort, proxy_cleaning_args(&r_array, r_size, limiter-1));
    cleaning_par_merge_sort(proxy_cleaning_args(&l_array, l_size, limiter-1));
    pthread_join(child_pid, NULL);

    *array = (uint64*)malloc(sizeof(uint64) * size);

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