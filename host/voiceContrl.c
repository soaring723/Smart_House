//语音控制设备
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringSerial.h>
#include <unistd.h>
#include <string.h>

#include "inputCommand.h"

int getCommand(struct InputCommander *voicer) //获取语音指令
{
    int nread = 0;
    memset(voicer->comand, '\0', sizeof(voicer->comand));
    nread = read(voicer->fd, voicer->comand, sizeof(voicer->comand));
    return nread;
}
//语音模块初始化
int voiceInit(struct InputCommander *voicer, char *ipAdress, char *port)
{
    int fd;
    if ((fd = serialOpen(voicer->deviceName, 9600)) == -1)
    {
        printf("语音初始化失败\n");
        exit(-1);
    }
    voicer->fd = fd;
    printf("语音初始化结束\n");
    return fd;
}
//语音模块结构体
struct InputCommander voiceContrl = {
    .commandName = "voice",
    .deviceName = "/dev/ttyAMA0",
    .comand = {'\0'},
    .Init = voiceInit,
    .getCommand = getCommand,
    .log = {'\0'},
    .next = NULL};

struct InputCommander *addVoiceContrlToInputCommanderLink(struct InputCommander *phead)
{
    if (phead == NULL)
    {
        return &voiceContrl;
    }
    else
    {
        voiceContrl.next = phead;
        phead = &voiceContrl;
    }
    return phead;
}
