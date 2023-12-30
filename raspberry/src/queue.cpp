#include "queue.h"

Item* cQueue::newItem(int key, int value)
{
    Item* item = malloc(sizeof(Item));
    item->key = key;
    item->value = value;
    return item;
}

Queue* cQueue::initQueue()
{
    Queue* queue = malloc(sizeof(Queue));
    return queue;
}

void cQueue::enqueue(Queue* queue, int key, int value)
{
    Item* item = newItem(key, value);
    if(queue->rear)
    {
        queue->rear->next = item;
        queue->rear = item;
    }
    else
        queue->front = queue->rear = item;
}

int cQueue::getByKey(Queue* queue, int key)
{
    Item* temp = queue->front;
    Item* prevItem = NULL;

    if (temp == NULL)
        return 0;

    while (temp != NULL && temp->key != key)
    {
        prevItem = temp;
        temp = temp->next;
    }

    prevItem->next = temp->next;

    if(queue->rear == temp)
        queue->rear = prevItem;

    free(temp);
    return 0;
}

void cQueue::dequeueAll(Queue* queue)
{
    while(queue->front)
    {
        Item* item = queue->front;
        queue->front = queue->front->next;
        free(item);
    }
    queue->rear = NULL;
}


int cQueue::queueNotEmpty(Queue* queue)
{
    if(queue->front)
        return 1;
    else
        return 0;
}

int cQueue::getCount(Queue* queue)
{
    int count = 0;
    Item* temp = queue->front;
    while (temp) 
    {
        count += 1;
        temp = temp->next;
    }
    return count;
}

int cQueue::searchKey(Queue* queue, int key)
{
    Item* temp = queue->front;
    while (temp) 
    {
        if (temp->key == key)
        {
            return temp->value;
        }
        temp = temp->next;
    }
    return -1;
}

int cQueue::updateKey(Queue* queue, int key, int value)
{
    Item* temp = queue->front;

    if (temp == NULL)
        return -1;

    while (temp != NULL)
    {
        if (temp->key == key)
        {
            temp->value = value;
            return 0;
        }
        temp = temp->next;
    }
    return -1;
}