#include "utils.h"

uint64 hash_cardinality;

void init_utils(uint64 partition_count) {
    hash_cardinality = partition_count;
}

// https://en.wikipedia.org/wiki/Hash_function
uint64 hash(uint64 input) {
    return input % hash_cardinality;
}