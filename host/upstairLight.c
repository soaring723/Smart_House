//客厅灯
#include <stdlib.h>
#include <fcntl.h>
#include "contrlDevices.h"

int upstairLightOpen(int pinNum) //打开客厅灯函数
{
    digitalWrite(pinNum, LOW);
}

int upstairLightClose(int pinNum) //关闭客厅灯函数
{
    digitalWrite(pinNum, HIGH);
}

int upstairLightCloseInit(int pinNum) //初始化函数
{
    pinMode(pinNum, OUTPUT);    //配置引脚为输出引脚
    digitalWrite(pinNum, HIGH); //引脚输出高电平，即默认为关闭状态
   // printf("客厅灯初始化成功\n");
}

int upstairLightCloseStatus(int status)
{
}

struct Devices upstairLight = { //客厅灯设备链表节点
    .deviceName = "kt",
    .pinNum = 22,
    .open = upstairLightOpen,
    .close = upstairLightClose,
    .deviceInit = upstairLightCloseInit,
    .changStatus = upstairLightCloseStatus

};

struct Devices *addupStairLightToDeviceLink(struct Devices *phead) //头插法将设备节点加入设备工厂链表函数
{
    if (phead == NULL)
    {
        return &upstairLight;
    }
    else
    {
        upstairLight.next = phead;
        phead = &upstairLight;
    }
}
