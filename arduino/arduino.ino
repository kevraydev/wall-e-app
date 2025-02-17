#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include <Fonts/Nimbus_Sans_L_Bold_9.h>
#include "WalleIcon.h"
#include <Wire.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
// #include "queue.h"
#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#define mapRange(a1,a2,b1,b2,s) (b1 + (s-a1)*(b2-b1)/(a2-a1))
#define TFT_CS 8
#define TFT_RST 12
#define TFT_DC 10

#define ENA 3
#define ENB 9
#define IN1 4
#define IN2 7
#define IN3 6
#define IN4 5

SoftwareSerial softSerial(16, 15);
DFRobotDFPlayerMini mPlayer;

// PCA9685 pwmController;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
unsigned long last = 0;
unsigned long lastUpdate = 0;

int sensorValue2 = 0;
int motorState = 0;

void setup(void)
{
  Serial.begin(9600);
  Wire.begin();
  softSerial.begin(9600);
  mPlayer.begin(softSerial);
  mPlayer.volume(23);
  tft.initR(INITR_144GREENTAB); // Init ST7735R chip
  // Serial.println("started");

  tft.fillScreen(ST77XX_BLACK);
  tft.setFont(&Nimbus_Sans_L_Bold_9);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(1, 10);
  tft.drawBitmap(6, 20, walle_icon, 31, 30, 0xFFE0);

  tft.println("SOLAR CHARGE LEVEL");

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // updateState(0);
  delay(100);
   //mPlayer.play(1);
  // delay(1500);
}

void loop()
{

  if (Serial.peek() > 0)
  {

    int x = Serial.read();
    int value1 = (x & 0b11110000) >> 4;
    int value2 = (x & 0b00001111);


    if (value1 > 0 && value2 > 0)
    {

      updateTracks(value1, value2);
      lastUpdate = millis();
    }
    else if(value1 == 0 && value2 == 1)
    {
        int sensorValue = analogRead(A6);
        int angle = mapf(sensorValue, 30, 900, 175, 25);//mapRange(35,950,180,10, sensorValue);
        //tft.println(angle);
        delay(5);
        Serial.write(angle);
        delay(5);
    }
    else if(value1 == 1 && value2 == 0)
    {
      updateState(0,0);
    }

    //delay(5);
  }


  if ((unsigned long)(millis() - last) >= 6000)
  {
    displaySolar();
    last = millis();
  }

  if((unsigned long)(millis() - lastUpdate) >= 250) {
    if(motorState != 0)
      updateState(0, 0);
  }
  //delay(5);
  // updateTracks();
}

float mapf(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void updateTracks(int left, int right)
{

int leftSpeed = 0;
int rightSpeed = 0;

if(left >= 8)
  leftSpeed = mapRange(8,15,70,250,left);
else
  leftSpeed = mapRange(1,7,70,250,left);

if(right >= 8)
  rightSpeed = mapRange(8,15,70,250,right);
else
  rightSpeed = mapRange(1,7,70,250,right);

  updateState(left, right);
  
  //tft.println(left);

  //if (left < 8)
  //  leftSpeed *= -1;

  //if (right < 8)
  //  rightSpeed *= -1;
  changeleft(leftSpeed);
  changeright(rightSpeed);
  //  else if(motorState != 0)
  //  {
  //    updateState(0);
  //    changeSpeed(0, 0);
  //    }
  //}
}

int BattVoltage()
{
  int batValue = analogRead(A0);
  float voltage = batValue * (2.95 / 1023.0);
  int level = mapf(voltage, 1.30, 2.60, 1, 12);
  // tft.println(voltage);
  return level;
}

void displaySolar()
{
  int x = BattVoltage();

  int barArr[12] = {104, 96, 88, 80, 72, 64, 56, 48, 40, 32, 24, 16};
  uint16_t color = ST77XX_YELLOW;
  if (x == 1)
    tft.fillRoundRect(44, 112, 84, 15, 1, ST77XX_RED);
  else
  {
    tft.fillRoundRect(44, 112, 84, 15, 1, color);
    for (int i = 1; i <= 12; i++)
    {
      if (i > x)
      {
        color = ST77XX_BLACK;
      }
      tft.fillRoundRect(44, barArr[i - 1], 84, 5, 1, color);
    }
  }
}

void updateState(int left, int right)
{
    if (left == 0 && right == 0)
    {
      if (motorState != 0)
      {
        digitalWrite(IN1, LOW); // tracks off
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        stopTracks();
        motorState = 0;
      }
    }
    else if (left >= 8 && right >= 8)
    {
      if (motorState != 1)
      {
        digitalWrite(IN1, LOW); // tracks forward
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        motorState = 1;
      }
    }
    else if (left <= 7 && right <= 7)
    {
      if (motorState != 2)
      {
        digitalWrite(IN1, HIGH); // tracks reverse
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        motorState = 2;
      }
    }
    else if (left >= 8 && right <= 7)
    {
      if (motorState != 3)
      {
        digitalWrite(IN1, LOW); // left turn
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        motorState = 3;
      }
    }
    else if (left <= 7 && right >= 8)
    {
      if (motorState != 4)
      {
        digitalWrite(IN1, HIGH); // right turn
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        motorState = 4;
      }
    }
}

void changeleft(int left)
{
  analogWrite(ENA, left);
}
void changeright(int right)
{
  analogWrite(ENB, right);
}

void stopTracks()
{
  changeleft(0);
  changeright(0);
}