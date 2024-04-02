#include "robot.h"

int addr = 0x40;
unsigned int setServo[_PCA9685_CHANS] = {0};
unsigned int offVals[_PCA9685_CHANS] = {0};

// Default POS - Min POS - Max POS
int controls[9][3] = {
    {100, 25, 175}, // Head
    {90, 25, 120},  // Neck
    {95, 50, 95},   // Left Eye
    {95, 65, 95},   // Right Eye
    {180, 1, 180},  // Left Track
    {120, 1, 180},  // Left Arm
    {1, 1, 180},    // Right Track
    {55, 1, 180},   // Right Arm
    {130, 30, 170}, // Bottom Neck
};

void robot_init()
{
    wiringPiSetup();
    serial_port = serialOpen("/dev/ttyAMA0", 9600);
    sleep(1);
    serialFlush(serial_port);

    fd = initHardware(1, addr, 50);

    sleep(2);

    /*  for(int i = 0; i < 9; i++)
      {
          //servos[i].defPos = controls[i][0];
          servos[i].pos = controls[i][0];
          servos[i].min = controls[i][1];
          servos[i].max = controls[i][2];
          servos[i].p = 0.0000001;
          servos[i].d = 0.0000001;
      }
  */
    posX.pos = controls[0][0];
    posX.p = 0.0045;
    posX.d = 0.039;
    posX.prevError = 0;
    posX.min = controls[0][1];
    posX.max = controls[0][2];

    posY.pos = controls[1][0];
    posY.p = 0.004;
    posY.d = 0.008;
    posY.prevError = 0;
    posY.min = controls[1][1];
    posY.max = controls[1][2];

    tilt.pos = controls[8][0];
    tilt.p = 0.055;
    tilt.d = 0.020;
    tilt.prevError = 0;
    tilt.min = controls[8][1];
    tilt.max = controls[8][2];

    // initPOS();
}

int initHardware(int adpt, int addr, int freq)
{
    int afd = PCA9685_openI2C(adpt, addr);
    PCA9685_initPWM(afd, addr, freq);
    return afd;
}

void delay(int mili)
{
    usleep(mili * 1000);
}

void sendData(int value1, int value2)
{
    //char strData[12];
    //sprintf(strData, "%03d %03d %02d", value1, value2, ctrlId);
    int leftSpeed = 0;
int rightSpeed = 0;

if(value1 > 0)
  leftSpeed = mapRange(60,250,8,15,value1);
else
  leftSpeed = mapRange(60,250,1,7,abs(value1));

if(value2 > 0)
  rightSpeed = mapRange(60,250,8,15,value2);
else
  rightSpeed = mapRange(60,250,1,7,abs(value2));

    unsigned char x = 0;
    x = (leftSpeed << 4) | rightSpeed;
    serialPutchar(serial_port, x);

    //serialFlush(serial_port);
}

void closeSerial()
{
    serialClose(serial_port);
}

uint16_t pulseWidth(int channel, float degree)
{
    if (degree < controls[channel][1])
        degree = controls[channel][1];
    else if (degree > controls[channel][2])
        degree = controls[channel][2];
    // controls[channel][2] = degree;
    return round(degree * (512 - 102) / 180 + 102);
}

void setServoAngle()
{ 
    //unsigned int onVals[_PCA9685_CHANS] = {0};
    //unsigned int offVals[_PCA9685_CHANS] = {0};

    //onVals[channel] = round(pulseWidth);

    PCA9685_setPWMVals(fd, addr, offVals, setServo);
    setServo[_PCA9685_CHANS] = 0;
}

void initPOS()
{
    for (int i = 0; i <= 8; i++)
    {
        //setServoAngle(i, controls[i][0]);
        delay(250);
    }
    delay(250);
    // eyeCalibration();
}

void eyeCalibration()
{
    int delayAmt = 500;
    if (servos[3].pos != controls[3][2])
        //setServoAngle(3, controls[3][2]);
    if (servos[2].pos != controls[2][2])
        //setServoAngle(2, controls[2][2]);

    if (servos[0].pos != 90)
    {
        //setServoAngle(0, 90);
        delay(1000);
    }

    //setServoAngle(2, controls[2][1]); // Left down
    delay(delayAmt);
    //setServoAngle(3, controls[3][1]); // Right down
    delay(delayAmt);
    //setServoAngle(2, controls[2][2]); // Left up
    delay(delayAmt);
    //setServoAngle(3, controls[3][2]); // Down up
    delay(delayAmt);
    //setServoAngle(3, controls[3][1]); // Left and Right Down
    //setServoAngle(2, controls[2][1]);
    delay(delayAmt + 200);
    //setServoAngle(3, controls[3][2]); // Left and Right Up
    //setServoAngle(2, controls[2][2]);
}

