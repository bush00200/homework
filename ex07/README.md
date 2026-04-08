# ex07：Web 网页端无极调光器（滑动条 + PWM）

## 文件

- 程序文件：[ex07.ino](ex07.ino)

## 实验说明

- 参考：PWM 呼吸灯（LEDC）与 Web 服务器（ESP32 SoftAP + WebServer）
- 硬件引脚：GPIO2（板载 LED / 外接 LED）
- 核心：
	- ESP32 开启 AP 并启动 HTTP Server
	- 网页端使用 `<input type="range" min="0" max="255">` 作为滑动条
	- JavaScript 监听 `input` 事件，用 `fetch()` 将当前值通过 `GET /brightness?v=xxx` 发送给 ESP32
	- ESP32 解析 URL 参数 `v`，并用 `ledcWrite(GPIO2, duty)` 写入 0~255 占空比

## 预期效果

- 手机/电脑连接 ESP32 的 Wi-Fi 后打开网页，页面显示一个滑动条
- 拖动滑动条时，开发板 LED 亮度随滑动条位置实时、平滑变化
