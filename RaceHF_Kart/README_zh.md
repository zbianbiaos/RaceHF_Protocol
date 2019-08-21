# RaceHF Kart 协议

## 概述

\[[English](README.md)\]
[[中文](README_zh.md)\]

Bean项目是RaceHF下的一个面向专业赛车的产品，提供更专业的数据显示方式和专业数据分析的支持，提供更为直接的驾驶行为分析。

产品主要特点：

- **M4内核**高性能处理器
- **BT+BLE+WIFI**融合通讯方式.
- 优质定位芯片，最高真实刷新率可达**20hz**，专为赛车应用场景优化算法。兼容GPS/GLONASS，内置高灵敏度陶瓷天线，可外接天线
- 支持**空中固件升级**
- 支持**SD卡**记录VBO格式文件，把记录的数据导入CTool Track attack等主流的赛车数据分析软件
- 实现开机即用，不再依赖手机
- 开放的数据通讯协议，第三方软件开发商快速开发和硬件兼容的APP
- 支持Andriod与IOS
- 支持全球使用最广泛的两个赛车数据分析APP：*Harry's Laptimer*（仅IOS）和*Racechrono*

***

## GPS数据协议

> 广播名&设备名：       RaceHF_XXXX (XXXX表示4个十六进制数)  
> Service UUID：       0000ABF0-0000-1000-8000-00805F9B34FB  
> Characteristic UUID：0000ABF1-0000-1000-8000-00805F9B34FB

Kart产品兼容**蓝牙4.2**协议(MTU_MAX=517)，数据包固定为**80字节**一包，
数据包第一个字节(index)表示该数据包类型。

Index | Type           | Comment
---   | ---            | ---
0x11  | GPS            | UNIX时间戳 / 毫秒 / 经度 / 纬度 / 速度 / 方位角 / HDOP / 海拔高度 / 锁定卫星数 / 定位模式
0x21  | Engine.Rpm     | UNIX时间戳 / 毫秒 / 发动机转速时间间隔 / 转速有效数据个数 / 发动机转速数组
0x21  | Engine.Temper  | UNIX时间戳 / 毫秒 / 冷却液温度 / 缸盖温度 / 排气管温度
0xA1  | Device         | 电池电量
0x80  | Meaningless    | 无意义数据包

### GPS

GPS部分包含从GPS中解析得到的数据，
数据主要从NMEA格式中读取包括经纬度、时间、速度等。  
按照指定格式排列到结构体中，无内存对齐。

GPS 包含 **UNIX时间戳** / **毫秒**  / **经度** / **纬度** / **速度** / **方位角** / **HDOP** / **海拔高度** / **锁定卫星数** / **定位模式**  
数据排列方式如下：

Byte Index | Content             | Type(bytes) | Comment
---        | ---                 | ---         | ---
0          | Index               | byte(1)     | = 0x11，表示该数据包是GPS
1          | unix timestamp      | uint32(4)   | UNIX时间戳，从*1970/1/1*以UTC-0作为时间起点
5          | millsecond          | uint16(2)   | 毫秒数，例如： 0, 100 ,200, ... 900 或者更高经度: 50, 150, 950等
7          | longitude           | double(8)   | 经度，例如：12.12345678 或 123.12345678 单位：度
15         | latitude            | double(8)   | 纬度，例如：12.12345678 or -12.12345678 单位：度
23         | speed               | float(4)    | 速度，总是大于等于0, 单位：km/h
27         | direction           | float(4)    | 方位角，单位：度
31         | hdop                | float(4)    | 水平定位因子，表示水平定位精度因数
35         | altitude            | int16(2)    | 海拔高度，**整数**类型，单位：米
37         | tracked satellites  | byte(1)     | 锁定卫星数量，没有GSA情况下最大是12，否则是24
38         | fix quality         | byte(1)     | 定位模式，0:未定位，1:2D定位，2:3D定位，4:3D差分定位

### Engine

#### Engine.Rpm

Engine.Rpm 包含 **UNIX时间戳** / **毫秒** / **发动机转速时间间隔** / **转速有效数据个数** / **发动机转速数组**  
数据排列方式如下：

Byte Index | Content             | Type(bytes) | Comment
---        | ---                 | ---         | ---
0          | Index               | byte(1)     | = 0x21，表示该数据包是GPS
1          | unix timestamp      | uint32(4)   | UNIX时间戳，从*1970/1/1*以UTC-0作为时间起点
5          | millsecond          | uint16(2)   | 毫秒数，例如： 0, 100 ,200, ... 900 或者更高经度: 50, 150, 950等
7          | rpm cap interval    | uint16(2)   | 转速采集间隔 单位：毫秒
9          | rpm count           | uint16(2)   | 转速数组中有效转速数据个数
11         | rpm array           | uint16(2*n) | 转速数组

> 发动机转速数组说明：  
> 为了降低空中数据包传输量，所以把从某个时刻的多个转速数据压缩到一个数据包中发送出来，  
> rpm cap interval表示转速每次采集的时间间隔。

#### Engine.Temper

Engine.Temper 包含 **UNIX时间戳** / **毫秒**  / **冷却液温度** / **缸盖温度** / **排气管温度**  
数据排列方式如下：

Byte Index | Content             | Type(bytes) | Comment
---        | ---                 | ---         | ---
0          | Index               | byte(1)     | = 0x21，表示该数据包是GPS
1          | unix timestamp      | uint32(4)   | UNIX时间戳，从*1970/1/1*以UTC-0作为时间起点
5          | millsecond          | uint16(2)   | 毫秒数，例如： 0, 100 ,200, ... 900 或者更高经度: 50, 150, 950等
7          | water temp          | float(4)    | 冷却液温度 单位：摄氏度
11         | cylinder head temp  | float(4)    | 缸盖温度 单位：摄氏度
15         | exhaust gas temp    | float(4)    | 排气管温度 单位：摄氏度

### Device

Bean设备信息包含 **电池电量**  
数据排列方式如下：

Byte Index | Content             | Type(bytes) | Comment
---        | ---                 | ---         | ---
0          | Index               | byte(1)     | = 0xA1，表示该数据包是Device
1          | battery percent     | int8(1)     | 电池电量百分比，电量错误-1

***

## 有问题反馈

在开发中有任何问题，欢迎反馈给我

- 邮件：[yuanxiaochen1995@gmail.com](yuanxiaochen1995@gmail.com)
- facebook： [@弁杓](https://www.facebook.com/profile.php?id=100015307727134)
