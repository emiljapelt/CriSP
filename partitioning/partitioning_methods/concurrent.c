#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include "../../utilities/types.h"
#include "concurrent.h"

void *create_args_concurrent(uint64 *input, uint64 *partitions, uint64 start_index, uint64 thread_section_size, uint64 partition_count, atomic_long *write_indeces, uint64 partition_size)
{
    uint64 *alloc = malloc(7 * sizeof(uint64));
    alloc[0] = (uint64)input;
    alloc[1] = (uint64)partitions;
    alloc[2] = start_index;
    alloc[3] = thread_section_size;
    alloc[4] = partition_count;
    alloc[5] = (uint64)write_indeces;
    alloc[6] = (uint64)partition_size;
    return alloc;
}

void *call_partition_concurrent(void *args)
{
    uint64 *input = *(uint64 **)args;
    uint64 *partitions = *(uint64 **)(args + 1 * 8);
    uint64 start_index = *(uint64 *)(args + 2 * 8);
    uint64 thread_section_size = *(uint64 *)(args + 3 * 8);
    uint64 partition_count = *(uint64 *)(args + 4 * 8);
    atomic_long *write_indeces = *(atomic_long **)(args + 5 * 8);
    uint64 partition_size = *(uint64 *)(args + 6 * 8);

    uint64 *thread_start = input + start_index;

    for (uint64 i = 0; i < thread_section_size / 2; i++)
    {
        uint64 *read_address = thread_start + i * 2;
        uint64 read_value = *read_address;
        uint64 read_value2 = *(read_address + 1);
        uint64 hash = read_value % partition_count;
        long write_index = atomic_fetch_add(&write_indeces[hash], 2);
        uint64 partition_write_index = hash * partition_size + write_index;
        partitions[partition_write_index] = read_value;
        partitions[partition_write_index + 1] = read_value2;
    }
    free(args);

    return NULL;
}

struct partition_info partition_concurrent_output(uint64 *input, uint64 input_size, uint64 thread_count, uint64 partition_count)
{
    int extra_buffer = input_size * 0.5; // in the paper they use extra buffers of 50% or more
    int partition_size = (input_size + extra_buffer) * 2 / partition_count; // times 2 because it is a tuple
    int thread_section_size = ((input_size + (thread_count - 1)) / thread_count) * 2;

    atomic_long *write_indeces = malloc(partition_count * sizeof(atomic_long));
    uint64 bytes_to_allocate = partition_size * partition_count * sizeof(uint64);
    uint64 *partitions = malloc(bytes_to_allocate);
    pthread_t threads[thread_count];

    for (int i = 0; i < partition_count; i++)
    {
        write_indeces[i] = 0;
    }

    for (int i = 0; i < thread_count; i++)
    {
        uint64 start_index = i * thread_section_size;
        if (i == thread_count - 1) {
            thread_section_size = (input_size * 2) - start_index;
        }
        void *args = create_args_concurrent(input, partitions, start_index, thread_section_size, partition_count, write_indeces, partition_size);
        pthread_create(&threads[i], NULL, call_partition_concurrent, args);
    }

    for (int i = 0; i < thread_count; i++)
    {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < partition_count; i++)
    {
        write_indeces[i] = write_indeces[i] / 2;
    }
    struct partition_info data = { partitions, (uint64*)write_indeces };
    return data;
}
