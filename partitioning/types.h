
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

struct partition_sequential_args
{
    struct tuple* input;
    struct partition_info* output;
    uint64 start_index;
    uint64 work_size;
    uint64 partition_count;
};