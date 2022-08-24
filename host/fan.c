//电风扇
#include "contrlDevices.h"

int fanOpen(int pinNum)
{
    // pinMode(pinNum, OUTPUT);
    digitalWrite(pinNum, LOW);
}

int fanClose(int pinNum)
{
    // pinMode(pinNum, INPUT);
    digitalWrite(pinNum, HIGH);
}

int fanCloseInit(int pinNum)
{
    //  pinMode(pinNum, INPUT);
    pinMode(pinNum, OUTPUT);
    digitalWrite(pinNum, HIGH);
    //printf("电风扇初始化成功\n");
}

int fanCloseStatus(int status)
{
}

struct Devices fan = {
    //.deviceName = "fan",
    .deviceName = "fan",
    .pinNum = 26,
    .open = fanOpen,
    .close = fanClose,
    .deviceInit = fanCloseInit,
    .changStatus = fanCloseStatus};

struct Devices *addFanToDeviceLink(struct Devices *phead)
{
    if (phead == NULL)
    {
        return &fan;
    }
    else
    {
        fan.next = phead;
        phead = &fan;
    }
    return phead;
}
