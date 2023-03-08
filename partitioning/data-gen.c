#include <stdlib.h>
#include <sys/random.h>

#include "types.h"
#include "data-gen.h"

void generate_data_linear(uint64** target, uint64 problem_size) {
    uint64* data = malloc((2 * sizeof(uint64)) * problem_size);
    char buffer[8];
    for(int i = 0; i < problem_size; i++) {
        getrandom(&buffer, 8, 0);
        data[2*i] = *(uint64*)&buffer;
        data[(2*i)+1] = i;
    }
    *target = data;
}

void generate_data_random(uint64** target, uint64 problem_size) {
    uint64 bytes_count = (2 * sizeof(uint64)) * problem_size;
    *target = (uint64*)malloc(bytes_count);
    getrandom(*(char**)target, bytes_count, 0);
}