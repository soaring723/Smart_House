//餐厅灯
#include "contrlDevices.h"

int restaurantLightOpen(int pinNum)
{
    digitalWrite(pinNum, LOW);
}

int restaurantLightClose(int pinNum)
{
    digitalWrite(pinNum, HIGH);
}

int restaurantLightCloseInit(int pinNum)
{
    pinMode(pinNum, OUTPUT);
    digitalWrite(pinNum, HIGH);
  //  printf("餐厅灯初始化成功\n");
}

int restaurantLightCloseStatus(int status)
{
}

struct Devices restaurantLight = {
    //.deviceName = "restaurantLight",
    .deviceName = "chu",
    .pinNum = 23,
    .open = restaurantLightOpen,
    .close = restaurantLightClose,
    .deviceInit = restaurantLightCloseInit,
    .changStatus = restaurantLightCloseStatus};

struct Devices *addrestaurantLightToDeviceLink(struct Devices *phead)
{
    if (phead == NULL)
    {
        return &restaurantLight;
    }
    else
    {
        restaurantLight.next = phead;
        phead = &restaurantLight;
    }
    return phead;
}
