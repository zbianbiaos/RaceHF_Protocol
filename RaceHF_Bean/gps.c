#include <stdio.h>
#include <stdint.h>


typedef struct
{
  uint32_t    seconds;        /* UNIX时间戳 */
  uint16_t    milliseconds;   /* 毫秒数 */
  double      longitude;      /* 经度 */
  double      latitude;       /* 纬度 */
  float       speed;          /* 速度 */
  float       direction;      /* 方位角 */
  float       hdop;           /* HDOP */
  int16_t     height;         /* 海拔 */
  uint8_t     satellites;     /* 锁定卫星数 */
  uint8_t     ablesatellites; /* 可视卫星数 */
  uint8_t     locatemode;     /* 定位方式 */
  uint8_t     gp_locks;       /* GPS锁定卫星数 */
  uint8_t     gl_locks;       /* GLONASS锁定卫星数 */
  uint8_t     ga_locks;       /* GALILEO锁定卫星数 */
  uint8_t     bd_locks;       /* 北斗锁定卫星数 */ 
} stuGPS;

// 第一包：index = 0x10
#pragma pack(1)
typedef struct
{
  uint8_t     index;
  double      longitude;      /* 经度 */
  double      latitude;       /* 纬度 */
  int16_t     height;         /* 海拔 */
  uint8_t     locatemode;     /* 定位方式 */
} stuGPSPack1;

// 第二包：index = 0x11
typedef struct
{
  uint8_t     index;
  uint32_t    seconds;        /* UNIX时间戳 */
  uint16_t    milliseconds;   /* 毫秒数 */
  float       speed;          /* 速度 */
  float       direction;      /* 方位角 */
  float       hdop;           /* HDOP */
  uint8_t     satellites;     /* 锁定卫星数 */
} stuGPSPack2;
#pragma pack()

static void ShowArrayHex(void *array, uint32_t size);


int main(void)
{
    stuGPS gps = 
    {
        .seconds      = 1568443193,       // 2019/9/14 06:39:53
        .milliseconds = 350,              // 350ms
        .longitude    = 123.12345678,     // 东经
        .latitude     = -23.45678912,     // 南纬
        .speed        = 112.34f,          // km/h
        .direction    = 123.123f,         // 正北方顺时针夹角
        .hdop         = 1.24f,            // 水平定位经度
        .height       = 123,              // 海拔高度，整数类型，单位米
        .satellites   = 18,               // 参与定位的锁定卫星数量
        .locatemode   = 3,                // 0:未定位; 1:2D定位; 2:3D定位; 3:DPGS模式3D定位
        .gp_locks     = 10,               // GPS(美国全球定位卫星系统)定位10颗卫星
        .gl_locks     = 3,                // GLONASS辅助GPS定位卫星3颗
        .ga_locks     = 5,                // GALILEO辅助定位卫星5颗
        .bd_locks     = 0,                // BEIDOU卫星没有参与定位系统
    };
    
    stuGPSPack1 gps1;
    stuGPSPack2 gps2;

    gps1.index        = 0x10;
    gps1.longitude    = gps.longitude;
    gps1.latitude     = gps.latitude;
    gps1.height       = gps.height;
    gps1.locatemode   = gps.locatemode;

    gps2.index        = 0x11;
    gps2.seconds      = gps.seconds;
    gps2.milliseconds = gps.milliseconds;
    gps2.speed        = gps.speed;
    gps2.direction    = gps.direction;
    gps2.hdop         = gps.hdop;
    gps2.satellites   = gps.satellites;

    printf("sizeof(stuGPSPack1) = %lu\r\ncontent = ", sizeof(stuGPSPack1));
    ShowArrayHex((void*)&gps1, sizeof(stuGPSPack1));

    printf("sizeof(stuGPSPack2) = %lu\r\ncontent = ", sizeof(stuGPSPack2));
    ShowArrayHex((void*)&gps2, sizeof(stuGPSPack2));

    return 0;
}


static void ShowArrayHex(void *array, uint32_t size)
{
    for(uint32_t n = 0; n < size; n++)
    {
        printf("0x%02X ", ((uint8_t*)array)[n]);
    }
    puts("\r\n");
}