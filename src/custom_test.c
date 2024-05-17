#include <stdio.h> 
#include <stdatomic.h>
#include <stdbool.h>
#include <time.h>
#include "multi_set.h"
#include "test_program.h"

/*
* Multi-threaded test program which prompts the user to choose the number of threads and the number of operations per thread
*
*/

int operation_calls;

void* custom_test_function(void* id) {                                                                                                                                                                                                       
    int thread_id = *((int*)id);

    srand(time(NULL));

    for (int i = 0; i < operation_calls; ++i) {
        int ran_element = rand() % operation_calls;
        int ran_operation = rand() % 4;

        switch (ran_operation) {
            case 0:
                add_element(&ran_element, sizeof(ran_element), thread_id);
                break;
            case 1:
                remove_element(&ran_element, sizeof(ran_element), thread_id);
                break;
            case 2:
                contains_element(&ran_element, sizeof(ran_element), thread_id);
                break;
            case 3:
                count_element(&ran_element, sizeof(ran_element), thread_id);
                break;
            default:
                break;
        }
    }

    return NULL;
}

void run_custom_test() { 
    int num_threads;
    clock_t start_time, end_time;
    double duration;
    double throughput;

    printf("Enter the number of threads: ");
    scanf("%d", &num_threads);
    printf("Enter the number of calls for each operation: ");
    scanf("%d", &operation_calls);

    pthread_t threads[num_threads];
    int thread_ids[num_threads];
    int total_ops = num_threads * operation_calls;

    start_time = clock();

    init_set();

    for (int i = 0; i < num_threads; ++i) {
        thread_ids[i] = i;

        if (pthread_create(&threads[i], NULL, custom_test_function, (void*)&thread_ids[i])) {
            fprintf(stderr, "Error creating thread\n");
        }
    }

    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    end_time = clock();
    duration = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    throughput = total_ops / duration;

    printf("Benchmark result:\n");
    printf("Number of threads = %i\n", num_threads);
    printf("Number of operations per thread = %i\n", operation_calls);
    printf("Total number of operations = %i:\n", total_ops);
    printf("Time taken to complete execution = %f\n", duration);
    printf("Throughput = %f\n", throughput);
    //printf("final multiset state:\n");
    //print_multiset();
}