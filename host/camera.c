//摄像头
#include "contrlDevices.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#define door_lock 29 //门锁
void postUrl();
size_t readData1(void *ptr, size_t size, size_t nmemb, void *stream);
char *getFace1();
char *getPicFromOCRBase641(char *Filepath);
struct Devices *addcameraToDeviceLink(struct Devices *phead);

char ocrRetBuf[1024] = {'\0'}; //全局变量，用来接收从OCR后台返回的数据

size_t readData1(void *ptr, size_t size, size_t nmemb, void *stream)
//回调函数，把从后台的数据拷贝给ocrRetBuf
{
    strncpy(ocrRetBuf, ptr, 1024);
}

char *getFace1()
{
    printf("拍照中...\n");
    system("raspistill -q 5 -t 1 -o image.jpg"); //-q 是图片质量，在0~100之间，我们调成5，压缩图片质量,生成的照片名字为imag.jpg
                                                 //-t 是拍照延时，设定1s后拍照

    while (access("./image.jpg", F_OK) != 0)
        ; //判断是否拍照完毕

    printf("拍照完成\n");

    char *base64BufFaceRec = getPicFromOCRBase641("./image.jpg");
    // system("rm image.jpg");

    return base64BufFaceRec; //返回刚才拍照的base64
}

char *getPicFromOCRBase641(char *Filepath)
{
    int fd;
    int filelen;
    char cmd[128] = {'\0'};

    sprintf(cmd, "base64 %s > tmpFile", Filepath);
    system(cmd);
    fd = open("./tmpFile", O_RDWR);
    filelen = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    char *bufpic = (char *)malloc(filelen + 2);
    memset(bufpic, '\0', filelen + 2);
    read(fd, bufpic, filelen + 128);
    system("rm -rf tmpFile");
    close(fd);

    return bufpic;
}

void postUrl()
{
    CURL *curl;
    CURLcode res;

    struct Devices *deviceTmp = NULL;

    //分开定义，然后字符串拼接
    char *key = "XXXXejLCjAKZUN2D63XXXX";                  //用自己的
    char *secret = "XXXXXXXXadca46859adXXXX69092XXXXXXXX"; //用自己的
    int typeId = 21;
    char *format = "xml";

    char *base64BufPic1 = getFace1();
    char *base64BufPic2 = getPicFromOCRBase641("gyf1.jpg");

    int len = strlen(key) + strlen(secret) + strlen(base64BufPic1) + strlen(base64BufPic2) + 128; //分配空间不够会导致栈溢出
    char *postString = (char *)malloc(len);
    memset(postString, '\0', len); //因为postString是一个指针，不能用sizeof来计算其指向的大小

    sprintf(postString, "img1=%s&img2=%s&key=%s&secret=%s&typeId=%d&format=%s", base64BufPic1, base64BufPic2, key, secret, typeId, format); //根据平台的传参格式编写

    curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postString);                   //指定post内容，传入参数
        curl_easy_setopt(curl, CURLOPT_URL, "https://netocr.com/api/faceliu.do"); // 指定url
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, readData1);                 //回调函数readDate读取返回值
        res = curl_easy_perform(curl);                                            //类似于状态码
        printf("OK:%d\n", res);

        if (strstr(ocrRetBuf, "是") != NULL)
        { //判断翔云后台返回的字符串中有没有“是”
            printf("是同一个人\n");
            digitalWrite(door_lock, LOW);  //打开门锁
            delay(3000);                   //等待3s
            digitalWrite(door_lock, HIGH); //关闭门锁
        }
        else
        {
            printf("不是同一个人\n");
        }
        curl_easy_cleanup(curl);
    }
}

int cameraInit(int pinNum) //该函数无用
{
    pinMode(pinNum, OUTPUT);
    digitalWrite(pinNum, HIGH);
    //  printf("摄像头模块初始化成功\n");
}

struct Devices camera = {

    .deviceName = "camera",
    .deviceInit = cameraInit,
    .pinNum = 6, //此引脚号无用
    .justDoOnce = postUrl,
    .getFace = getFace1,
    .getPicFromOCRBase64 = getPicFromOCRBase641,
    .readData = readData1

};

struct Devices *addcameraToDeviceLink(struct Devices *phead)
{
    if (phead == NULL)
    {
        return &camera;
    }
    else
    {
        camera.next = phead;
        phead = &camera;
    }
}
