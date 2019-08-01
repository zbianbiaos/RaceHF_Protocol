# RaceHF Bean 协议

## 概述

Bean项目是RaceHF下的一个面向大众赛车的产品，产品主要服务于业余赛车爱好者，为广大车友们提供一个融合软硬件平台的优质服务产品。

产品主要特点：

- **32位超低功耗**嵌入式处理器
- 最大兼容**蓝牙5.0**协议
- 优质定位芯片，最高真实刷新率可达**20hz**，专为赛车应用场景优化算法。兼容GPS/GLONASS，内置高灵敏度陶瓷天线，可外接天线。
- 支持空中固件升级
- 支持SD卡记录VBO格式文件，把记录的数据导入CTool Track attack等主流的赛车数据分析软件，实现开机即用，不再依赖手机
- 开放的数据通讯协议，第三方软件开发商快速开发和硬件兼容的APP。
- 支持Andriod与IOS
- 支持全球使用最广泛的两个赛车数据分析APP：Harry's Laptimer和Racechrono

***

## GPS数据协议

Bean产品兼容**蓝牙4.0**协议(MTU=23)，数据包固定为**20字节**一包  
数据包第一个字节(index)表示该数据包类型

Index | Type        | Comment
---   | ---         | ---
0x10  | GPS Part.1  | 经度 / 纬度 / 海拔高度 / 定位模式
0x11  | GPS part.2  | UNIX时间戳 / 毫秒  / 速度 / 方位角 / HDOP / 锁定卫星数
0xA1  | Device      | 硬件版本 / 软件版本 / 电池电量

### GPS Part

GPS部分包含从GPS中解析得到的数据，
数据主要从NMEA格式中读取包括经纬度、时间、速度等。  
GPS数据包含的内容太多，**蓝牙4.0**协议(MTU=23)无法一次承载全部信息，
所以把GPS信息分解到两条数据包发送，用不同的index(0x10,0x11)区分。

#### GPS Part.1

GPS Part.1包含 **经度** / **纬度** / **海拔高度** / **定位模式**
数据排列方式如下：

Byte Index | Content             | Type(bytes) | Comment
---        | ---                 | ---         | ---
0          | Index               | byte(1)     | = 0x10，表示该数据包是GPS Part.1
1          | longitude           | double(8)   | 经度，double类型精确到小数点后8位
9          | latitude            | double(8)   | 纬度，double类型精确到小数点后8位
17         | altitude            | int16(2)    | 海拔高度，**整数**类型
19         | locate mode         | byte(1)     | 定位模式，0:未定位，1:2D定位，2:3D定位，4:3D差分定位

#### GPS Part.2

GPS Part.2包含 **UNIX时间戳** / **毫秒**  / **速度** / **方位角** / **HDOP** / **锁定卫星数**
数据排列方式如下：

Byte Index | Content             | Type(bytes) | Comment
---        | ---                 | ---         | ---
0          | Index               | byte(1)     | = 0x11，表示该数据包是GPS Part.2
1          | Unix TimeStamp      | uint32(4)   | UNIX时间戳，从*1970/1/1*以UTC-0作为时间起点
5          | millsecond          | uint16(2)   | 毫秒数，以精确毫秒数表示，如100表示时间戳下的100毫秒
7          | speed               | float(4)    | 速度，以KM/H表示的速度，数值始终为正
11         | direction           | float(4)    | 方位角，运动方向与正北方向按逆时针方向旋转的角度
15         | HDOP                | float(4)    | 水平定位因子，表示水平定位精度因数
19         | locked satellites   | byte(1)     | 锁定卫星数量，没有GSA情况下最大是12

### Device

Bean设备信息包含 **硬件版本** / **软件版本** / **电池电量**  
数据排列方式如下：

Byte Index | Content             | Type(bytes) | Comment
---        | ---                 | ---         | ---
0          | Index               | byte(1)     | = 0xA0，表示该数据包是Device
1          | hardware version    | uint32(4)   | 硬件版本，参考下方说明
5          | software version    | uint32(4)   | 软件版本，参考硬件版本说明
9          | battery percent     | int8(1)     | 电池电量百分比，电量错误-1

> Hardware Version 使用4字节表示硬件版本，4字节中每个字节表示一个版本主次信息  
> 示例如下：
> 0x01020304可以拆分成0x01 0x02 0x03 0x04:  
> 表示版本信息是V1.2.3.4

## 有问题反馈

在开发中有任何问题，欢迎反馈给我

- 邮件：[yuanxiaochen1995@gmail.com](yuanxiaochen1995@gmail.com)
- facebook： [@弁杓](https://www.facebook.com/profile.php?id=100015307727134)
