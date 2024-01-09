#include "robot.h"

//int fd;
//int addr = 0x40;
//#define QUEUE_POLLING_RATE 1
//time_t startTime, currentTime;

//int initHardware(int adpt, int addr, int freq) {
//  int afd = PCA9685_openI2C(adpt, addr);
//  PCA9685_initPWM(afd, addr, freq);
//  return afd;
//}

Queue* q = initQueue();
//int serial_port;

void setup()
{
  robot_init();
}


void walle()
{
  time(&startTime);

      while(true) {
        time(&currentTime);
        double elapsed = difftime(currentTime, startTime);

        if(elapsed > QUEUE_POLLING_RATE){
            startTime = currentTime;
            //int c = getCount(q);
          if(queueNotEmpty(q))
          {
            iterateServos(q);
          }
            
        }

    }

    queue.dequeueAll(li);
    return 0;
}

void updateServo(int id, int angle)
{
  setServoAngle(id, angle);
}