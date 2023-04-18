#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <limits.h>

void array_print(int* array, int size);

void general_seq_merge_sort(int* array, int size, int split);

void* proxy_general_args(int* array, int size, int split, int limiter);

void* general_par_merge_sort(void* data);

void seq_merge_sort(int* array, int size);

void* proxy_args(int* array, int size, int limiter);

void* par_merge_sort(void* data);

int not_main();