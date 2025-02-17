#ifndef ROBOT_H
#define ROBOT_H

#include <wiringSerial.h>
#include <stdint.h>
#include <stdlib.h>
#include <PCA9685.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include "queue.h"


int fd;
int state = 0;
int animate = 0;
extern int addr;
#define POLL_RATE 250
#define WAIT_T 15000
#define CAM_WIDTH 640
#define CAM_HEIGHT 480
#define CAM_WIDTH_F 320
#define CAM_HEIGHT_F 240
#define LOW_SPEED 60
#define STEP_COUNT 10

#define TOP_HEAD 0
#define UPPER_NECK 1
#define LEFT_EYE 2
#define RIGHT_EYE 3
#define LEFT_TRACK 4
#define LEFT_ARM 5
#define RIGHT_TRACK 6
#define RIGHT_ARM 7
#define BOTTOM_NECK 8

#define A1 139
#define A2 63
#define EASING 0.20
#define mapRange(a1,a2,b1,b2,s) (b1 + (s-a1)*(b2-b1)/(a2-a1))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define rows(a) (sizeof(a)) / (sizeof(a[0]))
#define MIN(a,b) ((a) < (b)  ? (a) : (b) )
#define MAX(a,b) ((a) > (b)  ? (a) : (b) )
#define s(a) ((a) * (a))
#define RADTODEG(a) ((a)*(180.0)/(M_PI))
#define easeIn(x) ((1) - cos((x * M_PI) / (2)))
#define sl(a,b,c) ((1 - c) * (a) + (c) * (b))


//struct timespec startTime, currentTime;
int serial_port;

extern unsigned int setServo[_PCA9685_CHANS];
extern unsigned int offVals[_PCA9685_CHANS];

extern int controls[9][3];

typedef struct {
    struct timespec startTime;
    struct timespec currentTime;
} Timer;

typedef struct {
    double x;
    double y;
} Point;

typedef struct {
    int left;
    int right;
    int state;
} Speed;

/*typedef struct State {
    int trackingState;
    int joystickState;
} State;*/

typedef struct Servo {
    float targetPos;
    float pos;
    float easing;
    int timer;
    int min;
    int max;
} Servo;

//State obj;
Speed track;

Servo s[9];

typedef struct {
    uint8_t* addr;
    int width;
    int height;
} image;

typedef struct
{
    unsigned short H;
    unsigned char S;
    unsigned char V;
    unsigned char diff;
} color;

typedef struct {
    int b_Size;
    int b_Step;
    int num_Blocks;
    int step;
    int x;
    int y;
    int w;
    int h;
    color* p;
} roi;

roi rect;
image r_image;
Point bGrid;

#define MIN(a,b) ((a) < (b)  ? (a) : (b) )
#define MAX(a,b) ((a) > (b)  ? (a) : (b) )
#define CMIN(a,b,c) MIN(MIN(a,b),(c))
#define CMAX(a,b,c) MAX(MAX(a,b),(c))

#define RGB(r, g, b) ((uint8_t[]){(r),(g),(b)})

#define GET_R(pixel) (pixel[0])
#define GET_G(pixel) (pixel[1])
#define GET_B(pixel) (pixel[2])

#define PIXEL_ROW(image, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (y) _y = (y); \
    ((uint16_t *) _image->addr) + (_image->width * _y); \
})

#define GET_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    &((uint8_t *) _image->addr)[(_image->width * _y * 3) + (_x * 3)]; \
})

#define GET_U16_PIXEL(image, x, y) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    ((uint16_t *) _image->addr)[(_image->width * _y) + _x]; \
})

#define PUT_U16_PIXEL(image, x, y, v) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    __typeof__ (v) _v = (v); \
    ((uint16_t *) _image->addr)[(_image->width * _y) + _x] = _v; \
})

#define PUT_PIXEL(image, x, y, color) \
({ \
    __typeof__ (image) _image = (image); \
    __typeof__ (x) _x = (x); \
    __typeof__ (y) _y = (y); \
    uint8_t *_color = (color); \
    uint8_t *p = &((uint8_t *) _image->addr)[(_image->width * _y * 3) + (_x * 3)]; \
    p[0] = _color[0]; \
    p[1] = _color[1]; \
    p[2] = _color[2]; \
})

void setTime(Timer *time);

long getTime(Timer *time);

void updateTime(Timer *time);

int checkCommand();

void setCommand(int d)

void robot_init();

void eyeCalibration(Queue* queue);

void enqueueAnimation(Queue* queue, int arr[][10], int rows);

void moveBody(Queue* queue, int command);

void delay(int mili);

void updateTrackData(int left, int right);

void checkQueue(Queue* queue);

void sendData(int v1, int v2);

void closeSerial();

uint16_t pulseWidth(int channel, float degree);

void setServoAngle();

uint8_t sendCommand(int op1, int op2);

void initPOS();

int initHardware(int adpt, int addr, int freq);

void solveIK(int y, int d, double *theta1, double *theta2);

void updateHead(int x, int y, int d);

void updatePos(Servo* position, float easedAmt);

void updateBodyPos();

int checkBodyPos(Servo *serv, int channel);

void updateTrackCoord(int x, int y);

void setTrackSpeed();

void resetState();

void stopTracks();

Point convert_angle(int angle);

int percent(int a, int b);

void toHSV(color *hsv, uint8_t* pixel);

void process_Image(image* src);

void check_Roi(roi* rect);

void grid_Roi(image* src, roi* rect);


#endif