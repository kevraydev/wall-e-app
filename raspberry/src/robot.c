#include "robot.h"

int addr = 0x40;

//Default POS - Min POS - Max POS
int controls[9][3] = {
  {90, 10, 175},//Head
  {90, 25, 170},//Neck
  {95, 50, 95},//Left Eye
  {95, 65, 95},//Right Eye
  {180, 1, 180},//Left Track
  {120, 1, 180},//Left Arm
  {1, 1, 180},//Right Track
  {55, 1, 180},//Right Arm
  {130, 20, 170},//Bottom Neck
};



void robot_init()
{
    wiringPiSetup();
    serial_port = serialOpen("/dev/ttyAMA0", 9600);
    sleep(1);

    fd = initHardware(1, addr, 50);

    // initPOS();

    sleep(2);

    for(int i = 0; i < 9; i++)
    {
        //servos[i].defPos = controls[i][0];
        servos[i].pos = controls[i][0];
        servos[i].min = controls[i][1];
        servos[i].max = controls[i][2];
        servos[i].p = 0.000001;
        servos[i].d = 0.000001;
    }
    

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
    if (degree < servos[channel].min)
        degree = servos[channel].min;
    else if (degree > servos[channel].max)
        degree = servos[channel].max;
    //controls[channel][2] = degree;
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

void updateHead(int pan, int tilt)
{
  /*  if (pan != 0)
        updatePD(&posX, pan - 160);
    if (tilt != 0)
        updatePD(&posY, 120 - tilt);
    servo_set(posX.pos, posY.pos);*/
}

Item *newItem(int key, int value)
{
    Item *item = (Item*)malloc(sizeof(Item));
    if(item != NULL)
    {
        item->key = key;
        item->value = value;
        item->prevError = 0;
        item->next = NULL;
    }
    return item;
}


Queue *initQueue()
{
    Queue *queue = malloc(sizeof(Queue));
    if(queue != NULL)
    {
        queue->front = NULL;
        queue->rear = NULL;
    }
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

void updateControl(Item *current)
{
    int i = current->key;
    int error = current->value - servos[i].pos;
    if (current->prevError != 0)
    {
        servos[i].pos += (error * servos[i].p + (error - current->prevError) * servos[i].d);

        if (servos[i].pos > servos[i].max)
        {
            servos[i].pos = servos[i].max;
            //current->value = servos[i].pos;
        }
        else if (servos[i].pos < servos[i].min)
        {
            servos[i].pos = servos[i].min;
            //current->value = servos[i].pos;
        }
        //int currentError = current->value - servos[i].pos;

        //if (error < 0 && currentError > 0)
        //    current->value = servos[i].pos;
        //else if (error > 0 && currentError < 0)
        //    current->value = servos[i].pos;
            
    }
    current->prevError = error;

    setServoAngle(i, servos[i].pos);
}

void iterateServos(Queue *queue) 
{
    Item *current = queue->front;
    Item *prev = NULL;

    while (current != NULL)
    {
        if (servos[current->key].pos != current->value)
        {
            updateControl(current);
            prev = current;
            current = current->next;
        } else {
            if (prev == NULL)
            {
                queue->front = current->next;
                if (queue->front == NULL) queue->rear = NULL;
            } else {
                prev->next = current->next;
                if (current == queue->rear) queue->rear = prev;
            }

            Item *tmp = current;
            current = current->next;
            free(tmp);
        }
    }
}


