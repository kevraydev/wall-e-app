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
#define POLLING_RATE 150
#define CAM_WIDTH 480
#define CAM_HEIGHT 640
#define mapRange(a1,a2,b1,b2,s) (b1 + (s-a1)*(b2-b1)/(a2-a1))

struct timespec startTime, currentTime;
int serial_port;


extern int controls[9][3];

//typedef struct PD PD;

typedef struct {
    double x;
    double y;
} Point;

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

Point convert_angle(int angle);


#endif