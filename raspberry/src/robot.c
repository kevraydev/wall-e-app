/*
Author: Kevin Ray

This is C code for a Wall-E robot, running on a Raspberry PI. He has a number of
servo motors that are controlled by a PCA9685 16 channel PWM controller via I2C
connected to the PI. He has 2 DC motors for the tracks, and they are controlled
by a L298 Motor Bridge connected to a Pro Mini Arduino board. The Raspberry
sends commands to the Arduino board to control the track motors. The board also
controls audio and battery monitoring. This was to off-load different functions
from the PI as much as possible, so the PI could focus on image processing (usb
camera) and other important operations.

This C code is compiled as a shared lib, and loaded from python.
*/

#include "robot.h"
#include "animate.h"

int addr = 0x40;
unsigned int setServo[_PCA9685_CHANS] = {0};
unsigned int offVals[_PCA9685_CHANS] = {0};

void setTime(Timer *time) { clock_gettime(CLOCK_MONOTONIC, &time->startTime); }

long getTime(Timer *time) {
  clock_gettime(CLOCK_MONOTONIC, &time->currentTime);
  return (time->currentTime.tv_sec - time->startTime.tv_sec) * 1000 +
         (time->currentTime.tv_nsec - time->startTime.tv_nsec) / 1000000;
}

void updateTime(Timer *time) { time->startTime = time->currentTime; }

int checkCommand() { return state; }

void setCommand(int d) {
  if (d == 1) {
    if (state != 1)
      state = 1;
  } else if (d == 0) {
    if (state != 0)
      state = 0;
  }
}

// Starting the serial connection
void robot_init() {
  wiringPiSetup();
  serial_port = serialOpen("/dev/ttyAMA0", 9600);
  sleep(1);
  serialFlush(serial_port);

  fd = initHardware(1, addr, 50);
  state = 0;
  animate = 0;
  sleep(1);

  for (int i = 0; i < 9; i++) {
    s[i].pos = (controls[i][1] + controls[i][2]) / 2;
    s[i].easing = EASING;
    s[i].min = controls[i][1];
    s[i].max = controls[i][2];
    s[i].targetPos = controls[i][0];
  }
  // s[R_].targetPos = U_NECK_DEF;
  // s[BOTTOM_NECK].targetPos = B_NECK_DEF;

  bGrid.y = 550;
  bGrid.x = 270;
  /*roi.b_Size = 15;
  roi.b_Step = 40;
  roi.x = 1;
  roi.y = 1;
  roi.w = CAM_WIDTH;
  roi.h = CAM_HEIGHT;*/

  // r_image.addr = malloc(CAM_WIDTH*CAM_HEIGHT);
  // r_image.width = CAM_WIDTH;
  // r_image.height = CAM_HEIGHT;
  initPOS();
}

void moveBody(Queue *queue, int command) {

  if (queueNotEmpty(queue)) {
    if (command) {
      checkQueue(queue);
    } else
      dequeueAll(queue);
  }
  updateBodyPos();

  if (track.state) {
    setTrackSpeed();
  }
}

int initHardware(int adpt, int addr, int freq) {
  int afd = PCA9685_openI2C(adpt, addr);
  PCA9685_initPWM(afd, addr, freq);
  return afd;
}

void delay(int mili) { usleep(mili * 1000); }

void updateTrackData(int left, int right) {
  int leftSpeed = 0;
  int rightSpeed = 0;

  if (left > 0)
    leftSpeed = mapRange(60, 250, 8, 15, left);
  else
    leftSpeed = mapRange(60, 250, 1, 7, abs(left));

  if (right > 0)
    rightSpeed = mapRange(60, 250, 8, 15, right);
  else
    rightSpeed = mapRange(60, 250, 1, 7, abs(right));

  sendData(leftSpeed, rightSpeed);
}

void checkQueue(Queue *queue) {
  if (!animate && queue->front) {
    if (queue->front->startDelay == 0) {
      for (int i = 0; i < 9; i++) {
        if (queue->front->servoId[i] != 0) {
          if (s[i].pos != queue->front->servoId[i]) {
            s[i].targetPos = queue->front->servoId[i];
          }
          s[i].easing = queue->front->servoSpeed;
        }
      }
      dequeue(queue, queue->front->key);
    } else
      queue->front->startDelay -= 1;
  }
}

