//蜂鸣器
#include "contrlDevices.h"

int buzzerOpen(int pinNum)
{
    digitalWrite(pinNum, LOW);
}

int buzzerClose(int pinNum)
{
    digitalWrite(pinNum, HIGH);
}

int buzzerInit(int pinNum)
{
    pinMode(pinNum, OUTPUT);
    digitalWrite(pinNum, HIGH);
    printf("蜂鸣器初始化成功\n");
}

struct Devices buzzer = {
    .deviceName = "buzzser",
    .pinNum = 7,
    .open = buzzerOpen,
    .close = buzzerClose,
    .deviceInit = buzzerInit};

struct Devices *addBuzzerToDeviceLink(struct Devices *phead)
{
    if (phead == NULL)
    {
        return &buzzer;
    }
    else
    {
        buzzer.next = phead;
        phead = &buzzer;
    }
    return phead;
}
