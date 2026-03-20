# homework
程序设计综合实践课程作业

## 版本记录

- ex01: [ex01/hello.c](ex01/hello.c) —— v1：输出 Hello World；v2：新增 `input()`，支持键盘输入并回显
- lab01: [lab01/lab01.ino](lab01/lab01.ino) —— Arduino IDE 安装与 ESP32 环境配置（3.12）
- lab02: [lab02/lab02.ino](lab02/lab02.ino) —— GPIO2 板载 LED 闪烁（delay 方式），并保存演示视频/截图
- lab03: [lab03/lab03.ino](lab03/lab03.ino) —— 使用 `ledcAttach/ledcWrite` 实现 GPIO2 PWM 呼吸灯，并保存演示视频/截图
- ex02: [ex02/ex02.ino](ex02/ex02.ino) —— 三色 LED + 板载 LED（GPIO2）用 `millis()` 非阻塞方式 1Hz 同步稳定闪烁，并保存演示视频
- ex03: [ex03/ex03.ino](ex03/ex03.ino) —— SOS 摩尔斯码信号（状态机 + `millis()`），同步控制 GPIO2/25/26/27 并串口输出，同时保存演示视频/截图

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
