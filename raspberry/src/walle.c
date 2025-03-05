#include "robot.h"

void walle() {
  Queue* lq = initQueue();
  robot_init();
  //
  eyeCalibration(lq);
  
  Timer time_s, time_w;
  setTime(&time_s);
  setTime(&time_w);

  while(true) {
    
    if(getTime(&time_s) > POLL_RATE) {
      updateTime(&time_s);
      int command = checkCommand();
      moveBody(lq, 1);

 /*     if(command) {
        if(getTime(&time_w) > WAIT_T) {
          //
        }

      }
      else {
        setCommand(1);
        setTime(&time_w);
      }*/

    }
  }

  dequeueAll(lq);
  free(lq);
  return;
}

void update(int id, int angle, int distance) {
  Point coord;
  coord = convert_angle(angle);
  if(id == 0) {
    updateHead(coord.x, coord.y, distance);
    
  } else if(id == 9) {
    updateTrackCoord(coord.x, coord.y);

  }
  setCommand(0);
}

void updateServo(int id, int angle) {
  setServo[id] = pulseWidth(id, angle);
  setServoAngle();
}