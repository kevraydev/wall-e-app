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


void update(int x, int y, int area)
{
  
//  if(getByKey(q, id) < 0)
//    enqueue(q, id, value);
//  else
//   updateKey(q, id, value)
  //updateCoords(x, y, area);
  obj.x = x;
  obj.y = y;
  obj.area = area;
  if(state != 0)
    state = 0;
}

void updateServo(int id, int angle)
{
  setServoAngle(id, angle);
}