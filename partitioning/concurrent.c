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

void *create_args(uint64 *input, uint64 *partitions, pthread_mutex_t *mutexes, uint64 start_index, uint64 thread_section_size, uint64 partition_count, uint64 *write_indeces, uint64 partition_size)
{
    uint64 *alloc = malloc(8 * 8);
    alloc[0] = (uint64)input;
    alloc[1] = (uint64)partitions;
    alloc[2] = (uint64)mutexes;
    alloc[3] = start_index;
    alloc[4] = thread_section_size;
    alloc[5] = partition_count;
    alloc[6] = (uint64)write_indeces;
    alloc[7] = (uint64)partition_size;
    return alloc;
}

void *call_partition_concurrent(void *args)
{
    uint64 *input = *(uint64 **)args;
    uint64 *partitions = *(uint64 **)(args + 1 * 8);
    pthread_mutex_t *mutexes = *((pthread_mutex_t **)(args + 2 * 8));
    uint64 start_index = *(uint64 *)(args + 3 * 8);
    uint64 thread_section_size = *(uint64 *)(args + 4 * 8);
    uint64 partition_count = *(uint64 *)(args + 5 * 8);
    uint64 *write_indeces = *(uint64 **)(args + 6 * 8);
    uint64 partition_size = *(uint64 *)(args + 7 * 8);
    printf("%p %p %p %lld %lld %lld %p %lld\n", input, partitions, mutexes, start_index, thread_section_size, partition_count, write_indeces, partition_size);

    for (uint64 i = 0; i < thread_section_size; i++)
    {
        uint64 hash = (start_index + i) % partition_count;
        pthread_mutex_t curr_mutex = mutexes[hash];
        pthread_mutex_lock(&curr_mutex);
        uint64 write_index = write_indeces[hash];
        uint64 *currRead = input + start_index + i;
        uint64 partition_write_index = hash * partition_size + write_index;
        partitions[partition_write_index] = *currRead;
        // partitions[partition_write_index] = 11;
        // partitions[0] = 11;
        // uint64 *next = currWrite + 1;

        // *curr = input[start_index + (2 * i)];
        // *currWrite = *currRead;
        // *next = input[start_index + (2 * i) + 1];
        // *next = input + start_index + i + 1;
        // write_indeces[hash] += 2;
        write_indeces[hash] += 1;
        pthread_mutex_unlock(&curr_mutex);
    }
    free(args);

    return NULL;
}

void partition_concurrent_output(int b, uint64 *input, uint64 input_size, uint64 thread_count)
{
    int partition_count = pow(2, b);
    int extra_buffer = input_size * 3;
    int partition_size = (input_size + extra_buffer) / partition_count;
    int thread_section_size = (partition_size + (thread_count - 1)) / thread_count;

    pthread_mutex_t *mutexes = malloc(partition_count * sizeof(pthread_mutex_t));

    uint64 *write_indeces = malloc(partition_count);
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
        void *args = create_args(input, partitions, mutexes, start_index, thread_section_size, partition_count, write_indeces, partition_size);
        pthread_create(&threads[i], NULL, call_partition_concurrent, args);
    }

    for (int i = 0; i < thread_count; i++)
    {
        pthread_join(threads[i], NULL);
    }
    free(mutexes);
    free(write_indeces);

    // for (int i = 0; i < partition_count; i++)
    // {
    //     uint64 *curr = partitions[i];
    //     for (int j = 0; j < partition_size; j += 2)
    //     {
    //         printf("    %llu -> %lld\n", curr[0], curr[1]);
    //         curr = curr + 2;
    //     }
    // }
}

int main()
{
    uint64 input[5] = {12, 1, 2, 3, 4};
    partition_concurrent_output(2, input, 5, 2);
    return 0;
}
