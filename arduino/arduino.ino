#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>
#include <Fonts/Nimbus_Sans_L_Bold_9.h>
#include "WalleIcon.h"
#include <Wire.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
//#include "queue.h"
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

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

//PCA9685 pwmController;

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
unsigned long last = 0;
unsigned long lastUpdate = 0;

//Control ID - Default POS - Current POS - Min POS - Max POS
/*
int controls[9][5] = {
  {0, 90, 90, 10, 175},//Head
  {1, 90, 90, 25, 170},//Neck
  {2, 95, 95, 50, 95},//Left Eye
  {3, 95, 95, 65, 95},//Right Eye
  {4, 180, 180, 1, 180},//Left Track
  {5, 120, 130, 1, 180},//Left Arm
  {6, 1, 1, 1, 180},//Right Track
  {7, 55, 55, 1, 180},//Right Arm
  {8, 130, 130, 20, 170},//Bottom Neck
};
*/
uint16_t leftSpeed;
uint16_t rightSpeed;
int motorState = 1;


void setup(void) {
  Serial.begin(9600);
  Wire.begin();
  softSerial.begin(9600);
  mPlayer.begin(softSerial);
  mPlayer.volume(23);
  tft.initR(INITR_144GREENTAB); // Init ST7735R chip
  //Serial.println("started");

  tft.fillScreen(ST77XX_BLACK);
  tft.setFont(&Nimbus_Sans_L_Bold_9);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(1, 10);
  tft.drawBitmap(6, 20, walle_icon, 31, 30, 0xFFE0);
  
  tft.println("SOLAR CHARGE LEVEL");
  //pwmController.resetDevices(); 
  //pwmController.init(); 
  //pwmController.setPWMFrequency(50);
  
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  updateState(0);
  delay(100);
  //mPlayer.play(3);
  //delay(1500);
  //initPOS();
  //delay(1500);
}

void loop() {
  String dataIn = "";
  if(Serial.available()) {
    dataIn = Serial.readStringUntil('\n');
    
    int angle = dataIn.substring(0, 3).toInt();
    int ctrlId = dataIn.substring(4, 6).toInt();
    //int ctrlId = dataIn.substring(7, 9).toInt();
    
    if(angle > 0) {

        //updateControl(angle, ctrlId);
    }

    delay(1);
  }

  if((unsigned long)(millis() - last) >= 8000) {
    displaySolar();
    last = millis();
  }

  updateTracks();

}

void updateControl(int a, int c)
{
  if(c > 8) {
    //setTrackDirection(a, d);
    switch(c)
    {
      case 9:
              setTrackDirection(a, c);
              break;
      case 10:
              updateState(0);
              break;
      case 11:  
              break;
    }
  }
 // else {
  //  setServoAngle(c, a);
 // }
}


float mapf(float x, float in_min, float in_max, float out_min, float out_max) 
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
/*
void setServoAngle(int channel, float degree)
{
  if(degree < controls[channel][3])
    degree = controls[channel][3];
  else if(degree > controls[channel][4])
    degree = controls[channel][4];
  controls[channel][2] = degree;
  uint16_t pulseWidth = degree * (512 - 102) / 180 + 102;
  pwmController.setChannelPWM(channel, pulseWidth); 
}

void initPOS()
{
  
  for(int i = 0; i <= 8; i++)
  {
    setServoAngle(controls[i][0], controls[i][1]); 
    delay(250);
  }
  delay(250);
  eyeCalibration();
}


void eyeCalibration()
{
  int delayAmt = 500;
  if(controls[3][2] != controls[3][4])
    setServoAngle(3, controls[3][4]);
  if(controls[2][2] != controls[2][4])
    setServoAngle(2, controls[2][4]);

  if(controls[0][2] != 90)
  {
    setServoAngle(0, 90);
    delay(1000);
  }

  setServoAngle(2, controls[2][3]);//Left down
  delay(delayAmt);
  setServoAngle(3, controls[3][3]);//Right down
  delay(delayAmt);
  setServoAngle(2, controls[2][4]);//Left up
  delay(delayAmt);
  setServoAngle(3, controls[3][4]);//Down up
  delay(delayAmt);
  setServoAngle(3, controls[3][3]);//Left and Right Down
  setServoAngle(2, controls[2][3]);
  delay(delayAmt+200);
  setServoAngle(3, controls[3][4]);//Left and Right Up
  setServoAngle(2, controls[2][4]);
}

*/
void updateTracks()
{
  if((unsigned long)(millis() - lastUpdate) >= 500) {
  //  Serial.print("Time ");
  // Serial.println((unsigned long)(millis() - lastUpdate));
  
  if(leftSpeed > 0 || rightSpeed > 0)
  {
    
    leftSpeed -= leftSpeed >> 4;
    rightSpeed -= rightSpeed >> 4;
  }
  else if(motorState != 0)
  {
    updateState(0);
    changeSpeed(0, 0);
    }
  }

}

void setTrackDirection(int angle, int distance)
{
  /*
    lastUpdate = millis();
    int speed = mapf(distance, 0, 40, 20, 50);

    float rads = (angle * PI)/180;
    float cosA = cos(rads);
    float sinA = sin(rads);
    float absCos = abs(cosA);

    if(absCos > 0.4)
    {
      if(cosA > 0)
        leftSpeed += speed;
      else
        rightSpeed += speed;
    }
    else
    {
      leftSpeed += speed;
      rightSpeed += speed;
    }
    
    if(sinA > 0)
      updateState(1);
    else
      updateState(2);
    leftSpeed = constrain(leftSpeed, 100, 255);
    rightSpeed = constrain(rightSpeed, 100, 255);

    */
    //changeSpeed(leftSpeed, rightSpeed);
}

int BattVoltage() {
  int batValue = analogRead(A0);
  float voltage = batValue * (2.95 / 1023.0);
  int level = mapf(voltage, 1.30, 2.60, 1, 12);
  //tft.println(voltage);
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
  if(motorState != state)
  {
    motorState = state;
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
}

void changeSpeed(int left, int right)
{
  analogWrite(ENA, left);
  analogWrite(ENB, right);
}