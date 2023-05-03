#include "../../utilities/types.h"

void seq_merge_sort(uint64** array, uint64 size, char cleaning);

void* proxy_args(uint64** array, uint64 size, int limiter, char cleaning);

void* par_merge_sort(void* data);
void* better_par_merge_sort(void* data);