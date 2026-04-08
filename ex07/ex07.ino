#include <WiFi.h>
#include <WebServer.h>

// Wi-Fi AP 配置
const char* ap_ssid = "ESP32-bush00200";
const char* ap_pass = "88888888"; // 至少8位

const int LED_PIN = 2; // LED 连接的 GPIO 引脚

// PWM 配置
const int PWM_FREQ_HZ = 5000;   // PWM 频率 5kHz
const int PWM_RES_BITS = 8;     // 分辨率 8位 (0~255)

volatile int g_pwmDuty = 0; // 全局变量，存储当前的 PWM 占空比 (0~255)

WebServer server(80);

// 生成网页 HTML 代码
String makePage() {
  // 根据当前占空比判断状态
  String state = (g_pwmDuty > 0) ? ("ON (Duty: " + String(g_pwmDuty) + ")") : "OFF (Duty: 0)";
  
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 LED 亮度控制</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; background-color: #f4f4f4; }
    h1 { color: #333; }
    .container { max-width: 400px; margin: 20px auto; padding: 20px; background: #fff; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }
    .slider-container { margin: 30px 0; }
    input[type=range] { width: 100%; }
    .value-display { font-size: 1.2rem; font-weight: bold; color: #007BFF; }
  </style>
</head>
<body>
  <div class="container">
    <h1>💡 LED 亮度控制</h1>
    <p>当前状态：<span id="stateText" class="value-display">)rawliteral" + state + R"rawliteral(</span></p>
    
    <div class="slider-container">
      <p>拖动滑块调节亮度 (0-255)</p>
      <!-- 滑块范围修改为 0-255 -->
      <input id="slider" type="range" min="0" max="255" value=")rawliteral" + String(g_pwmDuty) + R"rawliteral(" />
      <p>当前值：<span id="val" class="value-display">)rawliteral" + String(g_pwmDuty) + R"rawliteral(</span></p>
    </div>
  </div>

  <script>
    const slider = document.getElementById('slider');
    const val = document.getElementById('val');
    const stateText = document.getElementById('stateText');

    let latest = slider.value;
    let inFlight = false;

    // 防抖和队列处理，确保滑动平滑且不丢包
    function pump() {
      if (inFlight) return;
      inFlight = true;
      const v = latest;
      // 发送 GET 请求，将 0-255 的值传给 ESP32
      fetch('/brightness?v=' + encodeURIComponent(v), { method: 'GET', cache: 'no-store' })
        .catch(() => {})
        .finally(() => {
          inFlight = false;
          if (v !== latest) pump(); // 如果等待期间有新值，继续发送
        });
    }

    // 监听滑块的 'input' 事件，实现实时响应
    slider.addEventListener('input', () => {
      val.textContent = slider.value;
      const pwm = parseInt(slider.value, 10) || 0;
      // 更新网页上的状态文字
      stateText.textContent = (pwm > 0) ? (`ON (Duty: ${pwm})`) : 'OFF (Duty: 0)';
      latest = slider.value;
      pump();
    });
  </script>
</body>
</html>
)rawliteral";
  return html;
}

// 应用新的 PWM 占空比
static void applyPwmDuty(int duty) {
  duty = constrain(duty, 0, 255); // 确保值在 0-255 范围内
  g_pwmDuty = duty;
  // 直接将 0-255 的值写入 PWM，无需 map
  ledcWrite(LED_PIN, duty);
  Serial.printf("PWM Duty set to: %d\n", duty);
}

// 处理根路径请求，返回网页
void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

// 处理亮度调节请求
void handleBrightness() {
  int v = g_pwmDuty;
  if (server.hasArg("v")) {
    // 从 URL 参数中获取滑块的值 (0-255)
    v = server.arg("v").toInt();
  }
  applyPwmDuty(v);
  // 返回 204 No Content，表示请求成功但无需返回内容，减少延迟
  server.send(204, "text/plain", "");
}

void setup() {
  Serial.begin(115200);
  
  // 初始化 PWM
  ledcAttach(LED_PIN, PWM_FREQ_HZ, PWM_RES_BITS);
  applyPwmDuty(0); // 初始状态为熄灭

  // 启动 AP 模式
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_pass);
  Serial.println("AP 已开启");
  Serial.print("AP IP 地址: ");
  Serial.println(WiFi.softAPIP());

  // 设置路由
  server.on("/", handleRoot);
  server.on("/brightness", handleBrightness);
  server.begin();
  Serial.println("HTTP 服务器已启动");
}

void loop() {
  server.handleClient();
}