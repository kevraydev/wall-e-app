#include <wiringSerial.h>
#include <stdio.h>

int serial_port;

void setup()
{
    wiringPiSetup();
    serial_port = serialOpen ("/dev/ttyAMA0", 9600);
    sleep(2);
}


void sendData(int data)
{
    serialPrintf(serial_port, "%d", data);
}

void closeSerial()
{
    serialClose(serial_port);
}