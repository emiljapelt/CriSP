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

void *create_args(uint64 *input, uint64 **partitions, pthread_mutex_t *mutexes, uint64 start_index, uint64 thread_section_size, uint64 partition_count, uint64 *write_indeces)
{
    uint64 *alloc = malloc(7 * 8);
    alloc[0] = (uint64)input;
    alloc[1] = (uint64)partitions;
    alloc[2] = (uint64)mutexes;
    alloc[3] = start_index;
    alloc[4] = thread_section_size;
    alloc[5] = partition_count;
    alloc[6] = (uint64)write_indeces;
    return alloc;
}

void *call_partition_concurrent(void *data)
{
    void *args = (void *)data;

    uint64 *input = *(uint64 **)args;
    uint64 **partitions = *(uint64 ***)(args + 1);
    pthread_mutex_t *mutexes = (pthread_mutex_t *)(args + 2);
    uint64 start_index = *(uint64 *)(args + 3);
    uint64 thread_section_size = *(uint64 *)(args + 4);
    uint64 partition_count = *(uint64 *)(args + 5);
    uint64 *write_indeces = (uint64 *)(args + 6);

    for (uint64 i = 0; i < thread_section_size; i++)
    {
        uint64 hash = input[start_index + (2 * i)] % partition_count;
        pthread_mutex_lock(&(mutexes[hash]));
        uint64 write_index = write_indeces[hash];
        partitions[hash][write_index] = input[start_index + (2 * i)];
        partitions[hash][write_index + 1] = input[start_index + (2 * i) + 1];
        write_indeces[hash] += 2;
        pthread_mutex_unlock(&mutexes[hash]);
    }
    free(data);

    return NULL;
}

void partition_concurrent_output(int b, uint64 *input, uint64 input_size, uint64 thread_count)
{
    int partition_count = pow(2, b);
    int extra_buffer = input_size * 1;
    int partition_size = (input_size + extra_buffer) / partition_count;
    int thread_section_size = (partition_size + (thread_count - 1)) / thread_count;

    pthread_mutex_t mutexes[partition_count];

    uint64 write_indeces[partition_count];
    uint64 *partitions[partition_count];
    pthread_t threads[thread_count];

    for (int i = 0; i < partition_count; i++)
    {
        write_indeces[i] = 0;
        pthread_mutex_init(&mutexes[i], NULL);
        partitions[i] = malloc(partition_size);
    }

    for (int i = 0; i < thread_count; i++)
    {
        uint64 start_index = i * thread_section_size;
        void *args = create_args(input, partitions, mutexes, start_index, thread_section_size, partition_count, write_indeces);
        pthread_create(&threads[i], NULL, call_partition_concurrent, args);
    }

    for (int i = 0; i < thread_count; i++)
    {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < partition_count; i++)
    {
        uint64 *curr = partitions[i];
        while (curr != NULL)
        {
            printf("    %llu -> %lld\n", curr[0], curr[1]);
            curr = curr + 2;
        }
    }
}

int main()
{
    uint64 input[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    partition_concurrent_output(2, input, 10, 2);
    return 0;
}