void sendData(int v1, int v2) {
  unsigned char x = 0;
  x = (v1 << 4) | v2;
  serialPutchar(serial_port, x);
}

void closeSerial() { serialClose(serial_port); }

uint16_t pulseWidth(int channel, float degree) {
  if (degree < controls[channel][1])
    degree = controls[channel][1];
  else if (degree > controls[channel][2])
    degree = controls[channel][2];
  // controls[channel][2] = degree;
  return round(degree * (512 - 102) / 180 + 102);
}

void setServoAngle() {
  PCA9685_setPWMVals(fd, addr, offVals, setServo);
  setServo[_PCA9685_CHANS] = 0;
}

uint8_t sendCommand(int op1, int op2) {
  uint8_t response = 0;
  int timeoutCounter = 0;
  sendData(op1, op2);

  while (1) {
    if (serialDataAvail(serial_port)) {
      response = serialGetchar(serial_port);
      break;
    } else {
      usleep(200);
      timeoutCounter++;
      sendData(op1, op2);

      if (timeoutCounter > 200) {
        printf("Timeout: No response\n");
        break;
      }
    }
  }

  if (timeoutCounter <= 200) {
    return response;
  }

  return 0;
}

void initPOS() {
  uint8_t res = 0;
  res = sendCommand(0, 1);
  if (res != 0) {
    s[TOP_HEAD].pos = constrain(res, s[TOP_HEAD].min, s[TOP_HEAD].max);
    s[TOP_HEAD].targetPos = HEAD_DEF; // controls[0][0];
  }
}

void eyeCalibration(Queue *queue) {

  enqueueAnimation(queue, a0, rows(a0), 0.49);
}
//
void enqueueAnimation(Queue *queue, int arr[][10], int rows, float speed) {

  for (int i = 0; i < rows; i++) {
    int q[9];
    int delay = 0;
    for (int k = 0; k < 10; k++) {
      if (k < 9)
        q[k] = arr[i][k];
      else
        delay = arr[i][k];
    }
    enqueue(queue, q, speed, delay);
  }
}

void solveIK(int y, int d, double *theta1, double *theta2) {
  double r1 = 0.0;
  double phi1 = 0.0;
  double phi2 = 0.0;
  double phi3 = 0.0;

  r1 = sqrt((d * d) + (y * y));
  phi1 = acos(((A2 * A2) - (r1 * r1) - (A1 * A1)) / (-2.0 * r1 * A1));
  phi2 = atan2(y, d);
  *theta1 = phi2 - phi1;
  phi3 = acos(((r1 * r1) - (A1 * A1) - (A2 * A2)) / (-2.0 * A1 * A2));
  *theta2 = M_PI - phi3;
  *theta1 = RADTODEG(*theta1);
  *theta2 = RADTODEG(*theta2);
}

void updateHead(int x, int y, int d) {
  if (x != 0 && y != 0) {
    double theta1 = 0.0;
    double theta2 = 0.0;
    int y_coord = mapRange(0, 480, 1, 170, y);
    solveIK(y_coord, d, &theta1, &theta2);

    if (theta1 != theta1 || theta2 != theta2) {
      solveIK(y_coord, 100, &theta1, &theta2);
    }
    int t = mapRange(-20, 90, 30, 220, (int)theta1);
    int y = mapRange(180, 20, 25, 120, (int)theta2);
    s[BOTTOM_NECK].targetPos =
        constrain(t, s[BOTTOM_NECK].min, s[BOTTOM_NECK].max);
    s[UPPER_NECK].targetPos =
        constrain(y, s[UPPER_NECK].min, s[UPPER_NECK].max);

    s[TOP_HEAD].targetPos =
        mapRange(0, 640, s[TOP_HEAD].min, s[TOP_HEAD].max, x);
  }
}

