#include "../../utilities/types.h"

void general_seq_merge_sort(uint64* array, uint64 size, int split);

void* proxy_general_args(uint64* array, uint64 size, int split, int limiter);

void* general_par_merge_sort(void* data);