//人体红外检测
#include "contrlDevices.h"

int humanInit(int pinNum)
{
    pinMode(pinNum, INPUT);
    digitalWrite(pinNum, HIGH);
   // printf("人体感应传感器初始化成功\n");
}

int readhumanStatus(int pinNum)
{
    return digitalRead(pinNum);
}

struct Devices human = {
    .deviceName = "human",
    .pinNum = 5,
    .deviceInit = humanInit,
    .readStatus = readhumanStatus};

struct Devices *addHumanToDeviceLink(struct Devices *phead)
{
    if (phead == NULL)
    {
        return &human;
    }
    else
    {
        human.next = phead;
        phead = &human;
    }
    return phead;
}
