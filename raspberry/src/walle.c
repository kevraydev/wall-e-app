#include <wiringSerial.h>
#include <stdio.h>

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