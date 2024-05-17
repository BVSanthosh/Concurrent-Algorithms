#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include "multi_set.h"

/*
* Implementation of optimistic synchronisation
*
*/

OptimisticNode* head;

void init_set(){
    head = (OptimisticNode*)malloc(sizeof(OptimisticNode));

    if (head == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    head->element = NULL;
    head->count = 0;
    head->next = NULL;

    pthread_mutex_init(&(head->mutex), NULL);
    //printf("=== multiset initialised ===\n");
    //printf("head element: %i\n", head->element);
    //printf("head count: %i\n", head->count);
    //printf("head next pointer: %p\n", head->next);
}

bool contains_element(void* element, size_t size, int thread) { 
    //printf("(thread %i) searching for element: %i\n", thread, *(int*)element);
    OptimisticNode* pred = head;
    OptimisticNode* curr = head->next;
    //printf("(thread %i) pred node: %p\n", thread, (void*)pred);
    //printf("(thread %i) curr node: %p\n", thread, (void*)curr);

    while(curr != NULL){
        if (compare_elements(element, size, curr->element, curr->size)){
            //printf("(thread %i) element found: %i\n", thread, *(int*)element);
            //printf("(thread %i) waiting to acquire lock at node: %p\n", thread, (void*)pred);
            pthread_mutex_lock(&(pred->mutex)); 
            //printf("(thread %i) lock acquired at node: %p\n", thread, (void*)pred);
            //printf("(thread %i) waiting to acquire lock at node: %p\n", thread, (void*)curr);
            pthread_mutex_lock(&(curr->mutex));
            //printf("(thread %i) lock acquired at node: %p\n", thread, (void*)curr);
            //printf("(thread %i) validating node: %p\n", thread, (void*)curr);

            if (validate(pred, curr)) {
                //printf("(thread %i) validation pass\n", thread);
                pthread_mutex_unlock(&(curr->mutex)); 
                //printf("(thread %i) lock released at node: %p\n", thread, (void*)curr);
                pthread_mutex_unlock(&(pred->mutex));
                //printf("(thread %i) lock released at node: %p\n", thread, (void*)pred);

                return true;
            } else {
                //printf("(thread %i) validation fail\n");
                pthread_mutex_unlock(&(curr->mutex)); 
                //printf("(thread %i) lock released at node: %p\n", thread, (void*)curr);
                pthread_mutex_unlock(&(pred->mutex));
                //printf("(thread %i) lock released at node: %p\n", thread, (void*)pred);

                pred = head;
                curr = head->next;
                //printf("(thread %i) pred node: %p\n", thread, (void*)pred);
                //printf("(thread %i) curr node: %p\n", thread, (void*)curr);

                continue;
            }
        }

        pred = curr;
        curr = curr->next;
        //printf("(thread %i) next pred node: %p\n", thread, (void*)pred);
        //printf("(thread %i) next curr node: %p\n", thread, (void*)curr);
    }

    //printf("(thread %i) element not found %i\n", thread, *(int*)element);

    return false;
}

int count_element(void* element, size_t size, int thread) {
    //printf("(thread %i) counting for element: %i\n", thread, *(int*)element);
    OptimisticNode* pred = head;
    OptimisticNode* curr = head->next;
    //printf("(thread %i) pred node: %p\n", thread, (void*)pred);
    //printf("(thread %i) curr node: %p\n", thread, (void*)curr);

    while(curr != NULL){
        if (compare_elements(element, size, curr->element, curr->size)){
            //printf("(thread %i) element found: %i, count: %i\n", thread, *(int*)curr->element, curr->count);
            //printf("(thread %i) waiting to acquire lock at node: %p\n", thread, (void*)pred);
            pthread_mutex_lock(&(pred->mutex)); 
            //printf("(thread %i) lock acquired at node: %p\n", thread, (void*)pred);
            //printf("(thread %i) waiting to acquire lock at node: %p\n", thread, (void*)curr);
            pthread_mutex_lock(&(curr->mutex));
            //printf("(thread %i) lock acquired at node: %p\n", thread, (void*)curr);
            //printf("(thread %i) validating node: %p\n", thread, (void*)curr);

            if (validate(pred, curr)) {
                //printf("(thread %i) validation pass\n", thread);
                pthread_mutex_unlock(&(curr->mutex)); 
                //printf("(thread %i) lock released at node: %p\n", thread, (void*)curr);
                pthread_mutex_unlock(&(pred->mutex));
                //printf("(thread %i) lock released at node: %p\n", thread, (void*)pred);

                return curr->count;
            } else {
                //printf("(thread %i) validation fail\n");
                pthread_mutex_unlock(&(curr->mutex)); 
                //printf("(thread %i) lock released at node: %p\n", thread, (void*)curr);
                pthread_mutex_unlock(&(pred->mutex));
                //printf("(thread %i) lock released at node: %p\n", thread, (void*)pred);

                pred = head;
                curr = head->next;
                //printf("(thread %i) pred node: %p\n", thread, (void*)pred);
                //printf("(thread %i) curr node: %p\n", thread, (void*)curr);
                
                continue;
            }
        }

        pred = curr;
        curr = curr->next;
        //printf("(thread %i) next pred node: %p\n", thread, (void*)pred);
        //printf("(thread %i) next curr node: %p\n", thread, (void*)curr);
    }

    //printf("(thread %i) element not found: %i\n", thread, *(int*)element);

    return 0;
}

void add_element(void* element, size_t size, int thread) {
    //printf("(thread %i) adding element: %i\n", thread, *(int*)element);
    OptimisticNode* pred = head;
    OptimisticNode* curr = head->next;
    //printf("(thread %i) pred node: %p\n", thread, (void*)pred);
    //printf("(thread %i) curr node: %p\n", thread, (void*)curr);

    while(true){
        if (curr == NULL) {
            //printf("(thread %i) element not found: %i\n", thread, *(int*)element);
            OptimisticNode* newNode = (OptimisticNode*)malloc(sizeof(OptimisticNode));
            //printf("(thread %i) new node created: %p\n", thread, (void*)newNode);

            if (newNode == NULL) {
                perror("Memory allocation failed");
                exit(EXIT_FAILURE);
            }

            pthread_mutex_init(&(newNode->mutex), NULL);

            newNode->element = malloc(size);
            memcpy(newNode->element, element, size);
            newNode->size = size;
            newNode->count = 1;
            newNode->next = NULL;
            //printf("(thread %i) waiting to acquire lock at node: %p\n", thread, (void*)pred);
            pthread_mutex_lock(&(pred->mutex)); 
            //printf("(thread %i) lock acquired at node: %p\n", thread, (void*)pred);
            //printf("(thread %i) validating node: %p\n", thread, (void*)pred);

            if (validate_tail(pred)) {
                //printf("(thread %i) validation pass\n", thread);
                pred->next = newNode;
                //printf("(thread %i) added new node: %p\n", thread, (void*)newNode);
                pthread_mutex_unlock(&(pred->mutex));
                //printf("(thread %i) lock released at node: %p\n", thread, (void*)pred);

                return;
            } else {
                //printf("(thread %i) validation fail\n", thread);
                pthread_mutex_unlock(&(pred->mutex));
                //printf("(thread %i) lock released at node: %p\n", thread, (void*)pred);

                pred = head;
                curr = head->next;
                //printf("(thread %i) pred node %p\n", thread, (void*)pred);
                //printf("(thread %i) curr node %p\n", thread, (void*)curr);

                free(newNode);

                continue;
            }
        } else if (compare_elements(element, size, curr->element, curr->size)){
            //printf("(thread %i) waiting to acquire lock at node: %p\n", thread, (void*)pred);
            pthread_mutex_lock(&(pred->mutex)); 
            //printf("(thread %i) lock acquired at node: %p\n", thread, (void*)pred);
            //printf("(thread %i) waiting to acquire lock at node: %p\n", thread, (void*)curr);
            pthread_mutex_lock(&(curr->mutex));
            //printf("(thread %i) lock acquired at node: %p\n", thread, (void*)curr);
            //printf("(thread %i) validating node: %p\n", thread, (void*)curr);

            if (validate(pred, curr)) {
                //printf("(thread %i) validation pass\n", thread);
                curr->count++;
                //printf("(thread %i) element found: %i, counter being incremented\n", thread, *(int*)curr->element);
                pthread_mutex_unlock(&(curr->mutex)); 
                //printf("(thread %i) lock released at node: %p\n", thread, (void*)curr);
                pthread_mutex_unlock(&(pred->mutex));
                //printf("(thread %i) lock released at node: %p\n", thread, (void*)pred);

                return;
            } else {
                //printf("(thread %i) validation fail\n");
                pthread_mutex_unlock(&(curr->mutex)); 
                //printf("(thread %i) lock released at node: %p\n", thread, (void*)curr);
                pthread_mutex_unlock(&(pred->mutex));
                //printf("(thread %i) lock released at node: %p\n", thread, (void*)pred);

                pred = head;
                curr = head->next;
                //printf("(thread %i) pred node: %p\n", thread, (void*)pred);
                //printf("(thread %i) curr node: %p\n", thread, (void*)curr);

                continue;
            }
        }

        pred = curr;
        curr = curr->next;
        //printf("(thread %i) next pred node: %p\n", thread, (void*)pred);
        //printf("(thread %i) next curr node: %p\n", thread, (void*)curr);
    }
}

bool remove_element(void* element, size_t size, int thread) {
    //printf("(thread %i) removing element: %i\n", thread, *(int*)element);
    OptimisticNode* pred = head;
    OptimisticNode* curr = head->next;
    //printf("(thread %i) pred node: %p\n", thread, (void*)pred);
    //printf("(thread %i) curr node: %p\n", thread, (void*)curr);

    while(curr != NULL) {
        if (compare_elements(element, size, curr->element, curr->size)) {
            //printf("(thread %i) waiting to acquire lock at node: %p\n", thread, (void*)pred);
            pthread_mutex_lock(&(pred->mutex));
            //printf("(thread %i) lock acquired at node: %p\n", thread, (void*)pred);
            //printf("(thread %i) waiting to acquire lock at node: %p\n", thread, (void*)curr);
            pthread_mutex_lock(&(curr->mutex));
            //printf("(thread %i) lock acquired at node: %p\n", thread, (void*)curr);
            //printf("(thread %i) validating node: %p\n", thread, (void*)curr);

            if (validate(pred, curr)) {
                //printf("(thread %i) validation pass\n", thread);
                curr->count--;
                //printf("(thread %i) element found: %i, count being decremented\n", thread, *(int*)curr->element);

                if (curr->count == 0) {
                    //printf("(thread %i) count 0 for: %i, node being removed\n", thread, *(int*)curr->element);
                    pred->next = curr->next;

                    pthread_mutex_unlock(&(curr->mutex)); 
                    //printf("(thread %i) lock released at node: %p\n", thread, (void*)curr);
                    pthread_mutex_unlock(&(pred->mutex));
                    //printf("(thread %i) lock released at node: %p\n", thread, (void*)pred);
                    pthread_mutex_destroy(&(curr->mutex));
                    free(curr);
                } else {
                    pthread_mutex_unlock(&(curr->mutex)); 
                    //printf("(thread %i) lock released at node: %p\n", thread, (void*)curr);
                    pthread_mutex_unlock(&(pred->mutex));
                    //printf("(thread %i) lock released at node: %p\n", thread, (void*)pred);
                }
            } else {
                //printf("(thread %i) validation fail\n", thread);
                pthread_mutex_unlock(&(curr->mutex)); 
                //printf("(thread %i) lock released at node: %p\n", thread, (void*)curr);
                pthread_mutex_unlock(&(pred->mutex));
                //printf("(thread %i) lock released at node: %p\n", thread, (void*)pred);

                pred = head;
                curr = head->next;
                //printf("(thread %i) pred node: %p\n", thread, (void*)pred);
                //printf("(thread %i) curr node: %p\n", thread, (void*)curr);

                continue;
            }

            return true;
        }

        pred = curr;
        curr = curr->next;
        //printf("(thread %i) next pred node: %p\n", thread, (void*)pred);
        //printf("(thread %i) next curr node: %p\n", thread, (void*)curr);
    }
    
    //printf("(thread %i) element not found: %i\n", thread, *(int*)element);

    return false;
}

bool compare_elements(const void* element, const size_t size, void* curr_element, size_t curr_size) {
    if (!(size == curr_size)) {
        return false;
    }

    return memcmp(curr_element, element, size) == 0;
}

bool validate(OptimisticNode* pred, OptimisticNode* curr) {
    OptimisticNode* node = head;

    while (node != NULL) {
        if (node == pred && node->next == curr) {
            return true;
        }

        node = node->next;
    }

    return false;
}

bool validate_tail(OptimisticNode* pred) {
    OptimisticNode* node = head;

    while (node != NULL) {
        if (node == pred && pred->next == NULL) {
            return true;
        }

        node = node->next;
    }

    return false;
}

void print_multiset() {
    OptimisticNode* curr = head->next;

    printf("head -> " );

    while(curr != NULL) {
        printf("%i(%i) -> ", *(int*)curr->element, curr->count);
        curr = curr->next;
    }

    printf("\n");
}