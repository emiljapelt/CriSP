#include <sys/random.h>
#include <stdio.h>

#include "types.h"

int main() {
    
    uint64 data_points = 100000;

    printf("#include \"data_loader.h\"\n\n");

    printf("void load_problem_size(unsigned long long* dest){\n");
    printf("\t*dest = %llu;\n", data_points);
    printf("}\n\n");

    printf("void load_data(unsigned long long* dest){\n");
    char buffer[8];
    uint64 i = 0;
    while(i < data_points) {
        getrandom(&buffer, 8, 0);
        printf("\tdest[2 * %llu] = %llullu;\n", i, *(uint64*)&buffer);
        printf("\tdest[(2 * %llu) + 1] = %llullu;\n", i, i);
        i++;
    }
    printf("}\n");

    return 0;
}