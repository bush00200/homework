# lab03：PWM 呼吸灯（GPIO2）

## 文件

- 程序文件：[lab03.ino](lab03.ino)
- 演示视频：[delay(5).mp4](delay(5).mp4)、[delay(10).mp4](delay(10).mp4)
- 截图：[13111983f476a3c33f557bc2d99c4ef8.png](13111983f476a3c33f557bc2d99c4ef8.png)

## 实验说明

- 核心：使用 ESP32 LEDC PWM（`ledcAttach/ledcWrite`）在 GPIO2 上做 0~255 占空比渐变，实现呼吸灯效果
