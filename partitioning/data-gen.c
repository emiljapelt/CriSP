#include <stdlib.h>
#include <stdio.h>
#include <sys/random.h>

#include "./pcg-c-0.94/include/pcg_variants.h"
#include "./pcg-c-0.94/extras/entropy.h"

#include "types.h"
#include "data-gen.h"

// Implemented using work found at https://www.pcg-random.org/
void generate_data(uint64** target, uint64 problem_size) {
    uint64* data = malloc((2 * sizeof(uint64)) * problem_size);

    pcg64_random_t rng;
    pcg128_t seeds[2];
    entropy_getbytes((void*)seeds, sizeof(seeds));
    pcg64_srandom_r(&rng, seeds[0], seeds[1]);

    for(int i = 0; i < problem_size; i++) {
        data[2*i] = pcg64_random_r(&rng);
    }

    *target = data;
}