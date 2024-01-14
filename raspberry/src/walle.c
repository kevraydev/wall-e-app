#include "robot.h"

Queue* q = initQueue();

void setup()
{
  robot_init();
}


void walle()
{
    clock_gettime(CLOCK_MONOTONIC, &startTime);

    while(true)
    {
        clock_gettime(CLOCK_MONOTONIC, &currentTime);

        long elapsed = (currentTime.tv_sec - startTime.tv_sec)*1000 + 
                       (currentTime.tv_nsec - startTime.tv_nsec)/1000000;
                       
        if(elapsed > QUEUE_POLLING_RATE)
        {
            startTime = currentTime;

            if(queueNotEmpty(q))
            {
                iterateServos(q);
            }   
        }
    }

    dequeueAll(q);
    return;
}


void update(int id, int value)
{
  if(getByKey(q, id) < 0)
    enqueue(q, id, value);
  else
    updateKey(q, id, value)
  
}

void updateServo(int id, int angle)
{
  setServoAngle(id, angle);
}