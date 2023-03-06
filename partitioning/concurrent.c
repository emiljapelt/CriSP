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

struct partition_data
{
    int partition_size;
    int partition_count;
    int thread_section_size;
    uint64 *partitions;
};

void *create_args(uint64 *input, uint64 *partitions, pthread_mutex_t *mutexes, uint64 start_index, uint64 thread_section_size, uint64 partition_count, uint64 *write_indeces, uint64 partition_size, uint64 input_size)
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
    uint64 input_size = *(uint64 *)(args + 8 * 8);

    for (uint64 i = 0; i < thread_section_size; i++)
    {
        if (start_index + i > input_size)
            break;
        uint64 *curr_read = input + start_index + i;
        uint64 hash = *(uint64 *)curr_read % partition_count;
        pthread_mutex_t curr_mutex = mutexes[hash];
        pthread_mutex_lock(&curr_mutex);
        uint64 write_index = write_indeces[hash];
        uint64 partition_write_index = hash * partition_size + write_index;
        partitions[partition_write_index] = *curr_read;
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

struct partition_data partition_concurrent_output(int b, uint64 *input, uint64 input_size, uint64 thread_count)
{
    int partition_count = pow(2, b);
    int extra_buffer = input_size;
    int partition_size = (input_size + extra_buffer) / partition_count;
    int thread_section_size = (input_size + (thread_count - 1)) / thread_count;

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
        void *args = create_args(input, partitions, mutexes, start_index, thread_section_size, partition_count, write_indeces, partition_size, input_size);
        pthread_create(&threads[i], NULL, call_partition_concurrent, args);
    }

    for (int i = 0; i < thread_count; i++)
    {
        pthread_join(threads[i], NULL);
    }
    free(mutexes);
    free(write_indeces);
    struct partition_data data = {partition_size, partition_count, thread_section_size, partitions};
    return data;
}

void print_result(struct partition_data result)
{
    for (int i = 0; i < result.partition_count; i++)
    {
        printf("partition %i\n", i);
        for (int j = 0; j < result.partition_size; j++)
        {
            printf("%lli ", result.partitions[result.partition_size * i + j]);
        }
        printf("\n");
    }
}

// args: problem_size b threads_count
int main(int argc, char **argv)
{
    int problem_size = atoi(argv[1]);
    int b = atoi(argv[2]);
    int thread_count = atoi(argv[3]);

    // Generate data
    uint64 *data = malloc((sizeof(uint64)) * problem_size);
    char buffer[8];
    for (int i = 0; i < problem_size; i++)
    {
        getrandom(&buffer, 8, 0);
        data[i] = *(uint64 *)&buffer;
    }

    struct timespec start, finish;
    clock_gettime(CLOCK_MONOTONIC, &start);
    struct partition_data result = partition_concurrent_output(b, data, problem_size, thread_count);
    clock_gettime(CLOCK_MONOTONIC, &finish);
    int elapsed = finish.tv_nsec - start.tv_nsec;

    print_result(result);

    printf("Completed in %i ns\n", elapsed);
    return 0;
}