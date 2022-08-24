//卧室灯
#include "contrlDevices.h"

int livingroomLightOpen(int pinNum)
{
    digitalWrite(pinNum, LOW);
}

int livingroomLightClose(int pinNum)
{
    digitalWrite(pinNum, HIGH);
}

int livingroomLightCloseInit(int pinNum)
{
    pinMode(pinNum, OUTPUT);
    digitalWrite(pinNum, HIGH);
  //  printf("卧室灯初始化成功\n");
}

int livingroomLightCloseStatus(int status)
{
}

struct Devices livingroomLight = {
    //.deviceName = "livingroomLight",
    .deviceName = "shui",
    .pinNum = 21,
    .open = livingroomLightOpen,
    .close = livingroomLightClose,
    .deviceInit = livingroomLightCloseInit,
    .changStatus = livingroomLightCloseStatus};

struct Devices *addlivingroomLightToDeviceLink(struct Devices *phead)
{
    if (phead == NULL)
    {
        return &livingroomLight;
    }
    else
    {
        livingroomLight.next = phead;
        phead = &livingroomLight;
    }
    return phead;
}
