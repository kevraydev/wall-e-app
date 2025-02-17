#ifndef QUEUE_H
#define QUEUE_H


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

AnimationQ* newServo(int key, int *servo);
Queue* initQueue();
int getCount(Queue* queue);
void enqueue(Queue* queue, int *servo, float speed, int delay);
int dequeue(Queue* queue, int key);
void dequeueAll(Queue* queue);
int queueNotEmpty(Queue* queue);

#endif // QUEUE_H
