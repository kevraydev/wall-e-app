#include <stdio.h>
#include <stdlib.h>

typedef struct AnimationQ {
    int key;
    int startDelay;
    int servoId[9];
    float servoSpeed;
    struct AnimationQ* next;
} AnimationQ;

typedef struct Queue {
    AnimationQ* front;
    AnimationQ* rear;
} Queue;

Queue* initQueue() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->front = NULL;
    queue->rear = NULL;
    return queue;
}

int getCount(Queue* queue) {
    int count = 0;
    AnimationQ* temp = queue->front;
    while (temp) {
        count += 1;
        temp = temp->next;
    }
    return count;
}

void enqueue(Queue* queue, int *servo, float speed, int delay) {
    int key = getCount(queue) + 1;
    AnimationQ* nServo = (AnimationQ*)malloc(sizeof(AnimationQ));
    nServo->key = key;
    for(int i = 0; i < 9; i++){
        nServo->servoId[i] = servo[i];
    }
    if(speed != 0)
        nServo->servoSpeed = speed;
    else
        nServo->servoSpeed = 0;
    if(delay != 0)
        nServo->startDelay = delay;
    else
        nServo->startDelay = 0;
    nServo->next = NULL; // Initialize next to NULL
    
    if (queue->rear) {
        queue->rear->next = nServo;
        queue->rear = nServo;
    } else {
        queue->front = queue->rear = nServo;
    }
}

void dequeue(Queue* queue, int key) {
    AnimationQ* temp = queue->front;
    AnimationQ* prevServo = NULL;

    while (temp != NULL && temp->key != key) {
        prevServo = temp;
        temp = temp->next;
    }

    if (prevServo) {
        prevServo->next = temp->next;
    } else {
        queue->front = temp->next;
    }

    if (queue->rear == temp) {
        queue->rear = prevServo;
    }

    free(temp);
}

void dequeueAll(Queue* queue) {
    while (queue->front) {
        AnimationQ* cServo = queue->front;
        queue->front = queue->front->next;
        free(cServo);
    }
    queue->rear = NULL;
}

int queueNotEmpty(Queue* queue) {
    return queue->front != NULL;
}