void updatePos(Servo *position, float easedAmt) {
  if (easedAmt < 0.05)
    easedAmt = 0.05;
  else if (easedAmt > 0.50)
    easedAmt = 0.50;

  float posEased = sl(position->pos, position->targetPos, easeIn(easedAmt));

  if (position->pos < position->targetPos) {
    if (posEased >= position->targetPos)
      position->pos = position->targetPos;
    else
      position->pos = posEased;
  } else if (position->pos > position->targetPos) {
    if (posEased <= position->targetPos)
      position->pos = position->targetPos;
    else
      position->pos = posEased;
  }
}

int checkBodyPos(Servo *serv, int channel) {
  float diff = ABS_DIFF(serv[channel].pos, serv[channel].targetPos);
  if (diff > 0.35) {
    updatePos(&serv[channel], serv[channel].easing);
    if (channel == TOP_HEAD) {
      if (serv[UPPER_NECK].pos > 100 && serv[BOTTOM_NECK].pos > 90) {
        if (serv[TOP_HEAD].pos >= 130)
          setServo[0] = pulseWidth(TOP_HEAD, 130);
        if (serv[TOP_HEAD].pos <= 70)
          setServo[0] = pulseWidth(TOP_HEAD, 70);
      } else
        setServo[channel] = pulseWidth(TOP_HEAD, (int)serv[TOP_HEAD].pos);
    } else
      setServo[channel] = pulseWidth(channel, (int)serv[channel].pos);
    if (!animate)
      animate = 1;
    return 1;
  }
  return 0;
}

void updateBodyPos() {

  int cnt = 0;

  for (int i = 1; i < 9; i++) {
    cnt += checkBodyPos(s, i);
  }
  cnt += checkBodyPos(s, TOP_HEAD);

  if (cnt) {
    setServoAngle();
  } else
    resetState();
}

void updateTrackCoord(int x, int y) {
  if (track.state != 1)
    track.state = 1;

  int x_st = abs(x - CAM_WIDTH_F);
  int y_st = abs(y - CAM_HEIGHT_F); // CAM_HEIGHT - obj->y;

  track.left = LOW_SPEED;
  track.right = LOW_SPEED;
  int speed = mapRange(0, CAM_HEIGHT_F, 0, 190, y_st); // constrain(y, 0, 195);
  int turn = mapRange(0, CAM_WIDTH_F, 0, 190, x_st);   // constrain(x, 0, 195);
  track.left += speed;
  track.right += speed;
  if (x < CAM_WIDTH_F) {
    track.left = constrain(track.left + turn, 60, 250);
    track.right = constrain(track.right - turn, 60, 250);
  } else {
    track.right = constrain(track.right + turn, 60, 250);
    track.left = constrain(track.left - turn, 60, 250);
  }

  if (y < CAM_HEIGHT_F) {
    track.left *= -1;
    track.right *= -1;
  }
}

void setTrackSpeed() {
  if (track.state != 0)
    updateTrackData(track.left, track.right);

  if (track.left > LOW_SPEED && track.right > LOW_SPEED) {
    track.left -= 1;
    track.right -= 1;
  } else if (track.left < -LOW_SPEED && track.right < -LOW_SPEED) {
    track.left += 1;
    track.right += 1;
  } else
    track.state = 0;
}

void resetState() {
  if (animate) {
    setServo[_PCA9685_CHANS] = 0;
    setServoAngle();
    animate = 0;
    for (int i = 1; i < 9; i++) {
      if (s[i].easing != EASING)
        s[i].easing = EASING;
    }
  }
}

// void stopTracks() {
//     sendData(1, 0);
// }

Point convert_angle(int angle) {
  Point result;
  double radians = (angle * 3.14159) / 180.0;

  // result.x = mapRange(-CAM_WIDTH,CAM_WIDTH,1,CAM_WIDTH,CAM_WIDTH *
  // cos(radians));
  result.x =
      mapRange(CAM_WIDTH, -CAM_WIDTH, 1, CAM_WIDTH, CAM_WIDTH * cos(radians));
  result.y = mapRange(-CAM_HEIGHT, CAM_HEIGHT, 1, CAM_HEIGHT,
                      CAM_HEIGHT * sin(radians));

  return result;
}

