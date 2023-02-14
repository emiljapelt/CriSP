#include "utils.h"

// https://en.wikipedia.org/wiki/Coprime_integers
// Branch 2
struct tuple coprime_1(unsigned int iters) {
    struct tuple result = { 2,1 };
    while(iters > 0) {
        uint64 fst = result.fst;
        result.fst = (fst << 1) + result.snd;
        result.snd = fst;
        iters--;
    }
    return result;
}

// Branch 2
struct tuple coprime_2(unsigned int iters) {
    struct tuple result = { 3,1 };
    while(iters > 0) {
        uint64 fst = result.fst;
        result.fst = (fst << 1) + result.snd;
        result.snd = fst;
        iters--;
    }
    return result;
};

struct tuple coprimes;

void init_utils() {
    coprimes = coprime_1(40);
}

// https://en.wikipedia.org/wiki/Hash_function
uint64 mult_hash(uint64 input, uint64 hash_cardinality) {
    return ((coprimes.snd * input) % coprimes.fst)/(coprimes.fst/hash_cardinality);
}