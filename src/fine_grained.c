#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include "multi_set.h"

/*
* Implementation of fine-grained synchronisation
*
*/

FineNode* head;

void init_set(){
    head = (FineNode*)malloc(sizeof(FineNode));

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
    //printf("(thread %i) waiting to aquire lock at head: %p\n", thread, (void*)head);
    pthread_mutex_lock(&(head->mutex));
    //printf("(thread %i) lock acquired at head: %p\n", thread, (void*)head);
    FineNode* pred = head;
    FineNode* curr = head->next;
    //printf("(thread %i) pred node: %p\n", thread, (void*)pred);
    //printf("(thread %i) curr node: %p\n", thread, (void*)curr);

    while(curr != NULL){
        //printf("(thread %i) waiting to aquire lock at node: %p\n", thread, (void*)curr);
        pthread_mutex_lock(&(curr->mutex));
        //printf("(thread %i) lock acquired at %i by thread %i\n", curr, thread);
        pthread_mutex_unlock(&(pred->mutex));
        //printf("(thread %i) lock released at node: %i\n", pred, thread);

        if (compare_elements(element, size, curr->element, curr->size)){
            //printf("(thread %i) element found: %i\n", *(int*)curr->element);
            pthread_mutex_unlock(&(curr->mutex));
            //printf("(thread %i) lock released at node: %p\n", thread, (void*)curr);
            return true;
        }

        pred = curr;
        curr = curr->next;
        //printf("(thread %i) next pred node: %p\n", thread, (void*)pred);
        //printf("(thread %i) next curr node: %p\n", thread, (void*)curr);
    }

    //printf("(thread %i) element not found: %i\n", thread, *(int*)element);
    pthread_mutex_unlock(&(pred->mutex));
    //printf("(thread %i) lock released at %i node: %p\n", thread, (void*)pred);

    return false;
}

int count_element(void* element, size_t size, int thread) {
    //printf("(thread %i) counting for element: %i\n", thread, *(int*)element);
    //printf("(thread %i) waiting to aquire lock at head: %p\n", thread, (void*)head);
    pthread_mutex_lock(&(head->mutex));
    //printf("(thread %i) lock acquired at head: %p\n", thread, (void*)head);
    FineNode* pred = head;
    FineNode* curr = head->next;
    //printf("(thread %i) pred node: %p\n", (void*)pred);
    //printf("(thread %i) curr node: %p\n", (void*)curr);

    while(curr != NULL){
        //printf("(thread %i) waiting to aquire lock at node: %p\n", thread, (void*)curr);
        pthread_mutex_lock(&(curr->mutex));
        //printf("(thread %i) lock acquired at node: %p\n", thread, (void*)curr);
        pthread_mutex_unlock(&(pred->mutex));
        //printf("(thread %i) lock released at node: %p\n", thread, (void*)pred);

        if (compare_elements(element, size, curr->element, curr->size)){
            //printf("(thread %i) element found: %i, count: %i\n", thread, *(int*)curr->element, curr->count);
            pthread_mutex_unlock(&(curr->mutex));
            //printf("(thread %i) lock released at node: %p\n", thread, (void*)curr);
            return curr->count;
        }

        pred = curr;
        curr = curr->next;
        //printf("(thread %i) next pred node: %p\n", thread, (void*)pred);
        //printf("(thread %i) next curr node: %p\n", thread, (void*)curr);
    }

    //printf("(thread %i) element not found: %i\n", thread, *(int*)element);
    pthread_mutex_unlock(&(pred->mutex));
    //printf("(thread %i) lock released at node: %p\n", thread, (void*)pred);

    return 0;
}

