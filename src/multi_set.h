#include <stdbool.h>
#include <pthread.h>
#include <stdatomic.h>

typedef struct CoarseNode{   //linked-list for coarse-grained synchronisation
    void* element;
    size_t size;
    int count;
    struct CoarseNode* next;
} CoarseNode;

typedef struct FineNode{   //linked-list for fine-grained synchronisation
  void* element;
  size_t size;
  int count;
  pthread_mutex_t mutex;
  struct FineNode* next;
} FineNode;

typedef struct OptimisticNode{   //linked-list for optimistic synchronisation
  void* element;
  size_t size;
  int count;
  pthread_mutex_t mutex;
  struct OptimisticNode* next;
} OptimisticNode;

typedef struct LazyNode{   //linked-list for lazy synchronisation
  void* element;
  size_t size;
  int count;
  bool marked;
  pthread_mutex_t mutex;
  struct LazyNode* next;
} LazyNode;

void init_set();   //initialises the linekd-list with a head node

bool contains_element(void* element, size_t size, int thread);   //checks whether an element is in the multi-set

int count_element(void* element, size_t size, int thread);   //counts the number of occurrence of an element

void add_element(void* element, size_t size, int thread);   //adds an element to the multi-set 

bool remove_element(void* element, size_t size, int thread);   //removes an elemenet from the multi-set

bool compare_elements(const void* element, const size_t size, void* curr_element, size_t curr_size);   //compares the current elemenet with the one being searched

bool validate(OptimisticNode* pred, OptimisticNode* curr);   //checks that the current node is still reachable

bool validate_tail(OptimisticNode* pred);   //checks that the last node in the list is still reachable 

void print_multiset();   //helper function to print the contents of the multi-set