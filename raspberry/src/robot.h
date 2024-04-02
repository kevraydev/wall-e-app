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
#define POLLING_RATE 5
#define CAM_WIDTH 480
#define CAM_HEIGHT 640
#define CAM_WIDTH_F 240
#define CAM_HEIGHT_F 320
#define LOW_SPEED 60
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

typedef struct {
    int left;
    int right;
    int state;
} Speed;

typedef struct objectCoord {
    int state;
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
Speed track;

Servo posX;
Servo posY;
Servo tilt;


void robot_init();

void delay(int mili);

void sendData(int value1, int value2);

void closeSerial();

uint16_t pulseWidth(int channel, float degree);

void setServoAngle();

void initPOS();

void eyeCalibration();

int initHardware(int adpt, int addr, int freq);

void updateHead(int x, int y);

void updateCoords(objectCoord* obj);

void updateTrackCoord(int x, int y, Speed *obj);

void setTrackSpeed(Speed *obj);

void resetState(objectCoord *obj);

void stopTracks();

Point convert_angle(int angle);


#endif