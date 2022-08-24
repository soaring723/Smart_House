#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "contrlDevices.h"
#include "inputCommand.h"
#define HIGH_TIME 32

// 433m端口
#define D0 4
#define D1 3
#define D2 2
#define D3 0

#define key 28 //拍照按键
// #define door_Lock 29 //门锁

//注意：定义线程不使用指针以免空指针异常
//注意：不建议线程传参（链表头）所以定为全局变量
pthread_t voiceThread;       //语音识别线程
pthread_t socketThread;      //服务器线程
pthread_t writeThread;       //通知线程：向客服端发送消息
pthread_t fireThread;        //火灾检测线程
pthread_t shakeThread;       //震动检测线程
pthread_t humanThread;       //人体红外检测线程
pthread_t remoteThread;      //遥控线程
pthread_t keyThread;         //按键检测线程
pthread_t cameraThread;      //人脸识别线程
pthread_t monitoringThread;  //视频监控线程
pthread_t clientWemosThread; //客服端线程
pthread_t dht11Thread;       //温湿度线程

struct InputCommander *pCommandHead = NULL;
struct Devices *pdeviceHead = NULL;
struct InputCommander *socketHandler = NULL;
// struct InputCommander *clientHandler = NULL;

pthread_mutex_t mutex; //定义互斥量（锁）
// pthread_cond_t  cond;   //条件

unsigned long databuf;

int c_fd; //注意：涉及到多线程不要轻易的去传参

int beep = 0; //蜂鸣器标志位   0：未被使用   1：被火灾传感器使用    2：被震动传感器使用
//摄像头相关，改变返回值命名，因为C语言中没有这样的返回值
#define true 1
#define false 0
typedef unsigned int bool;
char buf[1024] = {'\0'};

/*通知app [0]:温度   [1]:湿度   [2]:火灾监控   [3]:震动监控   [4]:人体红外监控*/
char Message[5][100] = {"未启用！", "未启用！", "未启用！", "未启用！", "未启用！"};
int write_flag = 0; //标记位，标记通知线程是否打开     1：打开   0：关闭

int human_flag = 0; //红外检测标志位     1：打开   0：关闭

struct Devices *findDeviceByName(char *name, struct Devices *phead) //查询设备
{
    if (phead == NULL)
    {
        return NULL;
    }

    while (phead != NULL)
    {
        // printf("phead:%s  name:%s\n", phead->deviceName, name);
        if (strstr(phead->deviceName, name) != NULL)
        {
            return phead;
        }
        phead = phead->next;
    }

    return NULL;
}

struct InputCommander *findCommandByName(char *name, struct InputCommander *phead) //查询控制
{
    if (phead == NULL)
    {
        return NULL;
    }
    while (phead != NULL)
    {
        if (strcmp(phead->commandName, name) == 0)
        {
            return phead;
        }
        phead = phead->next;
    }
    return NULL;
}

//查找设备
void *cameraThread_func(void *data) //起线程的函数有格式要求
{
    struct Devices *cameraTemp;

    cameraTemp = findDeviceByName("camera", pdeviceHead); //设备都要从工厂里面取出来

    if (cameraTemp == NULL)
    { //防止段错误的必需判断，当给指针赋值是，一定要考虑NULL的情况，否则后续操作都是空谈
        printf("find camera error\n");
        pthread_exit(NULL); //在线程中不用return
    }

    cameraTemp->justDoOnce(); //调用postUrl函数
}

