#include "../../utilities/types.h"

void cleaning_seq_merge_sort(uint64** array, uint64 size);

void* proxy_cleaning_args(uint64** array, uint64 size, int limiter);

void* cleaning_par_merge_sort(void* data);