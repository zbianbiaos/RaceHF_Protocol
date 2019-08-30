# RaceHF Kart 协议

## 概述

\[[English](README.md)\]
[[中文](README_zh.md)\]

Kart项目是RaceHF下的一个面向专业赛车的产品，提供更专业的数据显示方式和专业数据分析的支持，提供更为直接的驾驶行为分析。

产品主要特点：

- **M4内核**高性能处理器
- **BT+BLE+WIFI**融合通讯方式.
- 优质定位芯片，最高真实刷新率可达**20hz**，专为赛车应用场景优化算法。兼容GPS/GLONASS，内置高灵敏度陶瓷天线，可外接天线
- 支持**空中固件升级**
- 支持**SD卡**记录VBO格式文件，把记录的数据导入CTool、Track attack等主流的赛车数据分析软件
- 实现开机即用，不再依赖手机
- 开放的数据通讯协议，第三方软件开发商快速开发和硬件兼容的APP
- 支持Andriod与IOS
- 支持全球使用最广泛的两个赛车数据分析APP：*Harry's Laptimer*（仅IOS）和*Racechrono*

***

> 广播名&设备名：       RaceHF_XXXX (XXXX表示4个十六进制数)  
> Service UUID：       0000ABF0-0000-1000-8000-00805F9B34FB  
> Characteristic UUID：0000ABF1-0000-1000-8000-00805F9B34FB

Kart产品兼容**蓝牙4.2**协议(MTU_MAX=517)，数据包固定为**80字节**（无效字节为0）一包。

## 主动数据协议

主动数据协议是设备周期性主动发送数据，使用BLE通知方式发送。  
数据包第一个字节(index)表示该数据包类型。

Index | Type           | Comment
---   | ---            | ---
0x10  | GPS            | UNIX时间戳 / 毫秒 / 经度 / 纬度 / 速度 / 方位角 / HDOP / 海拔高度 / 锁定卫星数 / 定位模式
0x20  | Engine.Rpm     | UNIX时间戳 / 毫秒 / 发动机转速时间间隔 / 转速有效数据个数 / 发动机转速数组
0x21  | Engine.Temper  | UNIX时间戳 / 毫秒 / 冷却液温度 / 缸盖温度 / 排气管温度
0x30  | Device         | 电池电量
0x60  | Meaningless    | 无意义数据包

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
38		   | visible satellites  | byte(1)     | 可见卫星数量
39         | fix quality         | byte(1)     | 定位模式，0:未定位，1:2D定位，2:3D定位，4:3D差分定位

### Engine

#### Engine.Rpm

Engine.Rpm 包含 **UNIX时间戳** / **毫秒** / **发动机转速时间间隔** / **转速有效数据个数** / **发动机转速数组**  
数据排列方式如下：

Byte Index | Content             | Type(bytes) | Comment
---        | ---                 | ---         | ---
0          | Index               | byte(1)     | = 0x21，表示该数据包是Engine.Rpm
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
0          | Index               | byte(1)     | = 0x22，表示该数据包是Engine.Temper
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

## 被动数据协议

被动数据协议使用请求——响应方式传输信息，数据传输协议如下:

