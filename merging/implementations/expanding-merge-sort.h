#include "../../utilities/types.h"

void expanding_seq_merge_sort(uint64* array, uint64 size);

void* proxy_expanding_args(uint64* array, uint64 size, int limiter);

void* expanding_par_merge_sort(void* data);