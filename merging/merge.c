#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

void array_print(int* array, int size) {
    for(int i = 0; i < size; i++) printf("%i, ", array[i]);
    printf("\n");
}

void seq_merge_sort(int* array, int size) {
    if (size <= 1) return;
    int l_size = size / 2;
    int* l_array = malloc(sizeof(int) * l_size);
    memcpy(l_array, array, sizeof(int) * l_size);

    int r_size = size - l_size;
    int* r_array = malloc(sizeof(int) * r_size);
    memcpy(r_array, array + l_size, sizeof(int) * r_size);

    seq_merge_sort(l_array, l_size);
    seq_merge_sort(r_array, r_size);

    int i, l, r;
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
    while (l < l_size) {
        array[i] = l_array[l];
        i++; l++;
    }
    while (r < r_size) {
        array[i] = r_array[r];
        i++; r++;
    }

    free(l_array);
    free(r_array);
}

void* proxy_args(int* array, int size, int limiter) {
    char* data = malloc(16);
    *(int**)data = array;
    *(int*)(data+8) = size;
    *(int*)(data+12) = limiter;
    return data;
}

void* par_merge_sort(void* data) {
    int* array = *(int**)data;
    int size = *(int*)(data+8);
    int limiter = *(int*)(data+12);
    if (size <= limiter) {
        seq_merge_sort(array, size);
        return NULL;
    };

    int l_size = size / 2;
    int* l_array = malloc(sizeof(int) * l_size);
    memcpy(l_array, array, sizeof(int) * l_size);

    int r_size = size - l_size;
    int* r_array = malloc(sizeof(int) * r_size);
    memcpy(r_array, array + l_size, sizeof(int) * r_size);

    pthread_t l_pid;
    pthread_t r_pid;
    pthread_create(&l_pid, NULL, par_merge_sort, proxy_args(l_array, l_size, limiter));
    pthread_create(&r_pid, NULL, par_merge_sort, proxy_args(r_array, r_size, limiter));
    pthread_join(l_pid, NULL);
    pthread_join(r_pid, NULL);

    int i, l, r;
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
    while (l < l_size) {
        array[i] = l_array[l];
        i++; l++;
    }
    while (r < r_size) {
        array[i] = r_array[r];
        i++; r++;
    }

    free(data);
    free(l_array);
    free(r_array);
}

int main() {

    int size = 9;
    int array[] = { 2, 4, 5, 1, 124, 51, 12, 5, 895 };

    array_print(array, size);
    printf("\n");
    par_merge_sort(proxy_args(array, size, 4));
    array_print(array, size);

    return 0;
}