<table>
	<tr>
		<td >类</td>
		<td>项</td>
		<td>读取/设置(1/0)</td>	
		<td>7字节无效数据</td>	
		<td colspan = "3"> 参数(按顺序排列)</td>	
	</tr>
	<tr>
		<td rowspan = "3">版本(0x80) </td>
			<td>设备型号(0x01)</td>
			<td>1</td>
			<td>-</td>
			<td>字符串(byte[])</td>	
			<td> </td>	
			<td> </td>	
	</tr>
	<tr>
			<td>硬件版本(0x02)</td>
			<td>1</td>
			<td>-</td>
			<td>字符串(byte[])</td>
			<td> </td>	
			<td> </td>	
	</tr>	
	<tr>
			<td>软件版本(0x03))</td>
			<td>1</td>
			<td>-</td>
			<td>字符串(byte[])</td>
			<td> </td>	
			<td> </td>	
	</tr>	
	<tr>
		<td rowspan = "2">发动机(0x90)</td>
			<td>发动机转速倍率(0x01)</td>
			<td>1/0</td>
			<td>-</td>
			<td>倍率(rpm*n)(uint8)</td>	
			<td> </td>	
			<td> </td>	
	</tr>	
	<tr> 
			<td>发动机采样频率(0x02)</td>
			<td>1/0</td>
			<td>-</td>
			<td>频率(10,20,50,100)(uint16)</td>	
			<td> </td>	
			<td> </td>	
	</tr>	
	<tr>
		<td rowspan = "4">温度采集(0xA0)</td>
			<td>内部温度(0x01)</td>
			<td>1/0</td>
			<td>-</td>
			<td>温度值(int16)</td>	
			<td> </td>	
			<td> </td>	
	</tr>	
	<tr>
			<td>冷却液温度(0x11)</td>
			<td>1/0</td>
			<td>-</td>
			<td>最低温度(int16)</td>
			<td>最高温度(int16)</td>	
			<td>报警温度(int16)</td>				
	</tr>	
	<tr>
			<td>缸盖温度(0x12)</td>
			<td>1/0</td>
			<td>-</td>
			<td>最低温度(int16)</td>
			<td>最高温度(int16)</td>	
			<td>报警温度(int16)</td>	
	</tr>	
	<tr>
			<td>排气温度(0x13)</td>
			<td>1/0</td>
			<td>-</td>
			<td>最低温度(int16)</td>
			<td>最高温度(int16)</td>	
			<td>报警温度(int16)</td>	
	</tr>
	<tr>
		<td rowspan = "2">工作模式(0xB0)</td>
			<td>进入引导(0x01) </td>
			<td>0</td>
			<td>-</td>
			<td> </td>
			<td> </td>				
			<td> </td>	
	</tr>	
	<tr>
			<td>进入固件升级(0x12) </td>
			<td>0</td>
			<td>-</td>
			<td> </td>
			<td> </td>				
			<td> </td>	
	</tr>	
	<tr>
		<td rowspan = "3">固件升级(0xC0)</td>
			<td>最大数据包大小(0x11) </td>
			<td>1</td>
			<td>-</td>
			<td>数据包大小(uint32)</td>	
			<td>错误传输次数(uint8)</td>
			<td> </td>				
	</tr>
	<tr>
			<td>文件参数(0x12)</td>
			<td>0</td>
			<td>-</td>
			<td>文件大小(uint32)</td>	
			<td>校验码(uint8)</td>	
			<td> </td>	
	</tr>
	<tr>
			<td>数据包参数(0x13)</td>
			<td>0</td>
			<td>-</td>
			<td>数据包大小(uint32)</td>	
			<td>校验码(uint8)</td>	
			<td> </td>				
	</tr>		
	<tr>
		<td rowspan = "5">计时器(0xD0)</td>
			<td>总开机时间(0x10) </td>
			<td>1</td>
			<td>-</td>
			<td>分钟数(uint32)</td>	
			<td> </td>		
			<td> </td>				
	</tr>
	<tr>
			<td>保养计时器(0x11)</td>
			<td>1/0</td>
			<td>-</td>
			<td>开关(uint8)</td>	
			<td>分钟数(uint32)</td>	
			<td>保养周期(uint32)</td>	
	</tr>	
	<tr>
			<td>计时器1(0x21)</td>
			<td>1/0</td>
			<td>-</td>
			<td>开关(uint8)</td>	
			<td>分钟数(uint32)</td>	
			<td> </td>	
	</tr>	
	<tr>
			<td>计时器2(0x22)</td>
			<td>1/0</td>
			<td>-</td>
			<td>开关(uint8)</td>
			<td>分钟数(uint32)</td>	
			<td> </td>				
	</tr>	
	<tr>
			<td>计时器3(0x23)</td>
			<td>1/0</td>
			<td>-</td>
			<td>开关(uint8)</td>
			<td>分钟数(uint32)</td>	
			<td> </td>				
	</tr>		
</table>

备注:手机APP向设备发送数据完成后，手机回传数据，并添加状态码
<table>
	<tr>
		<td>响应状态</td>
		<td>响应码</td>
	</tr>
	<tr>
		<td>成功</td>
		<td>0x01 0x01 </td>
	</tr>
	<tr>
		<td>失败</td>
		<td>0x01 0x00 </td>
	</tr>
</table>

示例:  
>1.读取设备型号  
>手机发送数据 0x80 01 01 00 00 00 00 00 00 00 00 00 00 00 00  
>成功:设备传回数据 0x90 01 01 00 00 00 00 00 00 00 A0 00 00 00 00   
>成功:设备传回数据 0x90 01 01 00 00 00 00 00 00 00 00 00 00 00 00   
>
>2.设置发动机转速倍率  
>手机发送数据 0x90 01 00 00 00 00 00 00 00 00 00 00 00 00 00    
>成功：设备传回数据 0x90 01 00 00 00 00 00 00 00 00 01 01 00 00 00   
>失败：设备传回数据 0x90 01 00 00 00 00 00 00 00 00 01 00 00 00 00  
>备注:示例中无效字节为0
***

## 有问题反馈

在开发中有任何问题，欢迎反馈给我

- 邮件：[yuanxiaochen1995@gmail.com](yuanxiaochen1995@gmail.com)
- facebook： [@弁杓](https://www.facebook.com/profile.php?id=100015307727134)
