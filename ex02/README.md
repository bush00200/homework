# ex02：三色 LED + 板载 LED 同步稳定闪烁（millis）

## 文件

- 程序文件：[ex02.ino](ex02.ino)
- 演示视频：[millis().mp4](millis().mp4)

## 实验说明

- 硬件引脚：GPIO26（R）/GPIO27（G）/GPIO25（Y）+ GPIO2（板载 LED）
- 核心：用 `millis()` 做非阻塞定时，四个 LED 以 500ms 翻转一次，达到 1Hz 同步稳定闪烁
