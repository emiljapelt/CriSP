#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/random.h>
#include <sys/wait.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <stdatomic.h>
#include "types.h"
#include <time.h>

void *create_args_mutex(uint64 *input, uint64 *partitions, pthread_mutex_t *mutexes, uint64 start_index, uint64 thread_section_size, uint64 partition_count, uint64 *write_indeces, uint64 partition_size, uint64 input_size)
{
    uint64 *alloc = malloc(9 * sizeof(uint64));
    alloc[0] = (uint64)input;
    alloc[1] = (uint64)partitions;
    alloc[2] = (uint64)mutexes;
    alloc[3] = start_index;
    alloc[4] = thread_section_size;
    alloc[5] = partition_count;
    alloc[6] = (uint64)write_indeces;
    alloc[7] = (uint64)partition_size;
    alloc[8] = (uint64)input_size;
    return alloc;
}
void *call_partition_concurrent_mutex(void *args)
{
    uint64 *input = *(uint64 **)args;
    uint64 *partitions = *(uint64 **)(args + 1 * 8);
    pthread_mutex_t *mutexes = *((pthread_mutex_t **)(args + 2 * 8));
    uint64 start_index = *(uint64 *)(args + 3 * 8);
    uint64 thread_section_size = *(uint64 *)(args + 4 * 8);
    uint64 partition_count = *(uint64 *)(args + 5 * 8);
    uint64 *write_indeces = *(uint64 **)(args + 6 * 8);
    uint64 partition_size = *(uint64 *)(args + 7 * 8);
    uint64 input_size = *(uint64 *)(args + 8 * 8);

    for (uint64 i = 0; i < thread_section_size / 2; i++)
    {
        if (start_index + i > input_size * 2)
            break;
        uint64 *curr_read = input + start_index + i * 2;
        uint64 hash = *((uint64 *)curr_read) % partition_count;
        pthread_mutex_t curr_mutex = mutexes[hash];
        pthread_mutex_lock(&curr_mutex);
        uint64 write_index = write_indeces[hash];
        uint64 partition_write_index = hash * partition_size + write_index;
        partitions[partition_write_index] = *curr_read;
        partitions[partition_write_index + 1] = *(curr_read + 1);
        write_indeces[hash] += 2;
        pthread_mutex_unlock(&curr_mutex);
    }
    free(args);
    return NULL;
}
struct partition_info partition_concurrent_output_mutex(uint64 *input, uint64 input_size, uint64 thread_count, uint64 partition_count)
{
    int extra_buffer = input_size;
    int partition_size = (input_size + extra_buffer) * 2 / partition_count; // times 2 because it is a tuple
    int thread_section_size = ((input_size + (thread_count - 1)) / thread_count) * 2;

    pthread_mutex_t *mutexes = malloc(partition_count * sizeof(pthread_mutex_t));

    uint64 *write_indeces = malloc(partition_count * sizeof(uint64));
    uint64 bytes_to_allocate = partition_size * partition_count * sizeof(uint64);
    uint64 *partitions = malloc(bytes_to_allocate);
    pthread_t threads[thread_count];
    for (int i = 0; i < partition_count; i++)
    {
        write_indeces[i] = 0;
        pthread_mutex_init(&mutexes[i], NULL);
    }
    for (int i = 0; i < thread_count; i++)
    {
        uint64 start_index = i * thread_section_size;
        void *args = create_args_mutex(input, partitions, mutexes, start_index, thread_section_size, partition_count, write_indeces, partition_size, input_size);
        pthread_create(&threads[i], NULL, call_partition_concurrent_mutex, args);
    }
    for (int i = 0; i < thread_count; i++)
    {
        pthread_join(threads[i], NULL);
    }
    free(mutexes);
    struct partition_info data = {partitions, write_indeces};
    return data;
}
