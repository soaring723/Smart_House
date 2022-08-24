//震动传感器
#include "contrlDevices.h"

int shakeInit(int pinNum)
{
    pinMode(pinNum, INPUT);
    digitalWrite(pinNum, HIGH);
}

int readShakeStatus(int pinNum)
{
    return digitalRead(pinNum);
}

struct Devices shake = {
    .deviceName = "shake",
    .pinNum = 1,
    .deviceInit = shakeInit,
    .readStatus = readShakeStatus};

struct Devices *addShakeToDeviceLink(struct Devices *phead)
{
    if (phead == NULL)
    {
        return &shake;
    }
    else
    {
        shake.next = phead;
        phead = &shake;
    }
    return phead;
}
