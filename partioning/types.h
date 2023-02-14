
typedef unsigned long long uint64;

struct tuple 
{
    uint64 fst;
    uint64 snd;
};

struct partition_info 
{
    struct tuple** partitions;
    int* partition_sizes;
};