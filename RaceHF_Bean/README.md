# RaceHF Bean Protocol

## Overview

\[[English](README.md)\]
[[中文](README_zh.md)\]

Bean is aimed at the majority of racing enthusiasts, providing a quality service product that integrates software and hardware platforms for the majority of racer.

Main features of the product:

- **32-bit ultra low power** embedded processor
- Support **Bluetooth 5.0** protocol
- High-quality positioning chip, the highest real refresh rate up to **20hz**, designed for racing application scene optimization algorithms. Compatible with GPS/GLONASS/GALILEO, built-in high sensitivity ceramic antenna, external antenna avaliable
- Support for air firmware upgrades
- Support SD card to record VBO format file, import recorded data into mainstream racing data analysis software such as CTool Track attack, realize boot and use, no longer rely on mobile phone
- Open data communication protocol, third-party software developers quickly develop and hardware-compatible APP
- Support for Android and IOS
- Supports the world's most widely used two racing data analysis apps: Harry's Laptimer and Racechrono

***

## Development instructions

> Device Name: RaceHF_[0-9,A-Z,a-z]{1,4} (_ behind indicates 1~4 letters or numbers)  
> Data and Control Service UUID: AAA0  
> Positioning Data Characteristic UUID: AAA1  
> Mode Setting Characteristic UUID: AAA2  
> System Status Characteristic UUID: AAA3  
> Parameter Setting Characteristic UUID: AAA4  

In order to be compatible with a wider range of APP developers, Bean products use compatible **Bluetooth 4.0** protocol communication (MTU=23),
The location of the location packet is fixed to **20 bytes**.

### Positioning Data Protocol

The GPS part contains data parsed from GPS.
The data is mainly read from the NMEA format including latitude and longitude, time, speed, and the like.  
The GPS data contains too much content. **Bluetooth 4.0** protocol (MTU=23) cannot carry all the information at once.
Therefore, the GPS information is decomposed into two data packets and transmitted with different indexes (0x10, 0x11).

Index | Type        | Comment
---   | ---         | ---
0x10  | GPS Part.1  | Longitude / Latitude / Altitude / Positioning mode
0x11  | GPS part.2  | UNIX Timestamp / millsecond  / Speed / Track Angle / HDOP / Tracked Satellites Count

#### GPS Part.1

GPS Part.1 contains **Longitude** / **Latitude** / **Altitude** / **Positioning mode**  
The data is arranged as follows, with no memory alignment:

Byte Index | Content             | Type(bytes) | Comment
---        | ---                 | ---         | ---
0          | Index               | byte(1)     | = 0x10，Indicates that the packet is GPS Part.1
1          | longitude           | double(8)   | eg: 12.12345678 or 123.12345678 unit:degree
9          | latitude            | double(8)   | eg: 12.12345678 or -12.12345678 unit:degree
17         | altitude            | int16(2)    | **Integer** Type unit:meter
19         | fix quality         | byte(1)     | 0:invalid, 1:GPS fix(2D)，2:GPS fix(3D)，4:DGPS fix

The C code is as follows:

```C
/* GNSS Pack.1 */
#pragma pack(1)
typedef struct
{
  uint8_t     index;
  double      longitude;
  double      latitude;
  int16_t     height;
  uint8_t     locatemode;
} stuGPSPack1;
#pragma pack()
```

#### GPS Part.2

GPS Part.2 contains **UNIX Timestamp** / **millsecond**  / **Speed** / **Track Angle** / **HDOP** / **Tracked Satellites Count**  
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

The C code is as follows:

```C
/* GNSS Pack.2 */
#pragma pack(1)
typedef struct
{
  uint8_t     index;
  uint32_t    seconds;
  uint16_t    milliseconds;
  float       speed;
  float       direction;
  float       hdop;
  uint8_t     satellites;
} stuGPSPack2;
#pragma pack()
```

### Description

> Positioning data: AAA1  
> Access: Notify  

Two complete 40 bytes of 0x10 and 0x11 are called a set of data.
The device sends a set of data must be 0x10 before the first 0x11, the APP needs to restore the corresponding content according to the original data.  
  *there is a small probability that the APP can not receive a complete set of data packets, and directly discard the entire group*

### Example

If you receive the following two groups of data packets:

```Data
0x10 0x72 0x24 0x44 0xB7 0xE6 0xC7 0x5E 0x40 0x91 0x91 0xBB 0x21 0xF0 0x74 0x37 0xC0 0x7B 0x00 0x03
0x11 0x39 0x8B 0x7C 0x5D 0x5E 0x01 0x14 0xAE 0xE0 0x42 0xFA 0x3E 0xF6 0x42 0x52 0xB8 0x9E 0x3F 0x12
```

Can be obtained from the first package：

> Index = 0x10  
> Longitude raw data is: 0x72 0x24 0x44 0xB7 0xE6 0xC7 0x5E 0x40 => longitude = 123.12345678  
> Latitude raw data is: 0x91 0x91 0xBB 0x21 0xF0 0x74 0x37 0xC0 => latitude = -23.45678912  
> Altitude raw data is: 0x7B 0x00 => altitude = 123 meters  
> Positioning mode raw data is: 0x03 => DGPS+3D mode  

Can be obtained from the second package：

> Index = 0x11  
> UNIX timestamp raw data is: 0x39 0x8B 0x7C 0x5D => timestamp: 1568443193  
> Milliseconds raw data is: 0x5E 0x01 => milliseconds: 350  
> Speed raw data is: 0x14 0xAE 0xE0 0x42 => speed: 112.34km / h  
> Direction raw data is: 0xFA 0x3E 0xF6 0x42 => direction: 123.123  
> HDOP raw data is: 0x52 0xB8 0x9E 0x3F => hdop: 1.24  
> Number of locked satellites is: 0x12 => satellites: 18  

**Reference code：[gps.c](gps.c)**

***

## Feedback

If you have any questions during the development process, please contact us.

- Email: [yuanxiaochen1995@gmail.com](yuanxiaochen1995@gmail.com)
- facebook: [@弁杓](https://www.facebook.com/profile.php?id=100015307727134)
