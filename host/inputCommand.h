//控制的头文件
#include <wiringPi.h>
#include <stdio.h>

struct InputCommander
{
    char commandName[128]; // socket名
    char deviceName[128];  //串口名
    char comand[32];       //控制命令

    int (*Init)(struct InputCommander *voicer, char *ipAdress, char *port); // socket初始化
    int (*getCommand)(struct InputCommander *voicer);                       //读取数据

    char log[1024];
    int fd;
    char port[12];     //端口号
    char ipAdress[32]; // IP地址
    int sfd;
    int cfd;

    struct InputCommander *next;
};

//每个控制添加到控制链表的函数声明
struct InputCommander *addVoiceContrlToInputCommanderLink(struct InputCommander *phead);
struct InputCommander *addsocketContrlToInputCommanderLink(struct InputCommander *phead);
struct InputCommander *addclientContrlToInputCommanderLink(struct InputCommander *phead);