void add_element(void* element, size_t size, int thread) {
    //printf("(thread %i) adding element %i\n", thread, *(int*)element);
    //printf("(thread %i) waiting to aquire lock at head: %p\n", thread, (void*)head);
    pthread_mutex_lock(&(head->mutex));
    //printf("(thread %i) lock acquired at head: %p\n", thread, (void*)head);
    FineNode* pred = head;
    FineNode* curr = head->next;
    //printf("(thread %i) pred node: %p\n", thread, (void*)pred);
    //printf("(thread %i) curr node: %p\n", thread, (void*)curr);

    while(curr != NULL){
        //printf("(thread %i) waiting to aquire lock at node: %p\n", thread, (void*)curr);
        pthread_mutex_lock(&(curr->mutex));
        //printf("(thread %i) lock acquired at node: %p\n", thread, (void*)curr);

        if (compare_elements(element, size, curr->element, curr->size)){
            curr->count++;
            //printf("(thread %i) element found: %i, counter being incremented\n", thread, *(int*)curr->element);
            pthread_mutex_unlock(&(curr->mutex));
            //printf("(thread %i) lock released at node: %p\n", thread, (void*)curr);
            pthread_mutex_unlock(&(pred->mutex));
            //printf("(thread %i) lock released at node: %p\n", thread, (void*)pred);

            return;
        }

        pthread_mutex_unlock(&(pred->mutex));
        //printf("(thread %i) lock released at node: %p\n", thread, (void*)pred);
        pred = curr;
        curr = curr->next;
        //printf("(thread %i) next pred node: %p\n", thread, (void*)pred);
        //printf("(thread %i) next curr node: %p\n", thread, (void*)curr);
    }

    //printf("(thread %i) element not found: %i\n", thread, *(int*)element);
    FineNode* newNode = (FineNode*)malloc(sizeof(FineNode));
    //printf("(thread %i) new node created: %p\n", thread, (void*)newNode);

    if (newNode == NULL) {
        perror("Memory allocation failed");
        pthread_mutex_unlock(&(pred->mutex));
        exit(EXIT_FAILURE);
    }

    pthread_mutex_init(&(newNode->mutex), NULL);

    newNode->element = malloc(size);
    memcpy(newNode->element, element, size);
    newNode->size = size;
    newNode->count = 1;
    newNode->next = NULL;
    pred->next = newNode;
    //printf("(thread %i) new node added: %p\n", thread, (void*)newNode);
    pthread_mutex_unlock(&(pred->mutex));
    //printf("(thread %i) lock released at node: %p\n", thread, (void*)pred);
}

bool remove_element(void* element, size_t size, int thread) {
    //printf("(thread %i) removing element: %i\n", thread, *(int*)element);
    //printf("(thread %i) waiting to aquire lock at head: %i\n", thread, (void*)head);
    pthread_mutex_lock(&(head->mutex));
    //printf("(thread %i) lock acquired at head: %p\n", thread, (void*)head);
    FineNode* pred = head;
    FineNode* curr = head->next;
    //printf("(thread %i) pred node: %p\n", thread, (void*)pred);
    //printf("(thread %i) curr node: %p\n", thread, (void*)curr);
    
    while(curr != NULL) {
        //printf("(thread %i) waiting to aquire lock at node: %p\n", thread, (void*)curr);
        pthread_mutex_lock(&(curr->mutex));
        //printf("(thread %i) lock acquired at node: %p\n", thread, (void*)curr);

        if (compare_elements(element, size, curr->element, curr->size)) {
            curr->count--;
            //printf("(thread %i) node found: %i, counter being decremented\n", thread, *(int*)curr->element);

            if (curr->count == 0) {
                //printf("(thread %i) count 0 for: %i, node being removed\n", thread, *(int*)curr->element);
                pred->next = curr->next;
                free(curr);
            }
        
            pthread_mutex_unlock(&(curr->mutex));
            //printf("(thread %i) lock released at node: %p\n", thread, (void*)curr);
            pthread_mutex_unlock(&(pred->mutex));
            //printf("(thread %i) lock released at node: %p\n", thread, (void*)pred);

            return true;
        }

        pthread_mutex_unlock(&(pred->mutex));
        //printf("(thread %i) lock released at node: %p\n", thread, (void*)pred);
        pred = curr;
        curr = curr->next;
        //printf("(thread %i) next pred node: %p\n", thread, (void*)pred);
        //printf("(thread %i) next curr node: %p\n", thread, (void*)curr);
    }
    //printf("(thread %i) element not found: %i\n", thread, *(int*)element);
    pthread_mutex_unlock(&(pred->mutex));
    //printf("(thread %i) lock released at node: %p\n", thread, (void*)pred);

    return false;
}

bool compare_elements(const void* element, const size_t size, void* curr_element, size_t curr_size) {
    if (!(size == curr_size)) {
        return false;
    }

    return memcmp(curr_element, element, size) == 0;
}

void print_multiset() {
    FineNode* curr = head->next;
    
    printf("head -> " );

    while(curr != NULL) {
        printf("%i(%i) -> ", *(int*)curr->element, curr->count);
        curr = curr->next;
    }

    printf("\n");
}