#include "robot.h"

//int state = 0;
void setup()
{
  robot_init();
}

void walle()
{
    clock_gettime(CLOCK_MONOTONIC, &startTime);

    while(true)
    {
      if(!obj.state && !track.state)
      {

      }
      else
      {
      
        clock_gettime(CLOCK_MONOTONIC, &currentTime);

        long elapsed = (currentTime.tv_sec - startTime.tv_sec)*1000 + 
                       (currentTime.tv_nsec - startTime.tv_nsec)/1000000;
                       
        if(elapsed > POLLING_RATE)
        {
          startTime = currentTime;
          if(obj.state == 1)
          {
            //updateCoords(&obj);
            updateHead(obj.x, obj.y);
            //updateNeckServos();
          }

          if(track.state == 1)
          {
            //updateCoords(&track);
            setTrackSpeed(&track);
          }
          

        }
      }
    }

    return;
}


void update(int id, int angle)
{

  Point coord;
  coord = convert_angle(angle);
  if(id == 0)
  {
    obj.x = coord.x;
    obj.y = coord.y;
    if(obj.state != 1)
      obj.state = 1;
    
  }
  else if(id == 9) 
  {
    updateTrackCoord(coord.x, coord.y, &track);

  }


}

void setHeadxy(int x, int y)
{
  obj.x = coord.x;
  obj.y = coord.y;
  if(obj.state != 1)
    obj.state = 1;
}

//void updateServo(int id, int angle)
//{
//  setServo[id] = pulseWidth(id, angle);
//  setServoAngle();
//}

void updateServo(int id, int angle)
{
  Point coord;
  coord = convert_angle(angle);
  //track.x = coord.x;
  //  track.y = coord.y;
  if(id == 0)
  {
    setServo[id] = pulseWidth(id, angle);
  setServoAngle();
  }
  
}