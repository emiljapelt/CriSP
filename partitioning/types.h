
#ifndef TYPE_DEFS
#define TYPE_DEFS

typedef unsigned long long uint64;

struct partition_info 
{
    uint64* partitions;
    uint64* partition_sizes;
};

#endif