//火焰传感器
#include "contrlDevices.h"

int fireInit(int pinNum)
{
    pinMode(pinNum, INPUT);
    digitalWrite(pinNum, HIGH);
}

int readFireStatus(int pinNum)
{
    return digitalRead(pinNum);
}

struct Devices fire = {
    .deviceName = "fire",
    .pinNum = 25,
    .deviceInit = fireInit,
    .readStatus = readFireStatus};

struct Devices *addFireToDeviceLink(struct Devices *phead)
{
    if (phead == NULL)
    {
        return &fire;
    }
    else
    {
        fire.next = phead;
        phead = &fire;
    }

    return phead;
}
