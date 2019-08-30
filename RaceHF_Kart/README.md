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

The active data protocol is that the device periodically sends data periodically and sends it using BLE notification.  
The first byte of a packet indicates the packet type.

Index | Type            | Comment
---   | ---             | ---
0x10  | GPS             | UNIX Timestamp / Millsecond / Longitude / Latitude / Speed / Track angle / HDOP / Altitude / Tracked Satellites / Fix quality
0x20  | Engine.Rpm      | UNIX Timestamp / Millsecond / RPM Capture Interval / Valid RPM count in RPM Array / RPM Array
0x21  | Engine.Temper   | UNIX Timestamp / Millsecond / Water Temperature / Cylinder Head Temperature / Exhaust Gas Temperature
0x30  | Device          | Battery percentage
0x60  | Meaningless     | Meaningless Packet

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
38         | visible satellites  | byte(1)     | Number of visible satellites
39         | fix quality         | byte(1)     | 0:invalid, 1:GPS fix(2D)，2:GPS fix(3D)，4:DGPS fix

### Engine

#### Engine.Rpm

Engine.Rpm contains **UNIX Timestamp** / **millsecond** / **RPM Capture Interval** / **Valid RPM count in RPM Array** / **RPM Array**  
The data is arranged as follows：

Byte Index | Content             | Type(bytes) | Comment
---        | ---                 | ---         | ---
0          | Index               | byte(1)     | = 0x21，Indicates that the packet is Engine.Rpm
1          | unix timestamp      | uint32(4)   | Seconds from 1970/1/1 UTC-0 to the present
5          | millsecond          | uint16(2)   | eg: 0, 100 ,200, ... 900 or higher precision like: 50, 150,950,etc..
7          | rpm cap interval    | uint16(2)   | rpm capture interval unit:ms
9          | rpm count           | uint16(2)   | indicate valid rpms in rpm array
11         | rpm array           | uint16(2*n) | rpm array

> Engine speed array description:  
> In order to reduce the amount of air packet transmission, the rpms from a certain moment is compressed and sent to a data packet.  
> rpm cap interval indicates the time interval for each acquisition of the rpm.

#### Engine.Temper

Engine.Temper contains **UNIX Timestamp** / **millsecond**  / **Water Temperature** / **Cylinder Head Temperature** / **Exhaust Gas Temperature**  
The data is arranged as follows：

Byte Index | Content             | Type(bytes) | Comment
---        | ---                 | ---         | ---
0          | Index               | byte(1)     | = 0x22，Indicates that the packet is Engine.Temper
1          | unix timestamp      | uint32(4)   | Seconds from 1970/1/1 UTC-0 to the present
5          | millsecond          | uint16(2)   | eg: 0, 100 ,200, ... 900 or higher precision like: 50, 150,950,etc..
7          | water temp          | float(4)    | WAT unit:celsius
11         | cylinder head temp  | float(4)    | CHT unit:celsius
15         | exhaust gas temp    | float(4)    | EGT unit:celsius

### Device

Kart contains **Battery percentage**  
The data is arranged as follows :

Byte Index | Content             | Type(bytes) | Comment
---        | ---                 | ---         | ---
0          | Index               | byte(1)     | = 0xA1，Indicates that the packet is device information
1          | battery percent     | int8(1)     | Battery power percentage, error is -1

***

