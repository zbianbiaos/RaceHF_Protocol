# RaceHF Kart Protocol

## Overview

\[[English](README.md)\]
[[中文](README_zh.md)\]

> AdvData & DevName:   RaceHF_XXXX (XXXX represents 4 hexadecimal numbers)  
> Service UUID：       0000ABF0-0000-1000-8000-00805F9B34FB  
> Characteristic UUID：0000ABF1-0000-1000-8000-00805F9B34FB

Kart product is compatible with **Bluetooth 4.2** protocol (MTU_MAX=517),
the data packet is fixed to **80 bytes**.

## Active Packet Protocol

The content of the Active Packet Protocol  is the device  sends data  by BLE notification periodically and actively.  
The first byte of the packet notification represents the packet type.

Index | Type            | Comment
---   | ---             | ---
0x11  | GPS             | UNIX Timestamp / Millsecond / Longitude / Latitude / Speed / Track Angle / HDOP / Altitude / Tracked Satellites / Fix Quality
0x21  | Engine.Rpm      | UNIX Timestamp / Millsecond / RPM Capture Interval / Number of RPM Valid Data   / RPM Array
0x22  | Engine.Temper   | UNIX Timestamp / Millsecond / Water Temperature / Cylinder Head Temperature / Exhaust Gas Temperature
0xA1  | Device          | Battery Percentage
0x80  | Meaningless     | Meaningless Packet

### GPS Part

The GPS Part contains data parsed from GPS.  
The data is mainly obtained from the NMEA including latitude, longitude, time, speed, etc. 
The data is arranged into the structure according to the specified format, with no memory alignment.

GPS contains **UNIX Timestamp** / **Millsecond**  / **Longitude** / **Latitude** / **Speed** / **Track Angle** / **HDOP** / **Altitude** / **Tracked Satellites**  / **Fix Quality**  
The data is arranged as follows：

Byte Index | Content             | Type(bytes) | Comment
---        | ---                 | ---         | ---
0          | Index               | byte(1)     | = 0x11，indicates that the packet is GPS
1          | unix timestamp      | uint32(4)   | begins with 1970/1/1 
5          | millsecond          | uint16(2)   |eg: 0, 100 ,200, ... 900 or higher like: 50, 150,950,etc..
7          | longitude           | double(8)   | eg: 12.12345678 or 123.12345678 unit:degree
15         | latitude            | double(8)   | eg: 12.12345678 or -12.12345678 unit:degree
23         | speed               | float(4)    | Always >= 0, unit:km/h
27         | direction           | float(4)    | unit:degree
31         | hdop                | float(4)    | Horizontal dilution of position
35         | altitude            | int16(2)    | **Integer** Type unit:meter
37         | tracked satellites  | byte(1)     | The maximum is 12 without GSA, otherwise the maximum is 24
38         | fix quality         | byte(1)     | 0:invalid, 1:GPS fix(2D)，2:GPS fix(3D)，4:DGPS fix

### Engine

#### Engine.Rpm

Engine.Rpm contains **UNIX Timestamp** / **Millsecond** / **RPM Capture Interval** / **Count of RPM Valid Data  ** / **RPM Array**  
The data is arranged as follows：

Byte Index | Content             | Type(bytes) | Comment
---        | ---                 | ---         | ---
0          | Index               | byte(1)     | = 0x21，Indicates that the packet is Engine.Rpm
1          | unix timestamp      | uint32(4)   |  begins with 1970/1/1 
5          | millsecond          | uint16(2)   | eg: 0, 100 ,200, ... 900 or higher like: 50, 150,950,etc..
7          | rpm cap interval    | uint16(2)   | rpm capture interval unit:ms
9          | rpm count           | uint16(2)   | indicates count of RPM Valid Data
11         | rpm array           | uint16(2*n) | rpm array

> Engine speed array description:  
> In order to reduce the amount of air packet transmission, the rpms at certain moment is compressed to a data packet to be sent.  
> rpm cap interval indicates the time interval for each acquisition of the rpm.

#### Engine.Temper

Engine.Temper contains **UNIX Timestamp** / **Millsecond**  / **Water Temperature** / **Cylinder Head Temperature** / **Exhaust Gas Temperature**  
The data is arranged as follows：

Byte Index | Content             | Type(bytes) | Comment
---        | ---                 | ---         | ---
0          | Index               | byte(1)     | = 0x22，Indicates that the packet is Engine.Temper
1          | unix timestamp      | uint32(4)   |begins with 1970/1/1 
5          | millsecond          | uint16(2)   | eg: 0, 100 ,200, ... 900 or higher like: 50, 150,950,etc..
7          | water temp          | float(4)    | WAT unit:celsius
11         | cylinder head temp  | float(4)    | CHT unit:celsius
15         | exhaust gas temp    | float(4)    | EGT unit:celsius

### Device

The RaceHF_Kart contains **Battery percentage**  
The data is arranged as follows :

Byte Index | Content             | Type(bytes) | Comment
---        | ---                 | ---         | ---
0          | Index               | byte(1)     | = 0xA1，Indicates that the packet is device information
1          | battery percent     | int8(1)     | Battery power percentage, error is -1

***

## Passive Packet Protocol

***

## Feedback

If you have any question during the development process, please contact us.

- Email: [yuanxiaochen1995@gmail.com](yuanxiaochen1995@gmail.com)
- facebook: [@弁杓](https://www.facebook.com/profile.php?id=100015307727134)
