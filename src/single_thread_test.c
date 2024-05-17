#include <stdio.h> 
#include <stdlib.h>
#include <stdbool.h>
#include "multi_set.h"
#include "test_program.h"

/*
* Single-threaded test program which tests the core functionality of the multi-set
*
*/

#define OPERATION_CALLS 10

void run_single_thread_test() { 
    for (int i = 0; i < OPERATION_CALLS; ++i) {
        add_element(&i, sizeof(i), 0);
    }

    for (int i = 0; i < OPERATION_CALLS; ++i) {
        contains_element(&i, sizeof(i), 0);
    }

    for (int i = 0; i < OPERATION_CALLS; ++i) {
        count_element(&i, sizeof(i), 0);
    }

    for (int i = 0; i < OPERATION_CALLS; ++i) {
        remove_element(&i, sizeof(i), 0);
    }
}