unsigned char readSensorData(void) //温湿度初始化
{
    char crc;
    char i;

    pinMode(6, OUTPUT);   // 将模式设置为输出
    digitalWrite(6, LOW); // 输出高电平
    delay(25);
    digitalWrite(6, HIGH); // 输出低电平
    pinMode(6, INPUT);     // 将模式设置为输入
    pullUpDnControl(6, PUD_UP);

    delayMicroseconds(27);
    if (digitalRead(6) == 0) // SENSOR ANS
    {
        while (!digitalRead(6))
            ; // wait to high

        for (i = 0; i < 32; i++)
        {
            while (digitalRead(6))
                ; // 数据时钟启动
            while (!digitalRead(6))
                ; //数据开始
            delayMicroseconds(HIGH_TIME);
            databuf *= 2;
            if (digitalRead(6) == 1) // 1
            {
                databuf++;
            }
        }

        for (i = 0; i < 8; i++)
        {
            while (digitalRead(6))
                ; // 数据时钟启动
            while (!digitalRead(6))
                ; // 数据开始
            delayMicroseconds(HIGH_TIME);
            crc *= 2;
            if (digitalRead(6) == 1) // 1
            {
                crc++;
            }
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

void *dht11_thread(void *datas) //温湿度线程
{
    int W = 0, w = 0;
    int S = 0, s = 0;
    // int count = 0;
    int temp = 0;
    printf("Use GPIO4 to read data!\n");

    if (-1 == wiringPiSetup())
    {
        printf("Setup wiringPi failed!");
        // return 1;
    }

    pinMode(6, OUTPUT);    // set mode to output
    digitalWrite(6, HIGH); // output a high level

    printf("Enter OS-------\n");
    while (1)
    {
        pinMode(6, OUTPUT);    // set mode to output
        digitalWrite(6, HIGH); // output a high level
        delay(3000);

        if (readSensorData())
        {

            W = (databuf >> 8) & 0xff;
            w = databuf & 0xff;
            S = (databuf >> 24) & 0xff;
            s = (databuf >> 16) & 0xff;
            // printf("传感器数据读取正常!\n");
            if (temp == 0)
            {
                w++;
                s++;
                temp++;
            }
            else if (temp == 1)
            {
                if (w >= 1)
                {
                    w--;
                }
                if (s >= 1)
                {
                    w--;
                }

                temp--;
            }

            if ((W >= 15) && (W <= 35) && (S <= 95))
            {
                memset(Message[0], 0, sizeof Message[0]); //清空数组
                memset(Message[1], 0, sizeof Message[1]);
                // printf("温度：%d.%d    湿度：%d.%d\n", W, w, S, s); //清空数组
                sprintf(Message[0], "%d.%d C", W, w); //温度
                sprintf(Message[1], "%d.%d", S, s);   //湿度
                                                      //  printf("温度：%d.%d C", (databuf >> 8) & 0xff, databuf & 0xff);               //温度
            }                                         //   printf("   湿度：%d.%d %\n", (databuf >> 24) & 0xff, (databuf >> 16) & 0xff); //湿度

            databuf = 0;
        }
        else
        {
            //  count++;
            //  memset(Message[0], 0, sizeof Message[0]); //清空数组
            //  memset(Message[1], 0, sizeof Message[1]); //清空数组
            // sprintf(Message[0], "监测错误！");        //温度
            //  sprintf(Message[1], "监测错误！");        //湿度
            printf("Sorry! Sensor dosent ans!\n");
            databuf = 0;
        }

        // if (count >= 3)
        // {
        //     pthread_create(&dht11Thread, NULL, dht11_thread, NULL);
        // }
    }
}

void *fire_thread(void *datas) //火灾线程
{
    char msg[100];
    int status;
    struct Devices *fireDeviceTmp = NULL;
    struct Devices *buzzerDeviceTmp = NULL;

    fireDeviceTmp = findDeviceByName("fire", pdeviceHead); //在设备工厂找到火灾模块
    buzzerDeviceTmp = findDeviceByName("buzzser", pdeviceHead);

    fireDeviceTmp->deviceInit(fireDeviceTmp->pinNum); //火灾模块初始化
    buzzerDeviceTmp->deviceInit(buzzerDeviceTmp->pinNum);
    printf("火灾线程初始化成功\n");

    while (1)
    {
        delay(10);
        status = fireDeviceTmp->readStatus(fireDeviceTmp->pinNum); //读取火灾模块实时数据
        if (status == 0)
        {
            beep = 1;                                       //火灾传感器使用蜂鸣器
            buzzerDeviceTmp->open(buzzerDeviceTmp->pinNum); //蜂鸣器报警
            memset(Message[2], 0, sizeof Message[2]);       //清空数组
            sprintf(Message[2], "警报：发生火灾 ！");       //更新火灾信息
            delay(200);                                     //蜂鸣器报警延时
        }
        else if ((beep != 2) && (beep != 3)) //未被震动传感器和人体红外传感器使用
        {
            buzzerDeviceTmp->close(buzzerDeviceTmp->pinNum); //关闭蜂鸣器
            memset(Message[2], 0, sizeof Message[2]);        //清空数组
            sprintf(Message[2], "正常");                     //更新火灾信息
            beep = 0;                                        //蜂鸣器未被使用
        }
    }
}

void *shake_thread(void *datas) //震动线程
{
    char msg[100];
    int status;
    struct Devices *shakeDeviceTmp = NULL;
    struct Devices *buzzerDeviceTmp = NULL;

    shakeDeviceTmp = findDeviceByName("shake", pdeviceHead); //在设备工厂找到火灾模块
    buzzerDeviceTmp = findDeviceByName("buzzser", pdeviceHead);

    shakeDeviceTmp->deviceInit(shakeDeviceTmp->pinNum); //震动模块初始化
    buzzerDeviceTmp->deviceInit(buzzerDeviceTmp->pinNum);
    printf("震动线程初始化成功\n");
    while (1)
    {
        delay(5);
        status = shakeDeviceTmp->readStatus(shakeDeviceTmp->pinNum); //读取震动模块实时数据

        if (status == 0)
        {
            beep = 2; //震动传感器使用蜂鸣器
            buzzerDeviceTmp->open(buzzerDeviceTmp->pinNum);
            memset(Message[3], 0, sizeof Message[3]); //清空数组
            sprintf(Message[3], "警报：发生震动 ！");

            delay(300);
        }
        else if ((beep != 1) && (beep != 3)) //蜂鸣器未被火焰传感器和人体红外传感器使用
        {
            memset(Message[3], 0, sizeof Message[3]); //清空数组
            sprintf(Message[3], "正常");
            buzzerDeviceTmp->close(buzzerDeviceTmp->pinNum);
            beep = 0; //蜂鸣器未被使用
        }
    }
}

void *human_thread(void *datas) // 人体红外检测线程
{
    char msg[100];
    int status;
    struct Devices *humanDeviceTmp = NULL;
    struct Devices *buzzerDeviceTmp = NULL;

    humanDeviceTmp = findDeviceByName("human", pdeviceHead); //在设备工厂找到火灾模块
    buzzerDeviceTmp = findDeviceByName("buzzser", pdeviceHead);

    humanDeviceTmp->deviceInit(humanDeviceTmp->pinNum); //红外模块初始化
    buzzerDeviceTmp->deviceInit(buzzerDeviceTmp->pinNum);
    printf("人体红外检测初始化\n");

    while (1)
    {
        delay(10);
        status = humanDeviceTmp->readStatus(humanDeviceTmp->pinNum); //读取人体感应模块实时数据

        if (status == 1)
        {
            beep = 3;                                 //人体红外使用蜂鸣器
            memset(Message[4], 0, sizeof Message[4]); //清空数组
            sprintf(Message[4], "警报：有人进入 ！");
            buzzerDeviceTmp->open(buzzerDeviceTmp->pinNum);
            delay(200);
            buzzerDeviceTmp->close(buzzerDeviceTmp->pinNum);
            delay(100);
        }
        else if ((beep != 1) && (beep != 2)) //未被火焰传感器和震动传感器使用
        {
            memset(Message[4], 0, sizeof Message[4]); //清空数组
            sprintf(Message[4], "正常");
            buzzerDeviceTmp->close(buzzerDeviceTmp->pinNum);
            beep = 0; //蜂鸣器未被使用
        }
    }
}

void *monitoring_thread(void *datas) //视频监控线程
{
    system("./start_web_video.sh"); //执行脚本，打开视频监控
    pthread_exit(NULL);             //退出线程
}

void *voice_thread(void *arg) //语音线程
{
    int i = 0;
    int nread;
    struct InputCommander *voiceHandler = NULL;
    struct Devices *deviceTmp = NULL;

    voiceHandler = findCommandByName("voice", pCommandHead); //在控制工厂找到语音模块

    if (voiceHandler == NULL)
    {
        printf("查找语音处理程序错误\n");
        pthread_exit(NULL);
    }
    else
    {
        if (voiceHandler->Init(voiceHandler, NULL, NULL) < 0)
        { //语音模块初始化
            printf("语音初始化错误\n");
            pthread_exit(NULL); //退出线程
        }
        else
        {
            printf("%s 初始化成功\n", voiceHandler->commandName);
        } //语音初始化完成

        pthread_mutex_lock(&mutex); //加锁
                                    //语音读取一级指令的时候，为了避免其它线程对于 紧接着读取二级指令的干扰
        while (1)
        {
            memset(voiceHandler->comand, '\0', sizeof(voiceHandler->comand));

            nread = voiceHandler->getCommand(voiceHandler); //读取来自语音模块的串口数据
            printf("get voice command:%s\n", voiceHandler->comand);
            if (nread == 0)
            {
                printf("没有来自语音的数据，请再说一遍\n");
            }
            else if (strstr(voiceHandler->comand, "kwsd") != NULL) //打开卧室灯命令
            {
                printf("打开卧室灯光\n");
                deviceTmp = findDeviceByName("shui", pdeviceHead); //查找卧室灯设备
                deviceTmp->deviceInit(deviceTmp->pinNum);          //卧室灯设备初始化
                deviceTmp->open(deviceTmp->pinNum);                //打开卧室灯
            }
            else if (strstr(voiceHandler->comand, "gwsd") != NULL)
            {
                printf("关闭卧室灯光\n");
                deviceTmp = findDeviceByName("shui", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->close(deviceTmp->pinNum);
            }
            else if (strstr(voiceHandler->comand, "kysd") != NULL)
            {
                printf("打开浴室灯光\n");
                deviceTmp = findDeviceByName("yu", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->open(deviceTmp->pinNum);
            }
            else if (strstr(voiceHandler->comand, "gysd") != NULL)
            {
                printf("关闭浴室灯光\n");
                deviceTmp = findDeviceByName("yu", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->close(deviceTmp->pinNum);
            }
            else if (strstr(voiceHandler->comand, "kktd") != NULL)
            {
                printf("打开客厅灯光\n");
                deviceTmp = findDeviceByName("kt", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->open(deviceTmp->pinNum);
            }
            else if (strstr(voiceHandler->comand, "gktd") != NULL)
            {
                printf("关闭客厅灯光\n");
                deviceTmp = findDeviceByName("kt", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->close(deviceTmp->pinNum);
            }
            else if (strstr(voiceHandler->comand, "kctd") != NULL)
            {
                printf("打开餐厅灯光\n");
                deviceTmp = findDeviceByName("chu", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->open(deviceTmp->pinNum);
            }
            else if (strstr(voiceHandler->comand, "gctd") != NULL)
            {
                printf("关闭餐厅灯光\n");
                deviceTmp = findDeviceByName("chu", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->close(deviceTmp->pinNum);
            }
            else if (strstr(voiceHandler->comand, "kycd") != NULL) //开泳灯指令
            {
                printf("打开泳池灯光\n");
                deviceTmp = findDeviceByName("yong", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->open(deviceTmp->pinNum);
            }
            else if (strstr(voiceHandler->comand, "gqbd") != NULL) //关闭全部灯指令
            {
                printf("关闭所有灯光\n");
                deviceTmp = findDeviceByName("chu", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->close(deviceTmp->pinNum); //关闭餐厅灯
                deviceTmp = findDeviceByName("yu", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->close(deviceTmp->pinNum); //关闭浴室灯
                deviceTmp = findDeviceByName("kt", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->close(deviceTmp->pinNum); //关闭客厅灯
                deviceTmp = findDeviceByName("shui", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->close(deviceTmp->pinNum); //关闭卧室灯
            }
            else if (strstr(voiceHandler->comand, "kqbd") != NULL)
            {
                printf("打开所有灯光\n");
                deviceTmp = findDeviceByName("chu", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->open(deviceTmp->pinNum);
                deviceTmp = findDeviceByName("yu", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->open(deviceTmp->pinNum);
                deviceTmp = findDeviceByName("kt", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->open(deviceTmp->pinNum);
                deviceTmp = findDeviceByName("shui", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->open(deviceTmp->pinNum);
            }
            else if (strstr(voiceHandler->comand, "gycd") != NULL) //关泳池灯指令
            {
                printf("关闭泳池灯光\n");
                deviceTmp = findDeviceByName("yong", pdeviceHead); //搜索泳池灯设备
                deviceTmp->deviceInit(deviceTmp->pinNum);          //泳池灯设备初始化
                deviceTmp->close(deviceTmp->pinNum);               //关泳池灯
            }
            else if (strstr(voiceHandler->comand, "kdfs") != NULL) //开电风扇指令
            {
                printf("打开电风扇\n");
                deviceTmp = findDeviceByName("fan", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->open(deviceTmp->pinNum);
            }
            else if (strstr(voiceHandler->comand, "gdfs") != NULL) //关电风扇指令
            {
                printf("关闭电风扇\n");
                deviceTmp = findDeviceByName("fan", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->close(deviceTmp->pinNum);
            }
            else if (strstr(voiceHandler->comand, "ks") != NULL) //开锁指令
            {
                printf("打开门锁\n");
                deviceTmp = findDeviceByName("lock", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->open(deviceTmp->pinNum);
                delay(3000);
                deviceTmp->close(deviceTmp->pinNum);
            }
            /*else
            {
                deviceTmp = findDeviceByName(voiceHandler->comand, pdeviceHead);
                if (deviceTmp == NULL)
                {
                    printf("查找设备按名称错误\n");
                }
                else
                {
                    printf("查找设备 = %s\n", deviceTmp->deviceName);
                    deviceTmp->deviceInit(deviceTmp->pinNum);
                    deviceTmp->open(deviceTmp->pinNum);
                }
            }*/
        }

        pthread_mutex_unlock(&mutex); //解锁
    }
}

void *write_thread(void *datas) //通知线程，向客服端发送消息
{
    while (1)
    {
        delay(500);
        write(c_fd, Message, 500);
    }
}

void *read_thread(void *datas) //通过socket读取客户端发来的数据
{

    int n_read;
    struct Devices *deviceTmp = NULL;

    while (1)
    {
        memset(socketHandler->comand, '\0', sizeof(socketHandler->comand));

        n_read = read(c_fd, socketHandler->comand, sizeof(socketHandler->comand)); //读取客户端发来的数据
        if (n_read == -1)
        {
            perror("read_thread");
        }
        else if (n_read > 0)
        {
            printf("APP客户端指令:%s\n", socketHandler->comand);
            //处理客户端读到的命令
            if (strstr(socketHandler->comand, "kws") != NULL) //开卧室灯
            {
                printf("开卧室灯\n");
                deviceTmp = findDeviceByName("shui", pdeviceHead); //查找卧室灯设备
                deviceTmp->deviceInit(deviceTmp->pinNum);          //卧室灯设备初始化
                deviceTmp->open(deviceTmp->pinNum);                //打开卧室灯
            }
            else if (strstr(socketHandler->comand, "gws") != NULL) //关卧室灯
            {
                printf("关卧室灯\n");
                deviceTmp = findDeviceByName("shui", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->close(deviceTmp->pinNum);
            }
            else if (strstr(socketHandler->comand, "kys") != NULL) //开浴室灯
            {
                printf("开浴室灯\n");
                deviceTmp = findDeviceByName("yu", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->open(deviceTmp->pinNum);
            }
            else if (strstr(socketHandler->comand, "gys") != NULL) //关浴室灯
            {
                printf("关浴室灯\n");
                deviceTmp = findDeviceByName("yu", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->close(deviceTmp->pinNum);
            }
            else if (strstr(socketHandler->comand, "kkt") != NULL) //开客厅灯
            {
                printf("开客厅灯\n");
                deviceTmp = findDeviceByName("kt", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->open(deviceTmp->pinNum);
            }
            else if (strstr(socketHandler->comand, "gkt") != NULL) //关客厅灯
            {
                printf("关客厅灯\n");
                deviceTmp = findDeviceByName("kt", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->close(deviceTmp->pinNum);
            }
            else if (strstr(socketHandler->comand, "kct") != NULL) //开餐厅灯
            {
                printf("开餐厅灯\n");
                deviceTmp = findDeviceByName("chu", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->open(deviceTmp->pinNum);
            }
            else if (strstr(socketHandler->comand, "gct") != NULL) //关餐厅灯
            {
                printf("关餐厅灯\n");
                deviceTmp = findDeviceByName("chu", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->close(deviceTmp->pinNum);
            }
            else if (strstr(socketHandler->comand, "kyc") != NULL) //开泳池灯
            {
                printf("开泳池灯\n");
                deviceTmp = findDeviceByName("yong", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->open(deviceTmp->pinNum);
            }
            else if (strstr(socketHandler->comand, "gyc") != NULL) //关泳池灯
            {
                printf("关泳池灯\n");
                deviceTmp = findDeviceByName("yong", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->close(deviceTmp->pinNum);
            }
            else if (strstr(socketHandler->comand, "kfs") != NULL) //开风扇
            {
                printf("开电风扇\n");
                deviceTmp = findDeviceByName("fan", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->open(deviceTmp->pinNum);
            }
            else if (strstr(socketHandler->comand, "gfs") != NULL) //关风扇
            {
                printf("关电风扇\n");
                deviceTmp = findDeviceByName("fan", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->close(deviceTmp->pinNum);
            }
            else if (strstr(socketHandler->comand, "kqb") != NULL) //开室内全部灯
            {
                printf("开室内全部灯\n");
                deviceTmp = findDeviceByName("chu", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->open(deviceTmp->pinNum);
                deviceTmp = findDeviceByName("yu", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->open(deviceTmp->pinNum);
                deviceTmp = findDeviceByName("kt", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->open(deviceTmp->pinNum);
                deviceTmp = findDeviceByName("shui", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->open(deviceTmp->pinNum);
            }
            else if (strstr(socketHandler->comand, "gqb") != NULL) //关室内全部灯
            {
                printf("关室内全部灯\n");
                deviceTmp = findDeviceByName("chu", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->close(deviceTmp->pinNum);
                deviceTmp = findDeviceByName("yu", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->close(deviceTmp->pinNum);
                deviceTmp = findDeviceByName("kt", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->close(deviceTmp->pinNum);
                deviceTmp = findDeviceByName("shui", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->close(deviceTmp->pinNum);
            }
            else if (strstr(socketHandler->comand, "ks") != NULL) //开锁
            {
                printf("开锁\n");
                deviceTmp = findDeviceByName("lock", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->open(deviceTmp->pinNum);
            }
            else if (strstr(socketHandler->comand, "gs") != NULL) //关锁
            {
                deviceTmp = findDeviceByName("lock", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->close(deviceTmp->pinNum);
            }
            else if (strstr(socketHandler->comand, "kjk") != NULL) //开监控
            {
                printf("打开监控\n");
                pthread_create(&monitoringThread, NULL, monitoring_thread, NULL); //启动视频监控线程
            }
            else if (strstr(socketHandler->comand, "gjk") != NULL) //关监控
            {
                printf("关闭监控\n");
                // pthread_cancel(monitoringThread);
                system("killall -TERM mjpg_streamer"); //关闭摄像头监控功能
            }
            else if (strstr(socketHandler->comand, "khw") != NULL) //打开人体红外检测
            {
                printf("启动人体红外检测线程\n");
                beep = 3; //人体红外使用蜂鸣器
                deviceTmp = findDeviceByName("buzzser", pdeviceHead);
                deviceTmp->open(deviceTmp->pinNum);
                delay(700);
                deviceTmp->close(deviceTmp->pinNum);
                if (human_flag == 0)
                {
                    pthread_create(&humanThread, NULL, human_thread, NULL);
                    human_flag = 1;
                }
            }
            else if (strstr(socketHandler->comand, "ghw") != NULL) //关闭人体红外检测
            {
                printf("关闭人体红外检测\n");
                memset(Message[4], 0, sizeof Message[4]); //清空数组
                sprintf(Message[4], "未启用！");

                if (human_flag == 1)
                {
                    pthread_cancel(humanThread); //停止人体红外线程
                    human_flag = 0;
                }

                beep = 3; //人体红外使用蜂鸣器
                deviceTmp = findDeviceByName("buzzser", pdeviceHead);
                deviceTmp->open(deviceTmp->pinNum);
                delay(400);
                deviceTmp->close(deviceTmp->pinNum);
                delay(300);
                deviceTmp->open(deviceTmp->pinNum);
                delay(400);
                deviceTmp->close(deviceTmp->pinNum);
            }
            else if (strstr(socketHandler->comand, "gjms") != NULL) //归家模式
            {
                printf("开室内全部灯\n");
                deviceTmp = findDeviceByName("chu", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->open(deviceTmp->pinNum);
                deviceTmp = findDeviceByName("yu", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->open(deviceTmp->pinNum);
                deviceTmp = findDeviceByName("kt", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->open(deviceTmp->pinNum);
                deviceTmp = findDeviceByName("shui", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->open(deviceTmp->pinNum);

                printf("关闭人体红外检测\n");
                memset(Message[4], 0, sizeof Message[4]); //清空数组
                sprintf(Message[4], "未启用！");

                if (human_flag == 1)
                {
                    pthread_cancel(humanThread); //停止人体红外线程
                    human_flag = 0;
                }

                beep = 3; //人体红外使用蜂鸣器
                deviceTmp = findDeviceByName("buzzser", pdeviceHead);
                deviceTmp->open(deviceTmp->pinNum);
                delay(400);
                deviceTmp->close(deviceTmp->pinNum);
                delay(300);
                deviceTmp->open(deviceTmp->pinNum);
                delay(400);
                deviceTmp->close(deviceTmp->pinNum);
                beep = 0;
            }
            else if (strstr(socketHandler->comand, "ljms") != NULL) //离家模式
            {
                printf("关全部灯\n");
                deviceTmp = findDeviceByName("chu", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->close(deviceTmp->pinNum);
                deviceTmp = findDeviceByName("yu", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->close(deviceTmp->pinNum);
                deviceTmp = findDeviceByName("kt", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->close(deviceTmp->pinNum);
                deviceTmp = findDeviceByName("shui", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->close(deviceTmp->pinNum);
                deviceTmp = findDeviceByName("yong", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->close(deviceTmp->pinNum);

                printf("关电风扇\n");
                deviceTmp = findDeviceByName("fan", pdeviceHead);
                deviceTmp->deviceInit(deviceTmp->pinNum);
                deviceTmp->close(deviceTmp->pinNum);

                printf("启动人体红外检测线程\n");
                beep = 3; //人体红外使用蜂鸣器
                deviceTmp = findDeviceByName("buzzser", pdeviceHead);
                deviceTmp->open(deviceTmp->pinNum);
                delay(700);
                deviceTmp->close(deviceTmp->pinNum);
                beep = 0;
                if (human_flag == 0)
                {
                    pthread_create(&humanThread, NULL, human_thread, NULL);
                    human_flag = 1;
                }
            }
            else if (strstr(socketHandler->comand, "face") != NULL)
            {
                //启动人脸识别线程
                printf("进行人脸识别开锁\n");
                pthread_create(&cameraThread, NULL, cameraThread_func, NULL);
            }
        }
        else
        {
            printf("客户端退出\n");
            // exit(-1); //客户端退出，服务器程序退出
            pthread_cancel(writeThread); //停止通知线程
            write_flag = 0;              //通知线程标志位置0，通知线程关闭
            pthread_exit(NULL);          //退出本线程
        }
    }
}

void *socket_thread(void *datas) //开启socket服务端，并将socket服务端初始化
{

    int n_read = 0;
    pthread_t readPthread;

    struct sockaddr_in c_addr;
    memset(&c_addr, 0, sizeof(struct sockaddr_in));
    int clen = sizeof(struct sockaddr_in);

    socketHandler = findCommandByName("socketServer", pCommandHead); //在控制工厂找到socket
    if (socketHandler == NULL)
    {
        printf("查找套接字处理程序错误\n");
        pthread_exit(NULL);
    }
    else
    {
        printf("%s 初始化成功\n", socketHandler->commandName);
    }

    socketHandler->Init(socketHandler, NULL, NULL); //初始化socket

    while (1)
    {

        c_fd = accept(socketHandler->sfd, (struct sockaddr *)&c_addr, &clen);
        printf("c_fd = %d\n", c_fd);
        if (write_flag == 0) //通知线程处于关闭状态
        {
            write_flag = 1;
            pthread_create(&writeThread, NULL, write_thread, NULL); //打开通知线程
        }
        pthread_create(&readPthread, NULL, read_thread, NULL);
    }
}

void *remote_thread(void *datas) // 433m遥控线程
{
    int val = 0, val1 = 0, val2 = 0, val3 = 0;
    int count = 0, count1 = 0, count2 = 0, count3 = 0;
    int temp = 0, temp1 = 0, temp2 = 0, temp3 = 0;

    struct Devices *deviceTmp = NULL;

    wiringPiSetup();
    pinMode(D0, INPUT);
    pinMode(D1, INPUT);
    pinMode(D2, INPUT);
    pinMode(D3, INPUT);

    digitalWrite(D0, LOW);
    digitalWrite(D1, LOW);
    digitalWrite(D2, LOW);
    digitalWrite(D3, LOW);

    printf("遥控模块初始化成功\n");

    while (1)
    {

        delay(10);

        val = digitalRead(D0);
        val1 = digitalRead(D1);
        val2 = digitalRead(D2);
        val3 = digitalRead(D3);
        if (val == 1) //电风扇遥控信号
        {
            if (count == 0) //电风扇遥控按键被按下
            {
                if (temp == 0) //打开电风扇
                {
                    printf("433M：打开电风扇\n");
                    temp = 1;
                    deviceTmp = findDeviceByName("fan", pdeviceHead); //查找电风扇设备
                    deviceTmp->deviceInit(deviceTmp->pinNum);         //电风扇设备初始化
                    deviceTmp->open(deviceTmp->pinNum);               //打开电风扇
                }
                else if (temp == 1) //关闭电风扇
                {
                    printf("433M：关闭电风扇\n");
                    temp = 0;
                    deviceTmp = findDeviceByName("fan", pdeviceHead); //查找电风扇设备
                    deviceTmp->deviceInit(deviceTmp->pinNum);         //电风扇设备初始化
                    deviceTmp->close(deviceTmp->pinNum);              //关闭电风扇
                }

                count = 1;
            }
        }
        else if (val == 0)
        {
            count = 0;
        }

        if (val1 == 1) //门锁遥控
        {
            if (count1 == 0)
            {
                if (temp1 == 0)
                {
                    printf("433M：打开门锁\n");
                    temp1 = 1;
                    deviceTmp = findDeviceByName("lock", pdeviceHead);
                    deviceTmp->deviceInit(deviceTmp->pinNum);
                    deviceTmp->open(deviceTmp->pinNum);
                    delay(3000);
                    deviceTmp->close(deviceTmp->pinNum);
                }
                else if (temp1 == 1)
                {
                    printf("433M：关闭门锁\n");
                    temp1 = 0;
                    deviceTmp = findDeviceByName("lock", pdeviceHead);
                    deviceTmp->deviceInit(deviceTmp->pinNum);
                    deviceTmp->close(deviceTmp->pinNum);
                }

                count1 = 1;
            }
        }
        else if (val1 == 0)
        {
            count1 = 0;
        }

        if (val2 == 1) //泳池灯遥控
        {
            if (count2 == 0)
            {
                if (temp2 == 0)
                {
                    printf("433M：开泳池灯\n");
                    temp2 = 1;

                    deviceTmp = findDeviceByName("yong", pdeviceHead);
                    deviceTmp->deviceInit(deviceTmp->pinNum);
                    deviceTmp->open(deviceTmp->pinNum);
                }
                else if (temp2 == 1)
                {
                    printf("433M：关泳池灯\n");
                    temp2 = 0;
                    deviceTmp = findDeviceByName("yong", pdeviceHead);
                    deviceTmp->deviceInit(deviceTmp->pinNum);
                    deviceTmp->close(deviceTmp->pinNum);
                }

                count2 = 1;
            }
        }
        else if (val2 == 0)
        {
            count2 = 0;
        }

        if (val3 == 1) //人体红外线程控制
        {
            if (count3 == 0)
            {
                if (temp3 == 0) // 启动人体红外检测线程
                {
                    printf("433M：启动人体红外\n");
                    temp3 = 1;
                    printf("启动人体红外检测线程\n");
                    beep = 3; //人体红外使用蜂鸣器
                    deviceTmp = findDeviceByName("buzzser", pdeviceHead);
                    // deviceTmp->deviceInit(deviceTmp->pinNum);
                    deviceTmp->open(deviceTmp->pinNum);
                    delay(700);
                    deviceTmp->close(deviceTmp->pinNum);
                    beep = 0;
                    if (human_flag == 0)
                    {
                        pthread_create(&humanThread, NULL, human_thread, NULL);
                        human_flag = 1;
                    }
                }
                else if (temp3 == 1)
                {
                    printf("433M：关闭人体红外\n");
                    memset(Message[4], 0, sizeof Message[4]); //清空数组
                    sprintf(Message[4], "未启用！");
                    temp3 = 0;
                    printf("关闭人体红外检测线程\n");
                    beep = 3; //人体红外使用蜂鸣器
                              // TerminateThread(human_thread, -1 );  // 中止人体红外检测线程
                              // CloseHandle(human_thread); //清楚人体红外线程堆栈
                    // _beginthread(human_thread,0, NULL);
                    if (human_flag == 1)
                    {
                        pthread_cancel(humanThread); //停止人体红外线程
                        human_flag = 0;
                    }

                    deviceTmp = findDeviceByName("buzzser", pdeviceHead);
                    // deviceTmp->deviceInit(deviceTmp->pinNum);
                    deviceTmp->open(deviceTmp->pinNum);
                    delay(400);
                    deviceTmp->close(deviceTmp->pinNum);
                    delay(300);
                    deviceTmp->open(deviceTmp->pinNum);
                    delay(400);
                    deviceTmp->close(deviceTmp->pinNum);
                    beep = 0;
                }

                count3 = 1;
            }
        }
        else if (val3 == 0)
        {
            count3 = 0;
        }
    }
}

//检测按键状态
void *key_thread()
{
    // pinMode(door_Lock, OUTPUT); //将门锁端口置为输出
    int val;
    while (1)
    {
        val = digitalRead(key);
        // printf("key = %d\n", val);
        if (val == 0) //防止重复检测
        {
            delay(500);
            val = digitalRead(key);
            // printf("key = %d\n", val);
            if (val == 1) //按键按下，启动人脸识别线程
            {
                pthread_create(&cameraThread, NULL, cameraThread_func, NULL);
            }
        }
    }
}

int main()
{
    char name[32] = {'\0'};

    //树莓派库初始化
    if (wiringPiSetup() == -1)
    {
        printf("硬件接口初始化失败\n");
        return -1;
    }

    pthread_mutex_init(&mutex, NULL); //初始化互斥量（锁）

    // 1、指令工厂初始化
    pCommandHead = addVoiceContrlToInputCommanderLink(pCommandHead);  //语音识别初始化
    pCommandHead = addsocketContrlToInputCommanderLink(pCommandHead); //服务器初始化

    // 2、设备控制工程初始化
    pdeviceHead = addBathroomLightToDeviceLink(pdeviceHead);   //浴室灯光初始化
    pdeviceHead = addupStairLightToDeviceLink(pdeviceHead);    //卧室灯光初始化
    pdeviceHead = addlivingroomLightToDeviceLink(pdeviceHead); //客厅灯光初始化
    pdeviceHead = addrestaurantLightToDeviceLink(pdeviceHead); //餐厅灯光初始化
    pdeviceHead = addpoolLightToDeviceLink(pdeviceHead);       //泳池灯光初始化
    pdeviceHead = addDoorLockToDeviceLink(pdeviceHead);        //门锁初始化
    pdeviceHead = addFanToDeviceLink(pdeviceHead);             //电风扇初始化
    pdeviceHead = addBuzzerToDeviceLink(pdeviceHead);          //蜂鸣器初始化
    pdeviceHead = addFireToDeviceLink(pdeviceHead);            //火焰传感器初始化
    pdeviceHead = addShakeToDeviceLink(pdeviceHead);           //震动传感器初始化
    pdeviceHead = addcameraToDeviceLink(pdeviceHead);          //摄像头模块初始化
    pdeviceHead = addHumanToDeviceLink(pdeviceHead);           //人体感应传感器初始化

    //参数2：线程属性，一般设置为NULL，参数3：线程干活的函数，参数4：往voice_thread线程里面传送数据。
    pthread_create(&voiceThread, NULL, voice_thread, NULL);   // 语音线程启动
    pthread_create(&socketThread, NULL, socket_thread, NULL); // socket服务器线程启动
    pthread_create(&fireThread, NULL, fire_thread, NULL);     //火灾报警线程启动
    pthread_create(&shakeThread, NULL, shake_thread, NULL);   //震动报警线程启动
    pthread_create(&keyThread, NULL, key_thread, NULL);       //按键检测线程启动
    pthread_create(&remoteThread, NULL, remote_thread, NULL); // 遥控线程启动
    pthread_create(&dht11Thread, NULL, dht11_thread, NULL);   // 温湿度检测线程启动

    //等待线程
    pthread_join(voiceThread, NULL);
    pthread_join(socketThread, NULL);
    pthread_join(fireThread, NULL);
    pthread_join(shakeThread, NULL);
    pthread_join(humanThread, NULL);
    pthread_join(keyThread, NULL);
    pthread_join(remoteThread, NULL);
    pthread_join(cameraThread, NULL);
    pthread_join(monitoringThread, NULL);
    pthread_join(clientWemosThread, NULL);
    pthread_join(dht11Thread, NULL);
    pthread_join(writeThread, NULL);

    pthread_mutex_destroy(&mutex); //销毁互斥量
                                   // pthread_cond_destroy(&cond);        //条件的销毁

    return 0;
}