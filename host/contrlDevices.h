//外设设备的头文件
#include <wiringPi.h>
#include <stdio.h>
#include <curl/curl.h>

typedef unsigned int bool;

struct Devices
{
    char deviceName[128]; //设备名
    int status;           //读取到的数据
    int pinNum;           //引脚号

    int (*open)(int pinNum);       //打开设备函数指针
    int (*close)(int pinNum);      //关闭设备函数指针
    int (*deviceInit)(int pinNum); //设备初始化函数指针

    int (*readStatus)(int pinNum);  //读取数据函数指针
    int (*changStatus)(int status); //改变数据函数指针

    //摄像头相关的
    void (*justDoOnce)();           //用于摄像头
    char *(*getFace)();             //用于摄像头
    char *(*getPicFromOCRBase64)(); //用于摄像头
    size_t (*readData)();           //用于摄像头

    CURL *curl;
    char *key;
    char *secret;
    int typeId;
    char *format;
    bool (*cameraInit)(struct Devices *camera);
    int yesNum;
    int noNum;

    struct Devices *next;
};

//每个设备加到链表函数的声明
struct Devices *addBathroomLightToDeviceLink(struct Devices *phead);
struct Devices *addupStairLightToDeviceLink(struct Devices *phead);
struct Devices *addlivingroomLightToDeviceLink(struct Devices *phead);
struct Devices *addrestaurantLightToDeviceLink(struct Devices *phead);
struct Devices *addpoolLightToDeviceLink(struct Devices *phead);
struct Devices *addDoorLockToDeviceLink(struct Devices *phead);
struct Devices *addFanToDeviceLink(struct Devices *phead);
struct Devices *addFireToDeviceLink(struct Devices *phead);
struct Devices *addShakeToDeviceLink(struct Devices *phead);
struct Devices *addHumanToDeviceLink(struct Devices *phead) ;
struct Devices *addBuzzerToDeviceLink(struct Devices *phead);

// struct Devices *addcameraContrlToDeviceLink(struct Devices *phead);
struct Devices *addcameraToDeviceLink(struct Devices *phead);
