# RaceHF Bean Protocol

\[[中文](README_zh.md)\]

**-> [Overview](#Overview)**  
**-> [Development instructions](#Development-instructions)**  
**-> [Positioning Data Protocol](#Positioning-Data-Protocol)**  
**-> [Mode Setting Protocol](#Mode-Setting-Protocol)**  
**-> [System Status](#System-Status)**  
**-> [Parameter Configuration](#Parameter-Configuration)**  
**-> [Feedback](#Feedback)**  

## Overview

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
> **Endianness Mode: Little Endian**  
> ***
> Standard GATT Service：Battery Service、Device Information Service  
> Reference Link：
> *[service](https://www.bluetooth.com/specifications/gatt/services/)*, 
> *[characteristic](https://www.bluetooth.com/specifications/gatt/characteristics/)*


In order to be compatible with a wider range of APP developers, Bean products use compatible **Bluetooth 4.0** protocol communication (MTU=23),
The location of the location packet is fixed to **20 bytes**.

## Positioning Data Protocol

> Characteristic: AAA1  
> Access: Notify  

The GPS part contains data parsed from GPS.
The data is mainly read from the NMEA format including latitude and longitude, time, speed, and the like.  
The GPS data contains too much content. **Bluetooth 4.0** protocol (MTU=23) cannot carry all the information at once.
Therefore, the GPS information is decomposed into two data packets and transmitted with different indexes (0x10, 0x11).

Index | Type        | Comment
---   | ---         | ---
0x10  | GPS Part.1  | Longitude / Latitude / Altitude / Positioning mode
0x11  | GPS part.2  | UNIX Timestamp / millsecond  / Speed / Track Angle / HDOP / Tracked Satellites Number

### GPS Part.1

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

### GPS Part.2

GPS Part.2 contains **UNIX Timestamp** / **millsecond**  / **Speed** / **Track Angle** / **HDOP** / **Tracked Satellites Number**  
The data is arranged as follows：

Byte Index | Content             | Type(bytes) | Comment
---        | ---                 | ---         | ---
0          | Index               | byte(1)     | = 0x11，Indicates that the packet is GPS Part.2
1          | unix TimeStamp      | uint32(4)   | Seconds from 1970/1/1 UTC-0 to the present
5          | millsecond          | uint16(2)   | eg: 0, 100, 200, ... 900 or higher precision like: 50, 150, 950, etc..
7          | speed               | float(4)    | Always >= 0, unit:km/h
11         | direction           | float(4)    | unit:degree
15         | hdop                | float(4)    | Horizontal dilution of position
19         | tracked satellites  | byte(1)     | 

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

## Mode Setting Protocol

> Characteristic: AAA2  
> Access: Write Without Response, Read, Notify  

Set auto shutdown timeout duration, trigger mode of file recording start, file record type and file record file time-zone.

The C language structure in the program is as follows:

```C
struct
{
  uint8_t   autooff_timeout;       /* auto shutdown timeout */
  uint8_t   file_record_trigger;   /* starts recording trigger mode */
  uint8_t   file_record_filetype;  /* file type */
  int8_t    file_record_timezone;  /* time-zone */
} content;
```

Read this characteristic, you can retrieve the content of all the configuration items listed above.  
You need to send the relevant configuration id to set the relevant item of this article, whose format is: id+param.  
Once set up, a notification with all content will be sent.  

Example:
> Read content: 0x02 0x03 0x00 0x08  
> indicates that the content is:  
> - Auto shutdown timeout is 2 minutes
> - Manual switch file record
> - Record file format is VBO file
> - Time-zone is East 8th District

**Automatic shutdown timeout duration**  
Autooff_timeout: configuration id = 0x01  
Set the automatic shutdown timeout duration: the unit is minutes. When the speed is less than 10km/h and the Bluetooth is not connected, the automatic shutdown timeout timer is triggered. The value range is {0,255}.  
When set to 0, it means no automatic shutdown.

Example:
> Settings do not automatically shutdown: 0x01 0x00  
> Set timeout for 10 minutes to automatically shutdown: 0x01 0x0A  

**trigger mode of file recording start**  
file_record_trigger: configuration id = 0x11  
Trigger file record conditions: 0: Auto Mode; 1: Manual Mode;
> 0: Recording SD card will not be triggered anyway.  
> 1: The SD card record is forced to be opened. Limitations: SD card is inserted, GPS is successfully located, and 0 will be automatically cleaned after shutdown.  
> 2: The SD card record is opened automatically. Limitations: SD card is inserted, GPS is successfully located, and 0 will not be automatically cleaned after shutdown.  
> 3: The SD card record is triggered by the speed. Limitations: SD card is inserted, GPS is successfully located, and 0 will not be automatically cleaned after shutdown.  

Example:
> Start recording manually: Write 0x11 0x01  
> Stop recording manually: Write 0x11 0x00  
> Start recording automatically: write 0x11 0x02  
> Trigger recording by the speed: Write 0x11 0x03  

The speed trigger is related to sdcard_record_start_speed and sdcard_record_stop_timeout, and is used to set the trigger condition of 3.

**SD card starts recording trigger speed**  
Sdcard_record_start_speed: Configure index = 0x12  
Set the trigger speed for triggering SD card recording, the range is {5,200}

Example:
> Set the trigger speed to 10km/h: 0x12 0x0A  
> Set the trigger speed to 60km/h: 0x12 0x3C  

**SD card stop recording timeout**  
Sdcard_record_stop_timeout: Configure index = 0x13  
Set the timeout that triggers the SD card to stop recording. When the speed is less than 1km/h and the time exceeds the timeout, the SD card recordwill be stopped. The range is {10,200}.

Example:
> Set the timeout to 15 seconds: 0x13 0x0F  
> Set the timeout to 120 seconds: 0x13 0x78  

**SD card record file type**  
Sdcard_record_filetype: Configure index = 0x14  
Set the record file type. 0: VBO file; 1: RHF file  

Example:
> Set the record to vbo file: 0x14 0x00  
> Set the record to rhf file: 0x14 0x01  

**SD card record file time-zone**  
Sdcard_record_timezone: Configure index = 0x15  
Set time-zone: range is {-12,12}  

Example:
> Set up into the East 8th District: 0x15 0x08  
> Set to the West 4th District: 0x15 0xFC  

**Device Control Command**  
Configure index = 0xA0  

Example:
> Recovery from OAD state: 0xA0 0x01  
> Device shutdown: 0xA0 0x02  
> Device restart: 0xA0 0x03  

***

## System Status

> Characteristic: AAA3  
> Access: Read  

AAA3 only allows reading of system status.
The notification is sent automatically when the system status changes.  
*In order to put more system information into a status content, the bit field feature is used*

The C language structure in the program is as follows:

```C
typedef struct
{
  uint8_t     battery_percent;
  
  uint8_t     charge_mode:1;
  uint8_t     connect_mode:1;
  uint8_t     oad_mode:1;
  uint8_t     sdcard_mode:2;
  
  uint8_t     :0;
  
  uint8_t     gps_lock:1;
  uint8_t     sdcard_lock:1;
} stuSYS;
```

> *battery_percent*:  
> Battery power percentage. Always 100 when charging  
>  
> *charge_mode*:  
> Is charging. 1: charging; 0: not charging  
>  
> *connect_mode*:  
> Bluetooth is connected. 1: Bluetooth is connected; 0: Bluetooth is not connected (APP reading should always be 1)  
>  
> *oad_mode*:  
> Whether it is in the firmware upgrade mode. 1: Firmware upgrade is in progress; 0: No firmware upgrade  
>  
> *sdcard_mode*:  
> SD card status. 0: The SD card is not detected; 1: The SD card has been detected, but the file has not been recorded yet; 2: The SD card is recording the file; 3: The SD card is running abnormally.  
>  
> *gps_lock*:  
> GPS resolution task lock (APP does not care)  
>  
> *sdcard_lock*:  
> SD card record lock (APP does not need to be managed)  

Example:
> Read the following data: 0x07 0x12 0x00  
> indicates that the content is:  
> - Battery power is 7%
> - not charging
> - Bluetooth is connected
> - not in firmware upgrade
> - SD card is recording files

Example:
> Read the following data: 0x39 0x0B 0x00  
> indicates that the content is:  
> - Battery power is 57%
> - Charging
> - Bluetooth is connected
> - not in firmware upgrade
> - SD card detected but file not started yet

*[link:bit field](https://en.wikipedia.org/wiki/Bit_field)*

***

## Parameter Configuration

> Characteristic: AAA4  
> Access: Write Without Response, Read, Notify  

Used to configure or read parameters in the system.  
Read parameter method: directly send the index of the corresponding parameter with one byte zero will immediately respond to the corresponding content from Notify, or read the corresponding content from Read later.  
Set parameter method: similar to AAA2, use index+length+param mode, it will respond to the set parameters immediately after setting, and Read can also read the set parameters.  
The first two bytes of the read or set content are the index and payload lengths, and the second byte of the read parameter is written to 0x00.  

**User ID**  
Id = 0x01, readable and writable  
String type indicating the XXXX part of the device name RaceHF_XXXX  

Example:
> Get User ID:  
> Write: 0x01 0x00  
> Receive notification immediately: 0x01 0x04 0x59 0x58 0x43 0x00  
> Or read: 0x01 0x04 0x59 0x58 0x43 0x00  
> Indicates that the user ID is "YXC"  
>  
> Set User ID:  
> Write: 0x01 0x03 0x59 0x58 0x43  
> Receive notification immediately: 0x01 0x04 0x59 0x58 0x43 0x00  
> Or read: 0x01 0x04 0x59 0x58 0x43 0x00  
> Means to set the user ID to "YXC"  

**Device Model**  
Id = 0x02, read only  
String type, return the device model  

Example:
> Get the device model:  
> Write: 0x02 0x00  
> Receive notification immediately: 0x02 0x0B 0x52 0x61 0x63 0x48 0x46 0x20 0x42 0x65 0x61 0x6E  
> Or read: 0x02 0x0B 0x52 0x61 0x63 0x48 0x46 0x20 0x42 0x65 0x61 0x6E  
> Indicates that the device model is "RaceHF_Bean"  

**Hardware Version**  
Id = 0x03, read only  
String type, return the device hardware version  

**Software Version**  
Id = 0x04, read only  
String type, return device software version  

**Device Identification**  
Id = 0x05, read only  
Byte array type, Returns a byte array of device IDs  

Example:
> Get Device Identification:  
> Write: 0x05 0x00  
> Receive notification immediately: 0x05 0x06 0x12 0x23 0x34 0x45 0x56 0x67  
> Or read: 0x05 0x06 0x12 0x23 0x34 0x45 0x56 0x67  
> Means Device ID = 12:23:34:45:56:67  

**Account ID**  
id = 0x11，readable and writable  
Long integer. Indicating the account ID registered by the user on the Internet

Example:
> Get Account ID:  
> Write: 0x11 0x00  
> Receive notification immediately: 0x01 0x04 0x01 0x12 0x00 0x00  
> Or read: 0x01 0x04 0x01 0x12 0x00 0x00  
> Indicates that the account id is 4609  
>  
> Set Account ID:  
> Write: 0x01 0x04 0x03 0x12 0x00 0x00  
> Receive notification immediately: 0x01 0x04 0x03 0x12 0x00 0x00  
> Or read: 0x01 0x04 0x03 0x12 0x00 0x00  
> Set the account id value at 4611  

**Satellites Locked Numbers**  
Id = 0xA1, read only  
Byte array type, return all number of positioning satellites / GPS / GLONASS / GALILEO.

Example:
> Get the number ofSatellites Locked Numbers:  
> Write: 0xA1 0x00  
> Receive notification immediately: 0xA1 0x04 0x0E 0x09 0x02 0x03  
> Or read: 0xA1 0x04 0x0E 0x09 0x02 0x03 0x6E  
> Indicates that the response is id=0xA1, the content length is 4 bytes, the number of positioning satellites is 14, the number of GPS satellites is 9, the number of GLONASS satellites is 2, and the number of GALILEO satellites is 3.  

## Feedback

If you have any questions during the development process, please contact us.

- Email: [yuanxiaochen1995@gmail.com](yuanxiaochen1995@gmail.com)
- facebook: [@弁杓](https://www.facebook.com/profile.php?id=100015307727134)
