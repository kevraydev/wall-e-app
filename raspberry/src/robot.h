#ifndef ROBOT_H
#define ROBOT_H

#include <wiringSerial.h>
#include <stdio.h>
#include <stdint.h>
#include <PCA9685.h>
#include <math.h>
#include <time.h>

int fd;
int addr = 0x40;
#define QUEUE_POLLING_RATE 1
time_t startTime, currentTime;
int serial_port;

//Control ID - Default POS - Current POS - Min POS - Max POS
int controls[9][5] = {
  {0, 90, 90, 10, 175},//Head
  {1, 90, 90, 25, 170},//Neck
  {2, 95, 95, 50, 95},//Left Eye
  {3, 95, 95, 65, 95},//Right Eye
  {4, 180, 180, 1, 180},//Left Track
  {5, 120, 130, 1, 180},//Left Arm
  {6, 1, 1, 1, 180},//Right Track
  {7, 55, 55, 1, 180},//Right Arm
  {8, 130, 130, 20, 170},//Bottom Neck
};

typedef struct Item {
    int key;
    int value;
    struct Item* next;
} Item;

typedef struct Queue {
    Item* front;
    Item* rear;
} Queue;

//typedef struct PD PD;
typedef struct PD {
    int pos;
    int prevError;
    float p;
	float d;
} PD;

void robot_init();

void set_PD();

void delay(int mili);

void sendData(int angle, int distance, int ctrlId);

void closeSerial();

void setServoAngle(int channel, float degree);

void initPOS();

void eyeCalibration();

void updatePD(PD *newPos, int error);

int initHardware(int adpt, int addr, int freq);

void updateHead(int pan, int tilt);

Item* newItem(int key, int value);

Queue* initQueue();

void enqueue(Queue* queue, int key, int value);//enqueue new item with key and value

int dequeue(Queue* queue, int key);//dequeue by key

void dequeueAll(Queue* queue);//dequeue all

int queueNotEmpty(Queue* queue);//returns 0 if queue empty or 1 if not empty

int getCount(Queue* queue);//return number of items in queue

int getByKey(Queue* queue, int key);//return value stored at queue by it's key

int updateKey(Queue* queue, int key, int value);//update value stored at queue by it's key

void iterateServos(Queue *queue);

#endif