//泳池灯
#include "contrlDevices.h"

int poolLightOpen(int pinNum)
{
    digitalWrite(pinNum, LOW);
}

int poolLightClose(int pinNum)
{
    digitalWrite(pinNum, HIGH);
}

int poolLightCloseInit(int pinNum)
{
    pinMode(pinNum, OUTPUT);
    digitalWrite(pinNum, HIGH);
   // printf("泳池灯初始化成功\n");
}

int poolLightCloseStatus(int status)
{
}

struct Devices poolLight = {
    //.deviceName = "poolLight",
    .deviceName = "yong",
    .pinNum = 27,
    .open = poolLightOpen,
    .close = poolLightClose,
    .deviceInit = poolLightCloseInit,
    .changStatus = poolLightCloseStatus};

struct Devices *addpoolLightToDeviceLink(struct Devices *phead)
{
    if (phead == NULL)
    {
        return &poolLight;
    }
    else
    {
        poolLight.next = phead;
        phead = &poolLight;
    }
    return phead;
}
