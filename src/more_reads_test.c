#include <stdio.h> 
#include <stdatomic.h>
#include <stdbool.h>
#include <time.h>
#include "multi_set.h"
#include "test_program.h"

/*
* Multi-threaded test program which performs more reads than writes
*
*/

#define NUM_THREADS 20
#define OPERATIONS_PER_THREAD 1000
#define READ_PROB 7

void* more_reads_function(void* id) {                                                                                                                                                                                                       
    int thread_id = *((int*)id);

    srand(time(NULL));

    for (int i = 0; i < OPERATIONS_PER_THREAD; ++i) {
        int ran_element = rand() % OPERATIONS_PER_THREAD;
        int ran_operation = rand() % 10;
        int ran_choice = rand() % 2;

        if (ran_operation <= READ_PROB) {
            if (ran_choice) {
                contains_element(&ran_element, sizeof(ran_element), thread_id);
            } else {
                count_element(&ran_element, sizeof(ran_element), thread_id);
            }
        } else {
           if (ran_choice) {
                add_element(&ran_element, sizeof(ran_element), thread_id);
            } else {
                remove_element(&ran_element, sizeof(ran_element), thread_id);
            } 
        }
    }

    return NULL;
}

void run_more_reads_test() { 
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    int total_ops = NUM_THREADS * OPERATIONS_PER_THREAD;
    clock_t start_time, end_time;
    double duration;
    double throughput;

    start_time = clock();

    init_set();

    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_ids[i] = i;

        if (pthread_create(&threads[i], NULL, more_reads_function, (void*)&thread_ids[i])) {
            fprintf(stderr, "Error creating thread\n");
        }
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    end_time = clock();
    duration = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    throughput = total_ops / duration;

    printf("Benchmark result:\n");
    printf("Number of threads = %i\n", NUM_THREADS);
    printf("Number of operations per thread = %i\n", OPERATIONS_PER_THREAD);
    printf("Total number of operations = %i:\n", total_ops);
    printf("Time taken to complete execution = %f secs\n", duration);
    printf("Throughput = %f\n", throughput);
    //printf("final multiset state:\n");
    //print_multiset();
}