//门锁
#include "contrlDevices.h"

int doorLockOpen(int pinNum)
{
    digitalWrite(pinNum, LOW);
}

int doorLockClose(int pinNum)
{
    digitalWrite(pinNum, HIGH);
}

int doorLockCloseInit(int pinNum)
{
    pinMode(pinNum, OUTPUT);
    digitalWrite(pinNum, HIGH);
   // printf("门锁初始化成功\n");
}

int doorLockCloseStatus(int status)
{
}

struct Devices doorLock = {
    //.deviceName = "doorLock",
    .deviceName = "lock",
    .pinNum = 29,
    .open = doorLockOpen,
    .close = doorLockClose,
    .deviceInit = doorLockCloseInit,
    .changStatus = doorLockCloseStatus};

struct Devices *addDoorLockToDeviceLink(struct Devices *phead)
{
    if (phead == NULL)
    {
        return &doorLock;
    }
    else
    {
        doorLock.next = phead;
        phead = &doorLock;
    }
    return phead;
}
