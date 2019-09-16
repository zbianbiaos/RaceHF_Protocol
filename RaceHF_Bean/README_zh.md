# RaceHF Bean 协议

**[概述](#概述)**
**[开发说明](#开发说明)**
**[定位数据协议](#定位数据协议)**
**[模式设置协议](#模式设置协议)**

## 概述

\[[English](README.md)\]
[[中文](README_zh.md)\]

Bean面向广大赛车爱好者，为广大车友们提供一个融合软硬件平台的优质服务产品。

产品主要特点：

- **32位超低功耗**嵌入式处理器
- 支持**蓝牙5.0**协议
- 优质定位芯片，最高真实刷新率可达**20hz**，专为赛车应用场景优化算法。兼容GPS/GLONASS，内置高灵敏度陶瓷天线，可外接天线
- 支持空中固件升级
- 支持SD卡记录VBO格式文件，把记录的数据导入CTool Track attack等主流的赛车数据分析软件，实现开机即用，不再依赖手机
- 开放的数据通讯协议，第三方软件开发商快速开发和硬件兼容的APP
- 支持Andriod与IOS
- 支持全球使用最广泛的两个赛车数据分析APP：Harry's Laptimer和Racechrono

***

## 开发说明

> 设备名：RaceHF_[0-9,A-Z,a-z]{1,4} (_后面表示1~4个字母或数字)  
> 数据和控制 Service UUID：AAA0  
> 定位数据 Characteristic UUID：AAA1  
> 模式设置 Characteristic UUID：AAA2  
> 系统状态 Characteristic UUID：AAA3  
> 参数设置 Characteristic UUID：AAA4  

Bean产品为了兼容更广泛的APP开发者，使用兼容**蓝牙4.0**协议通讯(MTU=23)，
定位数据包内容固定为**20字节**。

## 定位数据协议

GPS部分包含从GPS中解析得到的数据，
数据主要从NMEA格式中读取包括经纬度、时间、速度等。  
GPS数据包含的内容太多，**蓝牙4.0**协议(MTU=23)无法一次承载全部信息，
所以把GPS信息分解到两条数据包发送，用不同的index(0x10,0x11)区分。

Index | Type        | Comment
---   | ---         | ---
0x10  | GPS Part.1  | 经度 / 纬度 / 海拔高度 / 定位模式
0x11  | GPS part.2  | UNIX时间戳 / 毫秒  / 速度 / 方位角 / HDOP / 锁定卫星数

### GPS Part.1

GPS Part.1包含 **经度** / **纬度** / **海拔高度** / **定位模式**
数据排列方式如下，没有内存对齐：

Byte Index | Content             | Type(bytes) | Comment
---        | ---                 | ---         | ---
0          | Index               | byte(1)     | = 0x10，表示该数据包是GPS Part.1
1          | longitude           | double(8)   | 经度，例如：12.12345678 或 123.12345678 单位：度
9          | latitude            | double(8)   | 纬度，例如：12.12345678 or -12.12345678 单位：度
17         | altitude            | int16(2)    | 海拔高度，**整数**类型，单位：米
19         | fix quality         | byte(1)     | 定位模式，0:未定位，1:2D定位，2:3D定位，4:3D差分定位

C语言代码如下：

```C
/* GNSS Pack.1 数据 */
#pragma pack(1)
typedef struct
{
  uint8_t     index;
  double      longitude;      /* 经度 */
  double      latitude;       /* 纬度 */
  int16_t     height;         /* 海拔 */
  uint8_t     locatemode;     /* 定位方式 */
} stuGPSPack1;
#pragma pack()
```

### GPS Part.2

GPS Part.2包含 **UNIX时间戳** / **毫秒**  / **速度** / **方位角** / **HDOP** / **锁定卫星数**  
数据排列方式如下，没有内存对齐：

Byte Index | Content             | Type(bytes) | Comment
---        | ---                 | ---         | ---
0          | Index               | byte(1)     | = 0x11，表示该数据包是GPS Part.2
1          | unix TimeStamp      | uint32(4)   | UNIX时间戳，从*1970/1/1*以UTC-0作为时间起点
5          | millsecond          | uint16(2)   | 毫秒数，例如： 0, 100 ,200, ... 900 或者更高经度: 50, 150, 950等
7          | speed               | float(4)    | 速度，总是大于等于0, 单位：km/h
11         | direction           | float(4)    | 方位角，单位：度
15         | hdop                | float(4)    | 水平定位因子，表示水平定位精度因数
19         | tracked satellites  | byte(1)     | 锁定卫星数量，没有GSA情况下最大是12，否则是24

C语言代码如下：

```C
/* GNSS Pack.2 数据 */
#pragma pack(1)
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
```

### 说明

> 定位数据：AAA1  
> 开放权限：Notify  

将0x10、0x11两条完整的共40字节称为一组数据。
设备发送一组数据一定是0x10在前0x11在后，APP需要根据原始数据还原出对应的内容。  
*有小概率APP接收不到完整的一组数据包，直接丢弃整组即可*

### 示例

如收到如下两条共一组数据包：  

```Data
0x10 0x72 0x24 0x44 0xB7 0xE6 0xC7 0x5E 0x40 0x91 0x91 0xBB 0x21 0xF0 0x74 0x37 0xC0 0x7B 0x00 0x03
0x11 0x39 0x8B 0x7C 0x5D 0x5E 0x01 0x14 0xAE 0xE0 0x42 0xFA 0x3E 0xF6 0x42 0x52 0xB8 0x9E 0x3F 0x12
```

从第一包中可以解出：

> Index = 0x10  
> 经度原始数据为：0x72 0x24 0x44 0xB7 0xE6 0xC7 0x5E 0x40   => 经度 = 123.12345678  
> 纬度原始数据为：0x91 0x91 0xBB 0x21 0xF0 0x74 0x37 0xC0   => 纬度 = -23.45678912  
> 海拔原始数据为：0x7B 0x00  => 海拔 = 123米  
> 定位模式原始数据为：0x03  => DGPS+3D模式  

从第二包中可以解出：

> Index = 0x11  
> UNIX时间戳原始数据为：0x39 0x8B 0x7C 0x5D  => 时间戳：1568443193  
> 毫秒数原始数据为：0x5E 0x01  => 毫秒数：350  
> 速度原始数据为：0x14 0xAE 0xE0 0x42  => 速度：112.34km/h  
> 方位角原始数据为：0xFA 0x3E 0xF6 0x42  => 方位角：123.123  
> HDOP原始数据为：0x52 0xB8 0x9E 0x3F  => HDOP：1.24  
> 锁定卫星数原始数据为：0x12  => 锁定卫星数量：18颗  

**参考代码：[gps.c](gps.c)**

***

## 模式设置协议

> 模式设置：AAA2  
> 开放权限：Write Without Response，Read，Notify  

设置*SD卡开始记录触发方式*，*SD卡开始记录触发速度*，*SD卡停止记录超时时间*，*SD卡记录文件类型*和*SD卡记录文件时区*。

程序中C语言结构体如下：

```C
struct content
{
  uint8_t   sdcard_record_trigger;        /* SD卡开始记录触发方式 */
  uint8_t   sdcard_record_start_speed;    /* SD卡开始记录触发速度 */
  uint8_t   sdcard_record_stop_timeout;   /* SD卡停止记录超时时间 */
  uint8_t   sdcard_record_filetype;       /* SD卡记录文件类型 */
  int8_t    sdcard_record_timezone;       /* SD卡记录文件时区 */
};
```

读可以直接获取结构体存储的所有信息。  
设置需要发送配置项ID和参数内容，指令格式是：id+param  
设置完毕后会回应一条结构体存储的所有信息。

示例：
> 读到内容：0x41 0x42 0x43 0x00 0x03 0x0A 0x14 0x00 0x08  
> 表示内容为：  
> - 用户ID="ABC"  
> - 速度触发SD卡记录  
> - 触发记录速度为10km/h  
> - 触发停止记录超时时间为20秒  
> - 记录文件格式为VBO文件  
> - 时区为东8区  

**SD卡开始记录触发方式**  
sdcard_record_trigger：配置id = 0x11  
触发SD卡记录条件：0：关闭记录；1：手动开始记录；2：开机自动记录；3：速度触发记录  
> 0：无论如何都不会触发记录SD卡  
> 1：强制开启SD卡记录，限制条件：SD卡插入，GPS定位成功，关机后自动清0  
> 2：开机自动SD卡记录，限制条件：SD卡插入，GPS定位成功，关机后不清0  
> 3：速度触发SD卡记录，限制条件：SD卡插入，GPS定位成功，关机后不清0  

示例：
> 手动记录开始：写 0x11 0x01  
> 手动记录停止：写 0x11 0x00  
> 开机自动记录：写 0x11 0x02  
> 速度触发记录：写 0x11 0x03  

其中速度触发与sdcard_record_start_speed、sdcard_record_stop_timeout有关，用于设置3的触发条件

**SD卡开始记录触发速度**  
sdcard_record_start_speed：配置index = 0x12  
设置触发SD卡记录的触发速度，范围为{5,200}

示例：
> 设置触发速度为10km/h：0x12 0x0A  
> 设置触发速度为60km/h：0x12 0x3C  

**SD卡停止记录超时时间**  
sdcard_record_stop_timeout：配置index = 0x13  
设置触发SD卡记录停止的超时时间，当速度小于1km/h的时间超过sdcard_record_stop_timeout后停止SD卡记录，范围为{10,200}

示例：
> 设置超时时间为15秒：0x13 0x0F  
> 设置超时时间为120秒：0x13 0x78  

**SD卡记录文件类型**  
sdcard_record_filetype：配置index = 0x14  
设置记录文件类型。0：VBO文件；1：RHF文件

示例：
> 设置记录成vbo文件：0x14 0x00  
> 设置记录成rhf文件：0x14 0x01  

**SD卡记录文件时区**  
sdcard_record_timezone：配置index = 0x15  
设置时区：范围为{-12,12}

示例：
> 设置成东八区：0x15 0x08  
> 设置成西四区：0x15 0xFC  

**设备控制命令**  
配置index = 0xA0  

示例：
> 从OAD状态恢复：0xA0 0x01  
> 设备关机：0xA0 0x02  
> 设备重启：0xA0 0x03  

***

## 有问题反馈

在开发中有任何问题，欢迎反馈给我

- 邮件：[yuanxiaochen1995@gmail.com](yuanxiaochen1995@gmail.com)
- facebook： [@弁杓](https://www.facebook.com/profile.php?id=100015307727134)
