#include "robot.h"

int state = 1;

void setup()
{
  robot_init();
}

void walle()
{
    clock_gettime(CLOCK_MONOTONIC, &startTime);

    while(true)
    {
      if(state)
      {

      }
      else if(obj.area != 0)
      {

      
        clock_gettime(CLOCK_MONOTONIC, &currentTime);

        long elapsed = (currentTime.tv_sec - startTime.tv_sec)*1000 + 
                       (currentTime.tv_nsec - startTime.tv_nsec)/1000000;
                       
        if(elapsed > POLLING_RATE)
        {
          startTime = currentTime;
          //updateHead(obj.x, obj.y, obj.area)
          updateCoords(&obj);
 
        }
      }
    }

    return;
}


void update(int angle)
{

  Point coord;
  coord = convert_angle(angle);
  obj.x = coord.x;
  obj.y = coord.y;
  obj.area = 1;
  if(state != 0)
    state = 0;
}

void updateServo(int id, int angle)
{
  setServo[id] = pulseWidth(id, angle);
  setServoAngle();
}