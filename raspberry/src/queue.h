 #include <stdlib.h>
 
 
typedef struct Item {
    int key;
    int value;
    struct Item* next;
} Item;

typedef struct Queue {
    Item* front;
    Item* rear;
} Queue;

class cQueue {
  public:
Item* newItem(int key, int value);

Queue* initQueue();

void push(Queue* queue, int key, int value);

int popKey(Queue* queue, int key);

void popAll(Queue* queue);

int queueNotEmpty(Queue* queue);

int getCount(Queue* queue);

int searchKey(Queue* queue, int key);

int updateKey(Queue* queue, int key, int value);

};