int percent(int a, int b) {
  float result = ((float)abs((a - b)) / ((a + b) / 2.0)) * 100;
  return (int)result;
}

void toHSV(color *hsv, uint8_t *pixel) {
  int H;
  float cmax, cmin, delta, S, V;
  float r = GET_R(pixel) / 255.0;
  float g = GET_G(pixel) / 255.0;
  float b = GET_B(pixel) / 255.0;

  cmax = CMAX(r, g, b);
  cmin = CMIN(r, g, b);
  delta = cmax - cmin;

  // Hue
  if (delta == 0)
    H = 0;
  else {
    if (cmax == r)
      H = 60 * ((g - b) / delta);
    if (cmax == g)
      H = 60 * ((b - r) / delta + 2);
    if (cmax == b)
      H = 60 * ((r - g) / delta + 4);
  }
  // Saturation
  if (cmax == 0)
    S = 0;
  else
    S = (delta / cmax) * 100;
  // Value
  V = cmax * 100;
  if (H < 0)
    H = 300 + abs(H);
  hsv->H = H;
  hsv->S = S;
  hsv->V = V;
}

void process_Image(image *src) {
  check_Roi(&rect);

  grid_Roi(src, &rect);
}

void check_Roi(roi *rect) {
  if (rect->p == NULL) {
    // rect = (roi *)malloc(sizeof(roi));
    if (rect->b_Size == 0)
      rect->b_Size = 20;
    if (rect->b_Step == 0)
      rect->b_Step = 10;
    if (rect->w == 0)
      rect->w = CAM_WIDTH;
    if (rect->h == 0)
      rect->h = CAM_HEIGHT;

    int block_size = rect->b_Size + rect->b_Step;

    float grid_w = rect->w / block_size;
    int g_w = (int)grid_w;
    if (grid_w - g_w) {
      int p_x = rect->w - (g_w * (rect->b_Size + rect->b_Step));
      rect->x = (int)(p_x / 2);
    }

    float grid_h = rect->h / block_size;
    int g_h = (int)grid_h;
    if (grid_h - g_h) {
      int p_y = rect->h - (g_h * (rect->b_Size + rect->b_Step));
      rect->y = (int)(p_y / 2);
    }
    rect->num_Blocks = g_w * g_h;
    rect->p = (color *)malloc((rect->num_Blocks) * sizeof(color));
  }
  // check_Roi(src);
}

void grid_Roi(image *src, roi *rect) {
  color pix;
  int step = rect->b_Size + rect->b_Step;
  int pixels = rect->b_Size * rect->b_Size;
  // int t_blocks = rect->num_Blocks;
  int t = 0;
  if (rect->step < STEP_COUNT)
    rect->step += 1;
  else
    rect->step = 0;

  for (int y = rect->y; y < rect->h; y += step) {
    for (int x = rect->x; x < rect->w; x += step) {
      int h_sum, s_sum, v_sum = 0;

      for (int ry = y, yy = y + rect->b_Size; ry < yy; ry++) {
        // uint16_t *p = PIXEL_ROW(src, ry);
        for (int rx = x, xx = x + rect->b_Size; rx < xx; rx++) {
          uint8_t *pixel = GET_PIXEL(src, rx, ry);
          toHSV(&pix, pixel);

          h_sum += pix.H;
          s_sum += pix.S;
          v_sum += pix.V;
          // if(rect->step > 0 && rect->step <= STEP_COUNT)
          // {
          // if(rect->p[t].diff == 1)
          //
          if (rect->p[t].diff == 1) // if(pix.H < 15 && pix.S > 70 && pix.V >
                                    // 70)//if(GET_R(pixel) < 100)
            PUT_PIXEL(src, rx, ry, RGB(100, 250, 30));
          // }
        }
      }
      // if(rect->step > 0)
      //  {
      int h = h_sum / pixels;
      int s = s_sum / pixels;
      int v = v_sum / pixels;
      if (percent(rect->p[t].V, v) > 70)
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
