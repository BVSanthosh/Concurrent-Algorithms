#include <stdio.h> 
#include <stdlib.h>
#include "test_program.h"

/*
* Entry point where the user is prompted to choose the test to run
*
*/

int main() {
    int choice;

    printf("Choose the type of test to run:\n");
    printf("0: single-thread\n");
    printf("1: multi-threaded with more reads\n");
    printf("2: multi-threaded with more writes\n");
    printf("3: multi-threaded with equal number of reads and writes\n");
    printf("4: customised multi-threaded\n");

    scanf("%i", &choice);

    switch (choice) {
        case 0:
            run_single_thread_test();
            break;
        case 1:
            run_more_reads_test();
            break;
        case 2:
            run_more_writes_test();
            break; 
        case 3:
            run_equal_reads_writes_test();
            break;
        case 4:
            run_custom_test();
            break;
        default:
            printf("Invalid choice");
    }

    printf("Test completed.\n");
    return 0;
}