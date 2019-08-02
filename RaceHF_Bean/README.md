# RaceHF Bean Protocol

## Overview

\[[English](https://github.com/zbianbiaos/RaceHF_Protocol/blob/master/RaceHF_Bean/README.md)\]
[[中文](https://github.com/zbianbiaos/RaceHF_Protocol/blob/master/RaceHF_Bean/README_zh.md)\]

## GPS Packet Protocol

> AdvData & DevName:   Race_XXXX (XXXX represents 4 hexadecimal numbers)  
> Service UUID：       0000AAA0-0000-1000-8000-00805F9B34FB  
> Characteristic UUID：0000AAA1-0000-1000-8000-00805F9B34FB

Bean product is compatible with **Bluetooth 4.0** protocol (MTU=23),
the data packet is fixed to **20 bytes** The first byte of a packet indicates the packet type.

Index | Type        | Comment
---   | ---         | ---
0x10  | GPS Part.1  | Longitude / Latitude / Altitude / Fix quality
0x11  | GPS part.2  | UNIX Timestamp / millsecond  / speed / Track angle / HDOP / Tracked Satellites
0xA1  | Device      | Hardware Version / Software Version / Battery percentage

### GPS Part

The GPS part contains data parsed from GPS.
The data is mainly read from the NMEA format including latitude and longitude, time, speed, and the like.
The GPS data contains too much content. **Bluetooth 4.0** protocol (MTU=23) cannot carry all the information at once.
Therefore, the GPS information is decomposed into two data packets and transmitted with different indexes (0x10, 0x11).

#### GPS Part.1

GPS Part.1 contains **Longitude** / **Latitude** / **Altitude** / **Fix quality**  
The data is arranged as follows：

Byte Index | Content             | Type(bytes) | Comment
---        | ---                 | ---         | ---
0          | Index               | byte(1)     | = 0x10，Indicates that the packet is GPS Part.1
1          | longitude           | double(8)   | eg: 12.12345678 or 123.12345678 unit:degree
9          | latitude            | double(8)   | eg: 12.12345678 or -12.12345678 unit:degree
17         | altitude            | int16(2)    | **Integer** Type unit:meter
19         | fix quality         | byte(1)     | 0:invalid, 1:GPS fix(2D)，2:GPS fix(3D)，4:DGPS fix

#### GPS Part.2

GPS Part.2 contains **UNIX Timestamp** / **millsecond**  / **speed** / **Track angle** / **HDOP** / **Tracked Satellites**  
The data is arranged as follows：

Byte Index | Content             | Type(bytes) | Comment
---        | ---                 | ---         | ---
0          | Index               | byte(1)     | = 0x11，Indicates that the packet is GPS Part.2
1          | unix TimeStamp      | uint32(4)   | Seconds from 1970/1/1 UTC-0 to the present
5          | millsecond          | uint16(2)   | eg: 0, 100 ,200, ... 900 or higher precision like: 50, 150,950,etc..
7          | speed               | float(4)    | Always >= 0, unit:km/h
11         | direction           | float(4)    | unit:degree
15         | hdop                | float(4)    | Horizontal dilution of position
19         | tracked satellites  | byte(1)     | The maximum is 12 without GSA, otherwise the maximum is 24

### Device

Bean contains **Hardware Version** / **Software Version** / **Battery percentage**  
The data is arranged as follows：

Byte Index | Content             | Type(bytes) | Comment
---        | ---                 | ---         | ---
0          | Index               | byte(1)     | = 0xA0，Indicates that the packet is device information
1          | hardware version    | uint32(4)   | As follows below
5          | software version    | uint32(4)   | Reference hardware version
9          | battery percent     | int8(1)     | Battery power percentage, error is -1

> Hardware Version uses 4 bytes to represent the hardware version,
> and each byte in 4 bytes represents a version of the primary and secondary information.  
> For example：
> 0x01020304 can be split into 0x01 0x02 0x03 0x04  
> The hardware version is V1.2.3.4

***

## Feedback

If you have any questions during the development process, please contact us.

- Email: [yuanxiaochen1995@gmail.com](yuanxiaochen1995@gmail.com)
- facebook: [@弁杓](https://www.facebook.com/profile.php?id=100015307727134)
