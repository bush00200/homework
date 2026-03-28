# homework
程序设计综合实践课程作业

## 版本记录

- ex01: [ex01/hello.c](ex01/hello.c) —— v1：输出 Hello World；v2：新增 `input()`，支持键盘输入并回显
- lab01: [lab01/lab01.ino](lab01/lab01.ino) —— Arduino IDE 安装与 ESP32 环境配置（3.12）
- lab02: [lab02/lab02.ino](lab02/lab02.ino) —— GPIO2 板载 LED 闪烁（delay 方式），并保存演示视频/截图
- lab03: [lab03/lab03.ino](lab03/lab03.ino) —— 使用 `ledcAttach/ledcWrite` 实现 GPIO2 PWM 呼吸灯，并保存演示视频/截图
- ex02: [ex02/ex02.ino](ex02/ex02.ino) —— 三色 LED + 板载 LED（GPIO2）用 `millis()` 非阻塞方式 1Hz 同步稳定闪烁，并保存演示视频
- ex03: [ex03/ex03.ino](ex03/ex03.ino) —— SOS 摩尔斯码信号（状态机 + `millis()`），同步控制 GPIO2/25/26/27 并串口输出，同时保存演示视频/截图
- ex04: [ex04/ex04.ino](ex04/ex04.ino) —— 基于触摸传感器（GPIO27/TOUCH7）的“自锁”开关（边缘检测 + 软件防抖），并保存演示视频/截图
- ex05: [ex05/ex05.ino](ex05/ex05.ino) —— 多档位触摸调速呼吸灯（PWM + 触摸自锁切档 1/2/3），并保存演示视频/截图
- ex06: [ex06/ex06.ino](ex06/ex06.ino) —— 双通道 PWM 反相渐变（一个变亮同时另一个变暗），并保存演示视频/截图

## 实验说明

### lab02：板载 LED 闪烁（delay）

- 程序文件：[lab02/lab02.ino](lab02/lab02.ino)
- 核心：使用 `delay(500)` 控制 GPIO2（常见 ESP32 板载 LED）稳定 1Hz 闪烁，并通过串口打印 `LED ON/OFF`
- 记录：
	- [lab02/delay(500).mp4](lab02/delay(500).mp4)
	- [lab02/delay(1000).mp4](lab02/delay(1000).mp4)
	- [lab02/屏幕截图 2026-03-20 165835.png](lab02/%E5%B1%8F%E5%B9%95%E6%88%AA%E5%9B%BE%202026-03-20%20165835.png)

### lab03：PWM 呼吸灯（GPIO2）

- 程序文件：[lab03/lab03.ino](lab03/lab03.ino)
- 核心：使用 ESP32 LEDC PWM（`ledcAttach/ledcWrite`）在 GPIO2 上做 0~255 占空比渐变，实现呼吸灯效果
- 记录：
	- [lab03/delay(5).mp4](lab03/delay(5).mp4)
	- [lab03/delay(10).mp4](lab03/delay(10).mp4)
	- [lab03/13111983f476a3c33f557bc2d99c4ef8.png](lab03/13111983f476a3c33f557bc2d99c4ef8.png)

### ex02：三色 LED + 板载 LED 同步稳定闪烁（millis）

- 程序文件：[ex02/ex02.ino](ex02/ex02.ino)
- 硬件引脚：GPIO26（R）/GPIO27（G）/GPIO25（Y）+ GPIO2（板载 LED）
- 核心：用 `millis()` 做非阻塞定时，四个 LED 以 500ms 翻转一次，达到 1Hz 同步稳定闪烁
- 记录：[ex02/millis().mp4](ex02/millis().mp4)

### ex03：SOS 求救信号（状态机 + millis）

- 程序文件：[ex03/ex03.ino](ex03/ex03.ino)
- 硬件引脚：GPIO2/GPIO25/GPIO26/GPIO27
- 核心：用状态机切换“短闪/长闪/间隔/字母间隔/整组停顿”，并通过串口输出对应的摩尔斯码与字母标记
- 记录：
	- [ex03/SOS.mp4](ex03/SOS.mp4)
	- [ex03/0913652c797e761f639572bebe9840b5.png](ex03/0913652c797e761f639572bebe9840b5.png)
	- 今天2026年3月26日早8打卡

### ex04：触摸自锁开关（边缘检测 + 防抖）

- 程序文件：[ex04/ex04.ino](ex04/ex04.ino)
- 硬件引脚：GPIO27（TOUCH7）作为触摸输入，GPIO2 作为 LED 输出
- 核心：
	- 使用布尔状态变量 `ledState` 表示 LED 当前状态
	- 使用 rawTouched/stableTouched 的软件防抖（默认 60ms）过滤手抖/噪声
	- 仅在检测到“未触摸 -> 触摸”的瞬间翻转 `ledState`（边缘检测），实现“自锁”效果
	- 触摸阈值：`THRESHOLD = 750`（按板子/环境可微调）
- 预期效果：摸一下触摸引脚，LED 亮起并保持；松开手再摸一下，LED 熄灭
- 记录：
	- [ex04/开关.mp4](ex04/%E5%BC%80%E5%85%B3.mp4)
	- [ex04/dee4e53e-c270-412e-b8ad-a9738f53b7f3.png](ex04/dee4e53e-c270-412e-b8ad-a9738f53b7f3.png)

### ex05：多档位触摸调速呼吸灯（PWM + 触摸切档）

- 程序文件：[ex05/ex05.ino](ex05/ex05.ino)
- 硬件引脚：GPIO2 为 PWM 输出；GPIO27（TOUCH7）为触摸输入
- 核心：
	- LEDC PWM：`ledcAttach/ledcWrite` 输出 0~255 占空比渐变形成呼吸灯
	- 速度档位：`speedLevel = 1/2/3`，每次触摸按下瞬间循环切换（1->2->3->1...）
	- 通过调整 `delay()` 时长与占空比步进（Step），形成三个明显速度级别
	- 触摸判定阈值：`750`，并采用与 ex04 相同的防抖 + 边缘检测逻辑；切档成功会在串口输出提示
- 预期效果：LED 持续呼吸；每触摸一次，呼吸节奏立即改变（慢/中/快三档）
- 记录：
	- [ex05/挡位切换.mp4](ex05/%E6%8C%A1%E4%BD%8D%E5%88%87%E6%8D%A2.mp4)
	- [ex05/cbcaed57-9e1c-489d-82a1-a3955c309ae2.png](ex05/cbcaed57-9e1c-489d-82a1-a3955c309ae2.png)

### ex06：双通道 PWM 反相渐变（双LED交替柔和闪烁）

- 程序文件：[ex06/ex06.ino](ex06/ex06.ino)
- 硬件引脚：GPIO27 与 GPIO26 各接一个 LED，分别作为两个 PWM 输出
- 核心：初始化两个独立 PWM 输出，并让两个灯占空比呈反相：
	- 灯A：`duty = 0 -> 255`
	- 灯B：`duty = 255 - duty`
- 预期效果：两个 LED 平滑交替渐变，一个变亮同时另一个逐渐变暗，过渡柔和
- 记录：
	- [ex06/警车双闪灯效.mp4](ex06/%E8%AD%A6%E8%BD%A6%E5%8F%8C%E9%97%AA%E7%81%AF%E6%95%88.mp4)
	- [ex06/f64e3cb5-93e6-4057-98f0-d283cb18a2fe.png](ex06/f64e3cb5-93e6-4057-98f0-d283cb18a2fe.png)
