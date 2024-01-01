
#include <wiringSerial.h>
#include <stdio.h>
#include <stdint.h>
#include <PCA9685.h>
#include <math.h>
//#include "queue.h"
int fd;
int addr = 0x40;
// Example for a second device. Set addr2 to device address (if set to 0x00 no second device will be used)
int fd2;
int addr2 = 0x00;

int initHardware(int adpt, int addr, int freq) {
  int afd = PCA9685_openI2C(adpt, addr);
  PCA9685_initPWM(afd, addr, freq);
  return afd;
}

 // pca.set_pwm_freq(50.0);
//cQueue q;
//Queue* li = q.initQueue();cd 
int serial_port;

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

void setup()
{
  wiringPiSetup();
  serial_port = serialOpen ("/dev/ttyAMA0", 9600);
  sleep(2);

  int adpt = 1;
  int freq = 50;
  fd = initHardware(adpt, addr, freq);
}

void sendData(int angle, int distance, int ctrlId)
{
  char strData[14];
  sprintf(strData, "%03d %02d", angle, ctrlId);
  //sprintf(strData, "%03d %02d %02d", angle, distance, ctrlId);

  serialPuts(serial_port, strData);
}

void closeSerial()
{
  serialClose(serial_port);
}


void setServoAngle(int channel, float degree)
{
  if(degree < controls[channel][3])
    degree = controls[channel][3];
  else if(degree > controls[channel][4])
    degree = controls[channel][4];
  controls[channel][2] = degree;
  uint16_t pulseWidth = degree * (512 - 102) / 180 + 102;

  unsigned int onVals[_PCA9685_CHANS] = {0};
  unsigned int offVals[_PCA9685_CHANS] = {0}; 
   
  offVals[channel] = round(pulseWidth);

  PCA9685_setPWMVals(fd, addr, onVals, offVals);
}


void updateServo(int id, int angle)
{
  setServoAngle(id, angle);
}


/*
void queueMovement(int a, int c)
{
  if(queueNotEmpty(li))
  {
    if(q.updateKey(li, c, a) < 0)
      q.enqueue(li, c, a);
  }
  else
    q.enqueue(li, c, a);
}

void updateMovement()
{
  int c = q.getCount(li);
  if(c != 0)
  {
    for(int i = 0; i < c; i++)
    {

    }
  }
}
*/