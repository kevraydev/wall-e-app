#include <wiringSerial.h>
#include <stdio.h>
#include "queue.h"

cQueue q;
Queue* li = q.initQueue();
int serial_port;

void setup()
{
    wiringPiSetup();
    serial_port = serialOpen ("/dev/ttyAMA0", 9600);
    sleep(2);
}


void sendData(int angle, int distance, int ctrlId)
{
    char strData[20];
    sprintf(strData, "%03d %02d %02d", angle, distance, ctrlId);

    serialPuts(serial_port, strData);
}

void closeSerial()
{
    serialClose(serial_port);
}


void queueMovement(int a, int c)
{
  if(queueNotEmpty(li))
  {
    if(q.updateKey(li, c, a) < 0)
      q.push(li, c, a);
  }
  else
    q.push(li, c, a);
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
