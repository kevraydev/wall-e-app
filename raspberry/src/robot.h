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
#define POLLING_RATE 1
#define CAM_WIDTH 480
#define CAM_HEIGHT 640
#define mapRange(a1,a2,b1,b2,s) (b1 + (s-a1)*(b2-b1)/(a2-a1))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define MIN(a,b) ((a) < (b)  ? (a) : (b) )
#define MAX(a,b) ((a) > (b)  ? (a) : (b) )

struct timespec startTime, currentTime;
int serial_port;

extern unsigned int setServo[_PCA9685_CHANS];
extern unsigned int offVals[_PCA9685_CHANS];

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

uint16_t pulseWidth(int channel, float degree);

void setServoAngle();

void initPOS();

void eyeCalibration();

int initHardware(int adpt, int addr, int freq);

void updateHead(int x, int y, int area);

void updateCoords(objectCoord* obj);

Point convert_angle(int angle);


#endif