#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <limits.h>

void array_print(int* array, int size) {
    for(int i = 0; i < size; i++) printf("%i, ", array[i]);
    printf("\n");
}

void general_seq_merge_sort(int* array, int size, int split) {
    if (size == 1) return;
    if (size < split) {
        general_seq_merge_sort(array, size, size);
        return;
    }

    int sizes[split];
    int* arrays[split];

    int normal_size = size / split;
    for(int i = 0; i < split-1; i++) sizes[i] = normal_size;
    sizes[split-1] = size - ((split-1)*normal_size);
    int offset = 0;
    for(int i = 0; i < split; i++) {
        arrays[i] = (int*) malloc(sizeof(int) * sizes[i]);
        memcpy(arrays[i], array + offset, sizeof(int)*sizes[i]);
        offset += sizes[i];
    }

    for(int i = 0; i < split; i++) general_seq_merge_sort(arrays[i], sizes[i], split);
    int merge_indecies[split];
    for(int i = 0; i < split; i++) merge_indecies[i] = 0;
    int idx = 0;
    while(1) {
        int guess = INT_MAX;
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

void* proxy_general_args(int* array, int size, int split, int limiter) {
    void* data = malloc(20);
    *(int**)data = array;
    *(int*)(data+8) = size;
    *(int*)(data+12) = split;
    *(int*)(data+16) = limiter;
    return data;
}

void* general_par_merge_sort(void* data) {
    int* array = *(int**)data;
    int size = *(int*)(data+8);
    int split = *(int*)(data+12);
    int limiter = *(int*)(data+16);

    if (size <= limiter) {
        general_seq_merge_sort(array, size, split);
        return NULL;
    }
    if (size < split) {
        general_seq_merge_sort(array, size, size);
        return NULL;
    }

    int sizes[split];
    int* arrays[split];
    pthread_t pids[split];
    int merge_indecies[split];

    int normal_size = size / split;
    for(int i = 0; i < split-1; i++) sizes[i] = normal_size;
    sizes[split-1] = size - ((split-1)*normal_size);
    int offset = 0;
    for(int i = 0; i < split; i++) {
        arrays[i] = (int*)malloc(sizeof(int) * sizes[i]);
        memcpy(arrays[i], array + offset, sizeof(int)*sizes[i]);
        offset += sizes[i];
    }

    for(int i = 0; i < split; i++) pthread_create(pids + i, NULL, general_par_merge_sort, proxy_general_args(arrays[i], sizes[i], split, limiter));
    for(int i = 0; i < split; i++) merge_indecies[i] = 0;
    for(int i = 0; i < split; i++) pthread_join(pids[i], NULL);

    int idx = 0;
    while(1) {
        int guess = INT_MAX;
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

void seq_merge_sort(int* array, int size) {
    if (size <= 1) return;
    int l_size = size / 2;
    int* l_array = (int*)malloc(sizeof(int) * l_size);
    memcpy(l_array, array, sizeof(int) * l_size);

    int r_size = size - l_size;
    int* r_array = (int*)malloc(sizeof(int) * r_size);
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
    void* data = malloc(16);
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
    int* l_array = (int*)malloc(sizeof(int) * l_size);
    memcpy(l_array, array, sizeof(int) * l_size);

    int r_size = size - l_size;
    int* r_array = (int*)malloc(sizeof(int) * r_size);
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
    return NULL;
}

char is_sorted(int* array, int size) {
    int current = array[0];
    for(int i = 1; i < size; i++) {
        if (array[i] >= current) continue;
        else return 0;
    }
    return 1;
}

void fill_with_random_ints(int* array, int n) {
    // fill
    for (int i = 0; i < n; i++)
        array[i] = i;

    // shuffle
    for (int i = 0; i < n; i++) {
        int j = rand() % n;
        // swap
        int ai = array[i];
        int aj = array[j];
        array[i] = aj;
        array[j] = ai;
    }
};

int not_main() {
    int size = 1000000;
    int array[size];

    fill_with_random_ints(array, size);

    // array_print(array, size);
    // par_merge_sort(proxy_args(array, size, 4));
    general_par_merge_sort(proxy_general_args(array, size, 4, 100));

    if (is_sorted(array, size)) printf("true\n");
    else printf("false\n");

    // array_print(array, size);

    return 0;
}