// socket控制
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

int socketgetCommand(struct InputCommander *socketMes)
{
    int c_fd;
    int n_read;

    struct sockaddr_in c_addr;
    memset(&c_addr, 0, sizeof(struct sockaddr_in));
    int clen = sizeof(struct sockaddr_in);

    // 4.accept
    c_fd = accept(socketMes->sfd, (struct sockaddr *)&c_addr, &clen);

    n_read = read(c_fd, socketMes->comand, sizeof(socketMes->comand));
    if (n_read == -1)
    {
        perror("读");
    }
    else if (n_read > 0)
    {
        printf("\n获取:%d\n", n_read);
    }
    else
    {
        printf("客户端退出\n");
    }

    return n_read;
}

int socketInit(struct InputCommander *socketMes, char *ipAdress, char *port)
{
    /*形参虽然定多了，用不上，咱不管*/

    int s_fd;
    struct sockaddr_in s_addr;

    memset(&s_addr, 0, sizeof(struct sockaddr_in));

    // 1.socket
    s_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (s_fd == -1)
    {
        perror("socket");
        exit(-1);
    }

    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(atoi(socketMes->port));
    inet_aton(socketMes->ipAdress, &s_addr.sin_addr);

    //解决服务器程序结束后端口被占用的情况
    int opt = 1;
    setsockopt(s_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 2.bind
    bind(s_fd, (struct sockaddr *)&s_addr, sizeof(struct sockaddr_in));

    // 3.listen
    listen(s_fd, 10);
    printf("套接字服务器侦听......\n");
    socketMes->sfd = s_fd;

    return s_fd;
}

struct InputCommander socketContrl = {
    .commandName = "socketServer",
    .comand = {'\0'},
    .port = "8083",
    .ipAdress = "172.26.183.9",
    //  .ipAdress = "192.168.245.9",
    .Init = socketInit,
    .getCommand = socketgetCommand,
    .log = {'\0'},
    .next = NULL};

struct InputCommander *addsocketContrlToInputCommanderLink(struct InputCommander *phead)
{
    if (phead == NULL)
    {

        return &socketContrl;
    }
    else
    {
        socketContrl.next = phead;
        phead = &socketContrl;
    }

    return phead;
}