## Passive Packet Protocol
<table>
	<tr>
		<td >Class</td>
		<td>Item</td>
		<td>Read/Set (1/0)</td>	
		<td>6-byte invalid data</td>		
		<td colspan = "3">Parameter(in order)</td>	
	</tr>
	<tr>
		<td rowspan = "3">version (0x80) </td>
			<td>Equipment Model(0x01)</td>
			<td>1</td>
			<td>-</td>
			<td> String (byte [])</td>	
			<td> </td>
			<td> </td>				
	</tr>
	<tr>
			<td> Hardware Version (0x02)</td>
			<td>1</td>
			<td>-</td>
			<td>String(byte [])</td>
			<td> </td>
			<td> </td>	
	</tr>	
	<tr>
			<td>Software Version(0x03))</td>
			<td>1</td>
			<td>-</td>
			<td>String(byte [])</td>
			<td> </td>
			<td> </td>	
	</tr>	
	<tr>
		<td rowspan = "2">Engine(0x90)</td>
			<td>Engine Speed Ratio(0x01)</td>
			<td>1/0</td>
			<td>-</td>
			<td>Ratio (rpm*n) (uint 8)</td>
			<td> </td>
			<td> </td>				
	</tr>	
	<tr> 
			<td>Engine Sampling Frequency(0x02)</td>
			<td>1/0</td>
			<td>-</td>
			<td> Frequency(10,20,50,100)(uint16)</td>	
			<td> </td>
			<td> </td>	
	</tr>	
	<tr>
		<td rowspan = "4">Temperature acquisition(0xA0)</td>
			<td> Internal temperature(0x01)</td>
			<td>1/0</td>
			<td>-</td>
			<td>Temperature value (int16)</td>	
			<td> </td>
			<td> </td>	
	</tr>	
	<tr>
			<td>Water temperature(0x11)</td>
			<td>1/0</td>
			<td>-</td>
			<td>minimum temperature (int16)</td>
			<td>Maximum temperature (int16) </td>
			<td>Alarm temperature (int16) </td>				
	</tr>	
	<tr>
			<td>Cylinder head temperature(0x12)</td>
			<td>1/0</td>
			<td>-</td>
			<td>minimum temperature (int16)</td>
			<td>Maximum temperature (int16) </td>
			<td>Alarm temperature (int16) </td>
	</tr>	
	<tr>
			<td>Exhaust Gas Temperature (0x13)</td>
			<td>1/0</td>
			<td>-</td>
			<td>minimum temperature (int16)</td>
			<td>Maximum temperature (int16) </td>
			<td>Alarm temperature (int16) </td>
	</tr>	
	<tr>
			<td rowspan = "2">Working mode (0xB0)</td>
			<td> Enter boot (0x01)</td>
			<td> 0</td>
			<td>- </td>
			<td> </td>
			<td> </td>
			<td> </td>
	</tr>
	<tr>
			<td> Enter firmware upgrade (0x12)</td>
			<td> 0</td>
			<td>- </td>
			<td> </td>
			<td> </td>
			<td> </td>
	</tr>
	<tr>
		<td rowspan = "3">Firmware upgrade (0xC0)</td>
			<td> Maximum Packet Size(0x11) </td>
			<td>1</td>
			<td>-</td>
			<td> Packet size (uint32)</td>
			<td> Number of error transfers (uint8)</td>
			<td> </td>		
	</tr>	
	<tr>
			<td>File parameters (0x12)</td>
			<td>0</td>
			<td>-</td>
			<td>Packet size (uint32)</td>	
			<td> Check code (uint8)</td>
			<td> </td>			
	</tr>	
	<tr>
			<td>Packet parameters(0x13)</td>
			<td>0</td>
			<td>-</td>
			<td>Packet size (uint32)</td>
			<td> Check code (uint8)</td>
			<td> </td>			
	</tr>		
	<tr>
		<td rowspan = "5"> Timer (0xD0)</td>
			<td> Total boot-up time(0x10) </td>
			<td>1</td>
			<td>-</td>
			<td> Number of minutes (uint32)</td>	
			<td> </td>
			<td> </td>
	</tr>
	<tr>
			<td> Maintenance timer(0x11)</td>
			<td>1/0</td>
			<td>-</td>
			<td>Switch (uint8)</td>	
			<td> Number of minutes (uint 32)</td>
			<td> Maintenance cycle (uint 32)</td>	
	</tr>	
	<tr>
			<td> Timer 1(0x21)</td>
			<td>1/0</td>
			<td>-</td>
			<td> Switch (uint8)</td>
			<td> Number of minutes (uint 32)</td>
			<td> </td>		
	</tr>	
	<tr>
			<td>Timer 2((0x22))</td>
			<td>1/0</td>
			<td>-</td>
			<td>Switch(uint8)</td>
			<td> Number of minutes (uint 32)</td>
			<td>  </td>			
	</tr>	
	<tr>
			<td>Timer 3((0x23))</td>
			<td>1/0</td>
			<td>-</td>
			<td>Switch(uint8)</td>	
			<td> Number of minutes (uint 32)</td>
			<td>  </td>
	</tr>		
</table>


Note: After the mobile app sends data to the device, the mobile phone returns the data and adds the status code.
<table>
	<tr>
		<td>Response state</td>
		<td>Response Code</td>
	</tr>
	<tr>
		<td>Success</td>
		<td>0x01 0x01 </td>
	</tr>
	<tr>
		<td>Failure</td>
		<td>0x01 0x00 </td>
	</tr>
</table>

eg:  
>1.Read Equipment Model  
>Mobile phone sending data 0x80 01 01 00 00 00 00 00 00 00 00 00 00 00 00  
>Success: Equipment Return Data 0x90 01 01 00 00 00 00 00 00 00 A0 00 00 00 00   
>Failure: Device Return Data 0x90 01 01 00 00 00 00 00 00 00 00 00 00 00 00   
>
>2.Setting Engine Speed Ratio  
>Mobile phone sending data 0x90 01 00 00 00 00 00 00 00 00 00 00 00 00 00    
>Success: Equipment Return Data 0x90 01 00 00 00 00 00 00 00 00 01 01 00 00 00   
> Failure: Device Return Data 0x90 01 00 00 00 00 00 00 00 00 01 00 00 00 00  
>Note: Invalid bytes in the example are 0
***

## Feedback

If you have any questions during the development process, please contact us.

- Email: [yuanxiaochen1995@gmail.com](yuanxiaochen1995@gmail.com)
- facebook: [@弁杓](https://www.facebook.com/profile.php?id=100015307727134)
