# lab02：板载 LED 闪烁（delay）

## 文件

- 程序文件：[lab02.ino](lab02.ino)
- 演示视频：[delay(500).mp4](delay(500).mp4)、[delay(1000).mp4](delay(1000).mp4)
- 截图：[屏幕截图 2026-03-20 165835.png](%E5%B1%8F%E5%B9%95%E6%88%AA%E5%9B%BE%202026-03-20%20165835.png)

## 实验说明

- 核心：使用 `delay(500)` 控制 GPIO2（常见 ESP32 板载 LED）稳定 1Hz 闪烁，并通过串口打印 `LED ON/OFF`
