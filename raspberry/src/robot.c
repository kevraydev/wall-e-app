#include "robot.h"

void robot_init()
{
    wiringPiSetup();
    serial_port = serialOpen("/dev/ttyAMA0", 9600);
    sleep(1);

    fd = initHardware(1, addr, 50);

    // initPOS();

    sleep(2);
    set_PD();
}

void set_PD()
{
    /*  posX.pos = SERVO_DUTY_CENTER;
      posX.p = 0.06;
      posX.d = 0.008;
      posX.prevError = 0;
      posX.minPOS = PAN_DUTY_MIN;
      posX.maxPOS = PAN_DUTY_MAX;

      posY.pos = SERVO_DUTY_CENTER_TILT;
      posY.p = 0.06;
      posY.d = 0.008;
      posY.prevError = 0;
      posY.minPOS = TILT_DUTY_MIN;
      posY.maxPOS = TILT_DUTY_MAX;*/
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

void sendData(int angle, int distance, int ctrlId)
{
    char strData[14];
    sprintf(strData, "%03d %02d", angle, ctrlId);
    // sprintf(strData, "%03d %02d %02d", angle, distance, ctrlId);

    serialPuts(serial_port, strData);
}

void closeSerial()
{
    serialClose(serial_port);
}

void setServoAngle(int channel, float degree)
{
    if (degree < controls[channel][3])
        degree = controls[channel][3];
    else if (degree > controls[channel][4])
        degree = controls[channel][4];
    controls[channel][2] = degree;
    uint16_t pulseWidth = degree * (512 - 102) / 180 + 102;

    unsigned int onVals[_PCA9685_CHANS] = {0};
    unsigned int offVals[_PCA9685_CHANS] = {0};

    offVals[channel] = round(pulseWidth);

    PCA9685_setPWMVals(fd, addr, onVals, offVals);
}

void initPOS()
{
    for (int i = 0; i <= 8; i++)
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
    if (controls[3][2] != controls[3][4])
        setServoAngle(3, controls[3][4]);
    if (controls[2][2] != controls[2][4])
        setServoAngle(2, controls[2][4]);

    if (controls[0][2] != 90)
    {
        setServoAngle(0, 90);
        delay(1000);
    }

    setServoAngle(2, controls[2][3]); // Left down
    delay(delayAmt);
    setServoAngle(3, controls[3][3]); // Right down
    delay(delayAmt);
    setServoAngle(2, controls[2][4]); // Left up
    delay(delayAmt);
    setServoAngle(3, controls[3][4]); // Down up
    delay(delayAmt);
    setServoAngle(3, controls[3][3]); // Left and Right Down
    setServoAngle(2, controls[2][3]);
    delay(delayAmt + 200);
    setServoAngle(3, controls[3][4]); // Left and Right Up
    setServoAngle(2, controls[2][4]);
}

void updatePD(PD *newPos, int error)
{
    if (newPos->prevError != 0)
    {
        newPos->pos += (error * newPos->p + (error - newPos->prevError) * newPos->d);

        if (newPos->pos > newPos->maxPOS)
            newPos->pos = newPos->maxPOS;
        else if (newPos->pos < newPos->minPOS)
            newPos->pos = newPos->minPOS;
    }
    newPos->prevError = error;
}

void updateHead(int pan, int tilt)
{
    if (pan != 0)
        updatePD(&posX, pan - 160);
    if (tilt != 0)
        updatePD(&posY, 120 - tilt);
    servo_set(posX.pos, posY.pos);
}

Item *newItem(int key, int value)
{
    Item *item = malloc(sizeof(Item));
    item->key = key;
    item->value = value;
    return item;
}

Queue *initQueue()
{
    Queue *queue = malloc(sizeof(Queue));
    return queue;
}

void enqueue(Queue *queue, int key, int value)
{
    Item *item = newItem(key, value);
    if (queue->rear)
    {
        queue->rear->next = item;
        queue->rear = item;
    }
    else
        queue->front = queue->rear = item;
}

int dequeue(Queue *queue, int key)
{
    Item *temp = queue->front;
    Item *prevItem = NULL;

    if (temp == NULL)
        return 0;

    while (temp != NULL && temp->key != key)
    {
        prevItem = temp;
        temp = temp->next;
    }

    prevItem->next = temp->next;

    if (queue->rear == temp)
        queue->rear = prevItem;

    free(temp);
    return 0;
}

void dequeueAll(Queue *queue)
{
    while (queue->front)
    {
        Item *item = queue->front;
        queue->front = queue->front->next;
        free(item);
    }
    queue->rear = NULL;
}

int queueNotEmpty(Queue *queue)
{
    if (queue->front)
        return 1;
    else
        return 0;
}

int getCount(Queue *queue)
{
    int count = 0;
    Item *temp = queue->front;
    while (temp)
    {
        count += 1;
        temp = temp->next;
    }
    return count;
}

int getByKey(Queue *queue, int key)
{
    Item *temp = queue->front;
    while (temp)
    {
        if (temp->key == key)
        {
            return temp->value;
        }
        temp = temp->next;
    }
    return -1;
}

int updateKey(Queue *queue, int key, int value)
{
    Item *temp = queue->front;

    if (temp == NULL)
        return -1;

    while (temp != NULL)
    {
        if (temp->key == key)
        {
            temp->value = value;
            return 0;
        }
        temp = temp->next;
    }
    return -1;
}

void iterateServos(Queue *queue)
{
    Item *temp = queue->front;
    Item *prevItem = NULL;

    while (temp != NULL)
    {
        int i = temp->key;

        if (controls[i][2] != temp->value)
        {
            prevItem = temp;

            if (controls[i][2] < temp->value)
            {
                controls[i][2] += 1;
                if (controls[i][2] > controls[i][4])
                    controls[i][2] = controls[i][4];
            }
            else
            {
                controls[i][2] -= 1;
                if (controls[i][2] < controls[i][3])
                    controls[i][2] = controls[i][3];
            }
            setServoAngle(i, controls[i][2]);

        }
        else
        {
            prevItem->next = temp->next;
            if (queue->rear == temp)
                queue->rear = prevItem;
            free(temp);
        }

        temp = temp->next;
    }

    
}
