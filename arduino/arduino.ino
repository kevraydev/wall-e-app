#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include <PCA9685.h>
#include <Fonts/Nimbus_Sans_L_Bold_9.h>
#include "WalleIcon.h"
#include "PCA9685.h"
#include <Wire.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

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

PCA9685 pwmController;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
unsigned long last = 0;

//float batVolt_Max = 12.3;
//float batVolt_Min = 6;

void setup(void) {
  Serial.begin(9600);
  Wire.begin();
  softSerial.begin(9600);
  mPlayer.begin(softSerial);
  mPlayer.volume(23);
  tft.initR(INITR_144GREENTAB); // Init ST7735R chip, green tab
  
  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;
  tft.setFont(&Nimbus_Sans_L_Bold_9);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(1, 10);
  tft.drawBitmap(6, 20, walle_icon, 31, 30, 0xFFE0);
  
  tft.println("SOLAR CHARGE LEVEL");
  pwmController.resetDevices(); 
  pwmController.init(); 
  pwmController.setPWMFrequency(50);
  
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  updateState(0);
  delay(2000);
  mPlayer.play(3);
}

void loop() {
  String dataIn = "";
  if(Serial.available()) {
    dataIn = Serial.readStringUntil('\n');
    
    int angle = dataIn.substring(0, 3).toInt();
    int distance = dataIn.substring(4, 6).toInt();
    int ctrlId = dataIn.substring(7, 9).toInt();
    
    if(angle > 10)
      setServoAngle(ctrlId, angle);
      else {
        Serial.read();
    }

    delay(10);
  }

  if((unsigned long)(millis() - last) >= 5000) {
    displaySolar();
    last = millis();
  }

}

float mapf(float x, float in_min, float in_max, float out_min, float out_max) 
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setServoAngle(int channel, float degree) {
  uint16_t pulseWidth = degree * (512 - 102) / 180 + 102;
  pwmController.setChannelPWM(channel, pulseWidth); 
}

int BattVoltage() {
  int batValue = analogRead(A0);
  float voltage = batValue * (3.2 / 1023.0);
  //float actVoltage = mapf(voltage, 1.5, 3.2, batVolt_Min, batVolt_Max);
  //int level = mapf(actVoltage, batVolt_Min, batVolt_Max, 1, 12);
  int level = mapf(voltage, 1.5, 3.2, 1, 12);
  return level;
}

void displaySolar() {
  int x = BattVoltage();
  int barArr[12] = {104, 96, 88, 80, 72, 64, 56, 48, 40, 32, 24, 16};
  uint16_t color = ST77XX_YELLOW;
  if(x == 1)
    tft.fillRoundRect(44, 112, 84, 15, 1, ST77XX_RED);
  else {
    tft.fillRoundRect(44, 112, 84, 15, 1, color);
    for(int i = 1; i <= 12; i++){
      if( i > x){
        color = ST77XX_BLACK;
      }
      tft.fillRoundRect(44, barArr[i-1], 84, 5, 1, color);
    }
  }
}

void updateState(int state)
{
    switch(state)
    {
    case 0:
        digitalWrite(IN1, LOW);//tracks off
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        changeSpeed(0,0);
        break;
    case 1:
        digitalWrite(IN1, LOW);//tracks forward
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        break;
    case 2:
        digitalWrite(IN1, HIGH);//tracks reverse
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        break;
    case 3:
        digitalWrite(IN1, LOW);//left turn
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        break;
    case 4:
        digitalWrite(IN1, HIGH);//right turn
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        break;
    }
}

void changeSpeed(int left, int right)
{
  analogWrite(ENA, left);
  analogWrite(ENB, right);
}