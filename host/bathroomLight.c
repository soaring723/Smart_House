//浴室灯
#include "contrlDevices.h"

int bathroomLightOpen(int pinNum)
{
    digitalWrite(pinNum, LOW);
}

int bathroomLightClose(int pinNum)
{
    digitalWrite(pinNum, HIGH);
}

int bathroomLightCloseInit(int pinNum)
{
    pinMode(pinNum, OUTPUT);
    digitalWrite(pinNum, HIGH);
  //  printf("浴室灯初始化成功\n");
}

int bathroomLightCloseStatus(int status)
{
}

struct Devices bathroomLight = {
    //.deviceName = "bathroomLight",
    .deviceName = "yu",
    .pinNum = 24,
    .open = bathroomLightOpen,
    .close = bathroomLightClose,
    .deviceInit = bathroomLightCloseInit,
    .changStatus = bathroomLightCloseStatus};

struct Devices *addBathroomLightToDeviceLink(struct Devices *phead)
{
    if (phead == NULL)
    {
        return &bathroomLight;
    }
    else
    {
        bathroomLight.next = phead;
        phead = &bathroomLight;
    }
    return phead;
}
