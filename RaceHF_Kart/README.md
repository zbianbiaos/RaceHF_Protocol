# RaceHF Kart Protocol

## Overview

\[[English](README.md)\]
[[中文](README_zh.md)\]

## GPS Packet Protocol

> AdvData & DevName:   RaceHF_XXXX (XXXX represents 4 hexadecimal numbers)  
> Service UUID：       0000ABF0-0000-1000-8000-00805F9B34FB  
> Characteristic UUID：0000ABF1-0000-1000-8000-00805F9B34FB

Kart product is compatible with **Bluetooth 4.2** protocol (MTU_MAX=517),
the data packet is fixed to **80 bytes** The first byte of a packet indicates the packet type.

Index | Type        | Comment
---   | ---         | ---
0x11  | GPS         | UNIX Timestamp / Millsecond  / Longitude / Latitude / Speed / Track angle / HDOP / Altitude / Tracked Satellites / Fix quality
0x21  | Engine      | UNIX Timestamp / Millsecond  / Water Temperature / Cylinder Head Temperature / Exhaust Gas Temperature / RPM Capture Interval / Valid RPM count in RPM Array / RPM Array
0xA1  | Device      | Battery percentage
0x80  | Meaningless | Meaningless Packet

### GPS Part

The GPS part contains data parsed from GPS.  
The data is mainly read from the NMEA format including latitude and longitude, time, speed, and the like.  
The data arranged into the structure in the specified format, no memory alignment.

GPS contains **UNIX Timestamp** / **millsecond**  / **Longitude** / **Latitude** / **Speed** / **Track Angle** / **HDOP** / **Altitude** / **Tracked Satellites**  / **Fix quality**  
The data is arranged as follows：

Byte Index | Content             | Type(bytes) | Comment
---        | ---                 | ---         | ---
0          | Index               | byte(1)     | = 0x11，Indicates that the packet is GPS
1          | unix timestamp      | uint32(4)   | Seconds from 1970/1/1 UTC-0 to the present
5          | millsecond          | uint16(2)   | eg: 0, 100 ,200, ... 900 or higher precision like: 50, 150,950,etc..
7          | longitude           | double(8)   | eg: 12.12345678 or 123.12345678 unit:degree
15         | latitude            | double(8)   | eg: 12.12345678 or -12.12345678 unit:degree
23         | speed               | float(4)    | Always >= 0, unit:km/h
27         | direction           | float(4)    | unit:degree
31         | hdop                | float(4)    | Horizontal dilution of position
35         | altitude            | int16(2)    | **Integer** Type unit:meter
37         | tracked satellites  | byte(1)     | The maximum is 12 without GSA, otherwise the maximum is 24
38         | fix quality         | byte(1)     | 0:invalid, 1:GPS fix(2D)，2:GPS fix(3D)，4:DGPS fix

### Engine

Engine contains **UNIX Timestamp** / **millsecond**  / **Water Temperature** / **Cylinder Head Temperature** / **Exhaust Gas Temperature** / **RPM Capture Interval** / **Valid RPM count in RPM Array** / **RPM Array**  
The data is arranged as follows：

Byte Index | Content             | Type(bytes) | Comment
---        | ---                 | ---         | ---
0          | Index               | byte(1)     | = 0x11，Indicates that the packet is Engine
1          | unix timestamp      | uint32(4)   | Seconds from 1970/1/1 UTC-0 to the present
5          | millsecond          | uint16(2)   | eg: 0, 100 ,200, ... 900 or higher precision like: 50, 150,950,etc..
7          | water temp          | float(4)    | WAT unit:celsius
11         | cylinder head temp  | float(4)    | CHT unit:celsius
15         | exhaust gas temp    | float(4)    | EGT unit:celsius
19         | rpm cap interval    | uint16(2)   | rpm capture interval unit:ms
21         | rpm count           | uint16(2)   | indicate valid rpms in rpm array
23         | rpm array           | uint16(2*n) | rpm array

> Engine speed array description:  
> In order to reduce the amount of air packet transmission, the rpms from a certain moment is compressed and sent to a data packet.  
> rpm cap interval indicates the time interval for each acquisition of the rpm.

### Device

Kart contains **Battery percentage**  
The data is arranged as follows :

Byte Index | Content             | Type(bytes) | Comment
---        | ---                 | ---         | ---
0          | Index               | byte(1)     | = 0xA0，Indicates that the packet is device information
1          | battery percent     | int8(1)     | Battery power percentage, error is -1

***

## Feedback

If you have any questions during the development process, please contact us.

- Email: [yuanxiaochen1995@gmail.com](yuanxiaochen1995@gmail.com)
- facebook: [@弁杓](https://www.facebook.com/profile.php?id=100015307727134)
