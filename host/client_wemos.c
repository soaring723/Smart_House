//树莓派作为客服端  连接wemos D1服务器
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringSerial.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "inputCommand.h"

int clientInit(struct InputCommander *client, char *ipAdress, char *port)
{
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(struct sockaddr_in));

    // 1.socket 创建套接字
    int s_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (s_fd == -1)
    {
        perror("socket");
        exit(-1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(client->port));
    inet_aton(client->ipAdress, &addr.sin_addr);

    // 2.connect 连接服务器
    if (connect(s_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1) //这里是连接wemosD1
    {
        perror("connect");
        exit(-1);
    }
    printf("客户端 wemosD1 连接....\n");

    client->sfd = s_fd;
    return s_fd;
}

struct InputCommander clientContrl = {
    .commandName = "client",     //加入到控制链表的名字
    .comand = {'\0'},            //命令
    .port = "8888",              // wemosD1 端口号
    .ipAdress = "192.168.1.103", // wemosD1 IP地址
    .Init = clientInit,          //树莓派客户端初始化
    .next = NULL};

struct InputCommander *addclientContrlToInputCommanderLink(struct InputCommander *phead)
{
    if (phead == NULL)
    {
        return &clientContrl;
    }
    else
    {
        clientContrl.next = phead;
        phead = &clientContrl;
    }
    return phead;
}
