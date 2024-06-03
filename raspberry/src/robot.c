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
{//
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
    //posX.p = 0.0042;
    //posX.d = 0.030;
    posX.p = 0.01;
    posX.d = 0.001;
    posX.prevError = 0;
    posX.min = controls[0][1];
    posX.max = controls[0][2];

    posY.pos = controls[1][0];
    posY.p = 0.05;
    posY.d = 0.005;
    posY.prevError = 0;
    posY.min = controls[1][1];
    posY.max = controls[1][2];

    tilt.pos = controls[8][0];
    tilt.p = 0.015;
    tilt.d = 0.010;
    tilt.prevError = 0;
    tilt.min = controls[8][1];
    tilt.max = controls[8][2];
    bGrid.y = 550;
    bGrid.x = 270;
    /*roi.b_Size = 15;
    roi.b_Step = 40;
    roi.x = 1;
    roi.y = 1;
    roi.w = CAM_WIDTH;
    roi.h = CAM_HEIGHT;*/

    //r_image.addr = malloc(CAM_WIDTH*CAM_HEIGHT);
    //r_image.width = CAM_WIDTH;
    //r_image.height = CAM_HEIGHT;
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
    for (int i = 0; i <= 8; i++){
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

void updatePD(Servo *newPos, float error)
{
    if (newPos->prevError != 0)
    {
        newPos->targetPos += (error * newPos->p + (error - newPos->prevError) * newPos->d);
        
        if (newPos->targetPos > newPos->max)
            newPos->targetPos = newPos->max;
        else if (newPos->targetPos < newPos->min)
            newPos->targetPos = newPos->min;
    }
    newPos->prevError = error;
}

void updateHead(int x, int y)
{
    if (x != 0)
    {
        updatePD(&posX, x - CAM_WIDTH_F);
        posX.pos = sl(posX.pos, posX.targetPos, easeIn(0.1));
        setServo[0] = pulseWidth(0, (int)posX.pos);
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
        int y_h = y - CAM_HEIGHT_F;
        bGrid.x = 270;
        bGrid.y += y_h;
        if(bGrid.y > 620)
            bGrid.y = 620;
        else if(bGrid.y < 1)
            bGrid.y = 1;
        //float theta1 = 0;
        //float theta2 = 0;
        int a1 = 480;
        int a2 = 320;
        float r1 = sqrt(s(bGrid.x)+s(bGrid.y));
        float phi1 = acos((s(a2)-s(r1)-s(a1))/(-2.0 *r1*a1));
        float phi2 = atan2(bGrid.y,bGrid.x);
        float phi3 = acos((s(r1)-s(a1)-s(a2))/(-2.0 *a1*a2));
        float angle1 = RADTODEG(phi2 - phi1);
        float angle2 = RADTODEG(phi3);
        if(!isnan(angle1))
            posY.targetPos = constrain(120 - angle1, 25, 120);

        if(!isnan(angle2))
            tilt.targetPos = constrain( angle2, 30, 170);//theta 2

        tilt.targetPos = map(0,180,170,30,tilt.targetPos);

        updateNeckServos();
   /*    posY.pos = sl(posY.pos, posY.targetPos, easeIn(0.1));
        tilt.pos = sl(tilt.pos, tilt.targetPos, easeIn(0.1));
       //updatePD(&posY, CAM_HEIGHT_F - y);
        setServo[1] = pulseWidth(1, (int)posY.pos);
        //int tilt_error = ((posY.min + posY.max) / 2) - posY.pos;
        //updatePD(&tilt, tilt_error);
        setServo[8] = pulseWidth(8, (int)tilt.pos); */ 
    }
    
    setServoAngle();
}

void updateNeckServos()
{
    if(posY.targetPos != 0 || tilt.targetPos != 0)
    {
        if(posY.pos != posY.targetPos)
        {
            posY.pos = sl(posY.pos, posY.targetPos, easeIn(0.05));
            setServo[1] = pulseWidth(1, (int)posY.pos);
        }

        if(tilt.pos != tilt.targetPos)
        {
            tilt.pos = sl(tilt.pos, tilt.targetPos, easeIn(0.05));
            setServo[8] = pulseWidth(8, (int)tilt.pos); 
        }
    }

}

void updateCoords(objectCoord *obj)
{
    if (obj->state != 0)
    {
        //if(obj->state == 1)
            updateHead(obj->x, obj->y);
        //else
         //   updateTracks(obj);//obj->x - CAM_WIDTH_F, CAM_HEIGHT - obj->y);
//
        if (obj->x == CAM_WIDTH_F && obj->y == CAM_HEIGHT_F)
            resetState(obj);
        else
        {
            //if(obj->x != CAM_WIDTH_F)
            //{
            if (obj->x > CAM_WIDTH_F)
                obj->x = constrain(obj->x -= 5, CAM_WIDTH_F, CAM_WIDTH);//-= 1;
            else if (obj->x < CAM_WIDTH_F)
                obj->x = constrain(obj->x += 5, 0, CAM_WIDTH_F);
            //}
            
           //if(obj->y != CAM_HEIGHT_F)
            //{
            if (obj->y > CAM_HEIGHT_F)
                obj->y = constrain(obj->y -= 5, CAM_HEIGHT_F, CAM_HEIGHT);
            else if (obj->y < CAM_HEIGHT_F){
                obj->y = constrain(obj->y += 5, 0, CAM_HEIGHT_F);}
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

Point convert_angle(int angle)
{
    Point result;
    double radians = (angle * 3.14159) / 180.0;

    result.x = mapRange(-CAM_WIDTH,CAM_WIDTH,1,CAM_WIDTH,CAM_WIDTH * cos(radians));
    result.y = mapRange(-CAM_HEIGHT,CAM_HEIGHT,1,CAM_HEIGHT,CAM_HEIGHT * sin(radians));

    return result;
}

int percent(int a, int b)
{
    float result = ((float)abs((a-b)) / ((a+b)/2.0)) * 100;
    return (int)result;
}

void toHSV(color *hsv, uint8_t* pixel)
{
int H;
float cmax,cmin,delta,S,V;
float r = GET_R(pixel)/255.0;
float g = GET_G(pixel)/255.0;
float b = GET_B(pixel)/255.0;

cmax = CMAX(r,g,b);
cmin = CMIN(r,g,b);
delta = cmax-cmin;

//Hue
if(delta == 0) H = 0;
else
{
    if(cmax == r) H = 60*((g-b)/delta);
    if(cmax == g) H = 60*((b-r)/delta + 2);
    if(cmax == b) H = 60*((r-g)/delta + 4);
}
//Saturation
if(cmax == 0) S = 0;
else S = (delta/cmax) * 100;
//Value
V = cmax * 100;
if(H < 0) H = 300 + abs(H);
hsv->H = H;
hsv->S = S;
hsv->V = V;
}

void process_Image(image* src)
{
    check_Roi(&rect);

    grid_Roi(src, &rect);
}

void check_Roi(roi* rect)
{
    if(rect->p == NULL)
    {
        //rect = (roi *)malloc(sizeof(roi));
        if(rect->b_Size == 0)
            rect->b_Size = 20;
        if(rect->b_Step == 0)
            rect->b_Step = 10;
        if(rect->w == 0)
            rect->w = CAM_WIDTH;
        if(rect->h == 0)
            rect->h = CAM_HEIGHT;

        int block_size = rect->b_Size + rect->b_Step;

        float grid_w = rect->w / block_size;
        int g_w = (int)grid_w;
        if(grid_w - g_w)
        {
            int p_x = rect->w - (g_w * (rect->b_Size + rect->b_Step));
            rect->x = (int)(p_x/2);
        }

        float grid_h = rect->h / block_size;
        int g_h = (int)grid_h;
        if(grid_h - g_h)
        {
            int p_y = rect->h - (g_h * (rect->b_Size + rect->b_Step));
            rect->y = (int)(p_y/2);
        }
        rect->num_Blocks = g_w * g_h;
        rect->p = (color *)malloc((rect->num_Blocks) * sizeof(color));

    }
    //check_Roi(src);
}

void grid_Roi(image* src, roi* rect)
{
    color pix;
    int step = rect->b_Size + rect->b_Step;
    int pixels = rect->b_Size * rect->b_Size;
    //int t_blocks = rect->num_Blocks;
    int t = 0;
    if(rect->step < STEP_COUNT)
        rect->step += 1;
    else
        rect->step = 0;

    for(int y = rect->y; y < rect->h; y += step){   
        for(int x = rect->x; x < rect->w; x += step){
            int h_sum, s_sum, v_sum = 0;
            
            for(int ry = y, yy = y + rect->b_Size; ry < yy; ry++){
                //uint16_t *p = PIXEL_ROW(src, ry);
                for(int rx = x, xx = x + rect->b_Size; rx < xx; rx++){
                    uint8_t* pixel = GET_PIXEL(src, rx, ry);
                    toHSV(&pix, pixel);

                    h_sum += pix.H;
                    s_sum += pix.S;
                    v_sum += pix.V;
                   // if(rect->step > 0 && rect->step <= STEP_COUNT)
                   // {
                        //if(rect->p[t].diff == 1)
                        //
                        if(rect->p[t].diff == 1)//if(pix.H < 15 && pix.S > 70 && pix.V > 70)//if(GET_R(pixel) < 100)
                            PUT_PIXEL(src, rx, ry, RGB(100, 250, 30));
                   // }
                    
                }
            }
           //if(rect->step > 0)
           // {
            int h = h_sum / pixels;
            int s = s_sum / pixels;
            int v = v_sum / pixels;
            if(percent(rect->p[t].V, v) > 70)
                rect->p[t].diff = 1;
            else
                rect->p[t].diff = 0;
           // }
            rect->p[t].H = h;
            rect->p[t].S = s;
            rect->p[t].V = v;
            t += 1;
            //
        }
    }
}