void updatePD(Servo *newPos, int error)
{
    if (newPos->prevError != 0)
    {
        newPos->pos += (error * newPos->p + (error - newPos->prevError) * newPos->d);

        if (newPos->pos > newPos->max)
            newPos->pos = newPos->max;
        else if (newPos->pos < newPos->min)
            newPos->pos = newPos->min;
    }
    newPos->prevError = error;
}

void updateHead(int x, int y)
{
    if (x != 0)
    {
        updatePD(&posX, x - CAM_WIDTH_F);
        setServo[0] = pulseWidth(0, posX.pos);
        if(posY.pos > 100 && tilt.pos > 90)
        {
            if(posX.pos >= 130)
                setServo[0] = pulseWidth(0, 130);
            if(posX.pos <= 70)
                setServo[0] = pulseWidth(0, 70);
        }
    }
    
    if (y != 0)
    {
        updatePD(&posY, y - CAM_HEIGHT_F);
        setServo[1] = pulseWidth(1, posY.pos);
        int tilt_error = posY.pos - ((posY.min + posY.max) / 2);
        updatePD(&tilt, tilt_error);
        setServo[8] = pulseWidth(8, tilt.pos);
    }
    
    setServoAngle();
}

void updateCoords(objectCoord *obj)
{
    if (obj->state != 0)
    {
        //if(obj->state == 1)
            updateHead(obj->x, obj->y);
        //else
         //   updateTracks(obj);//obj->x - CAM_WIDTH_F, CAM_HEIGHT - obj->y);

        if (obj->x == CAM_WIDTH_F && obj->y == CAM_HEIGHT_F)
            resetState(obj);
        else
        {
            //if(obj->x != CAM_WIDTH_F)
            //{
            if (obj->x > CAM_WIDTH_F)
                obj->x = constrain(obj->x -= 3, CAM_WIDTH_F, CAM_WIDTH);//-= 1;
            else if (obj->x < CAM_WIDTH_F)
                obj->x = constrain(obj->x += 3, 0, CAM_WIDTH_F);
            //}
            
           //if(obj->y != CAM_HEIGHT_F)
            //{
            if (obj->y > CAM_HEIGHT_F)
                obj->y = constrain(obj->y -= 3, CAM_HEIGHT_F, CAM_HEIGHT);
            else if (obj->y < CAM_HEIGHT_F){
                obj->y = constrain(obj->y += 3, 0, CAM_HEIGHT_F);}
           // }
            
        }
    }

}

void updateTrackCoord(int x, int y, Speed *obj)
{
    if(obj->state != 1)
        obj->state = 1;

    int x_st = abs(x - CAM_WIDTH_F);
    int y_st = abs(y - CAM_HEIGHT_F); //CAM_HEIGHT - obj->y;
    //x = constrain(obj->x += 10, 0, CAM_WIDTH_F);
    obj->left = LOW_SPEED;
    obj->right = LOW_SPEED;
    int speed = mapRange(0,CAM_HEIGHT_F,0,190,y_st);//constrain(y, 0, 195);
    int turn = mapRange(0,CAM_WIDTH_F,0,190,x_st);//constrain(x, 0, 195);
    obj->left += speed;
    obj->right += speed;
    if(x < CAM_WIDTH_F)
    {
        obj->left = constrain(obj->left + turn, 60, 250);
        obj->right = constrain(obj->right - turn, 60, 250);
    }
    else
    {
        obj->right = constrain(obj->right + turn, 60, 250);
        obj->left = constrain(obj->left - turn, 60, 250);
    }
        
    if(y < CAM_HEIGHT_F)
    {
       obj->left *= -1;
       obj->right *= -1;
        //sendData(-left, -right);
    }
    //else
    //sendData(left, right);
}

void setTrackSpeed(Speed *obj)
{
    if(obj->state != 0)
        sendData(obj->left, obj->right); 

    if(obj->left > LOW_SPEED && obj->right > LOW_SPEED)
    {
        obj->left -= 1;
        obj->right -= 1;
    }
    else if(obj->left < -LOW_SPEED && obj->right < -LOW_SPEED)
    {
        obj->left += 1;
        obj->right += 1;
    }
    else
        obj->state = 0;
   
}

void resetState(objectCoord *obj)
{   
    if(obj->state == 1)
    {
        setServo[_PCA9685_CHANS] = 0;
        setServoAngle();
    }
  //  else if(obj->state == 2)
   // {
   //     stopTracks();
   // }
    obj->state = 0;
}

void stopTracks()
{
    sendData(1, 0);
}

Point convert_angle(int angle) {
    Point result;

    double radians = (angle * 3.14159) / 180.0;

    result.x = mapRange(-CAM_WIDTH,CAM_WIDTH,1,CAM_WIDTH,CAM_WIDTH * cos(radians));
    result.y = mapRange(-CAM_HEIGHT,CAM_HEIGHT,1,CAM_HEIGHT,CAM_HEIGHT * sin(radians));

    return result;
}