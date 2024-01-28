#ifndef ROBOT_H
#define ROBOT_H

#include <wiringSerial.h>
#include <stdint.h>
#include <stdlib.h>
#include <PCA9685.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

int fd;
extern int addr;
#define POLLING_RATE 50
#define CAM_WIDTH 640 / 2
#define CAM_HEIGHT 480 / 2
//#define MEDIAN(value1, value2)
struct timespec startTime, currentTime;
int serial_port;


extern int controls[9][3];
/*
typedef struct Item {
    int key;
    int value;
    //int prevError;
    struct Item* next;
} Item;

typedef struct Queue {
    Item* front;
    Item* rear;
} Queue;
*/
//typedef struct PD PD;

typedef struct objectCoord {
    int area;
    int x;
    int y;
} objectCoord;

typedef struct Servo {
    int prevError;
    int pos;
    int min;
    int max;
    float p;
	float d;
} Servo;

struct Servo servos[6];

objectCoord obj;

Servo posX;
Servo posY;
Servo tilt;


void robot_init();

void delay(int mili);

void sendData(int angle, int distance, int ctrlId);

void closeSerial();

void setServoAngle(int channel, float degree);

void initPOS();

void eyeCalibration();

int initHardware(int adpt, int addr, int freq);

void updateHead(int pan, int tilt);

void updateCoords(objectCoord* obj);
//Item* newItem(int key, int value);

//Queue* initQueue();

//void enqueue(Queue* queue, int key, int value);//enqueue new item with key and value

//int dequeue(Queue* queue, int key);//dequeue by key

//void dequeueAll(Queue* queue);//dequeue all

//int queueNotEmpty(Queue* queue);//returns 0 if queue empty or 1 if not empty

//int getCount(Queue* queue);//return number of items in queue

//int getByKey(Queue* queue, int key);//return value stored at queue by it's key

//int updateKey(Queue* queue, int key, int value);//update value stored at queue by it's key

//void updateControl(Item *current);

//void iterateServos(Queue *queue);

#endif