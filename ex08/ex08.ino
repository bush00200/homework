#include <WiFi.h>
#include <WebServer.h>

// ================= 配置区域 =================
const char* ap_ssid = "ESP32-bush00200";  // 已更新
const char* ap_pass = "88888888";         // 已更新

// 引脚定义
const int LED_PIN = 2;        // 板载 LED
const int TOUCH_PIN = 27;     // 触摸传感器引脚 (TOUCH7 / GPIO27)
const int TOUCH_THRESHOLD = 600; // 触摸阈值

// 系统状态枚举
enum SystemState {
  STATE_DISARMED = 0, // 撤防
  STATE_ARMED = 1,    // 布防
  STATE_ALARM = 2     // 报警锁定
};

SystemState systemState = STATE_DISARMED; // 初始状态为撤防

WebServer server(80);

// 报警闪烁控制变量
unsigned long lastBlinkTime = 0;
const unsigned long ALARM_INTERVAL_MS = 150; // 报警闪烁速度
bool ledCurrentState = false;

// ================= 网页界面生成 =================
String makePage() {
  String statusText = "";
  String statusColor = "";

  // 根据状态决定网页显示的文字和颜色
  switch (systemState) {
    case STATE_DISARMED:
      statusText = "已撤防 (安全)";
      statusColor = "#28a745"; // 绿色
      break;
    case STATE_ARMED:
      statusText = "已布防 (警戒中)";
      statusColor = "#ffc107"; // 黄色
      break;
    case STATE_ALARM:
      statusText = "⚠️ 报警! (入侵检测)";
      statusColor = "#dc3545"; // 红色
      break;
  }

  // 按钮样式逻辑
  String armBtnClass = (systemState == STATE_ARMED || systemState == STATE_ALARM) ? "activeBtn" : "inactiveBtn";
  String disArmBtnClass = (systemState == STATE_DISARMED) ? "activeBtn" : "inactiveBtn";

  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 安防主机</title>
  <style>
    body { font-family: 'Segoe UI', sans-serif; text-align: center; margin-top: 40px; background-color: #222; color: white; }
    h1 { margin-bottom: 10px; }
    .container { max-width: 400px; margin: 0 auto; padding: 30px; background: #333; border-radius: 15px; box-shadow: 0 0 20px rgba(0,0,0,0.5); }
    .status-badge { display: inline-block; padding: 10px 20px; border-radius: 5px; font-size: 1.2rem; font-weight: bold; margin-bottom: 30px; width: 80%; }
    .btn-group { display: flex; gap: 20px; justify-content: center; }
    button { padding: 15px 30px; font-size: 1.1rem; border: none; border-radius: 8px; cursor: pointer; transition: transform 0.1s; color: white; }
    button:active { transform: scale(0.95); }
    .btn-arm { background-color: #ffc107; color: #333; }
    .btn-disarm { background-color: #dc3545; }
    .activeBtn { box-shadow: 0 0 15px currentColor; transform: scale(1.05); border: 2px solid white; }
    .inactiveBtn { opacity: 0.5; filter: grayscale(1); }
  </style>
</head>
<body>
  <div class="container">
    <h1>🛡️ 智能安防主机</h1>
    <div class="status-badge" style="background-color: )rawliteral" + statusColor + R"rawliteral(;">
      )rawliteral" + statusText + R"rawliteral(
    </div>
    <div class="btn-group">
      <a href="/arm" style="text-decoration:none;"><button class="btn-arm )rawliteral" + armBtnClass + R"rawliteral(">布防 (Arm)</button></a>
      <a href="/disarm" style="text-decoration:none;"><button class="btn-disarm )rawliteral" + disArmBtnClass + R"rawliteral(">撤防 (Disarm)</button></a>
    </div>
    <p style="margin-top:20px; font-size: 0.8rem; color: #888;">点击布防后，触摸 GPIO27 将触发报警</p>
  </div>
</body>
</html>
)rawliteral";
  return html;
}

// ================= 功能函数 =================

void checkTouchSensor() {
  if (systemState == STATE_ARMED) {
    int touchValue = touchRead(TOUCH_PIN);
    if (touchValue < TOUCH_THRESHOLD) {
      systemState = STATE_ALARM;
      Serial.println(">>> 触发报警！入侵检测！ <<<");
      delay(200); // 简单防抖
    }
  }
}

void handleLED() {
  if (systemState == STATE_ALARM) {
    if (millis() - lastBlinkTime >= ALARM_INTERVAL_MS) {
      lastBlinkTime = millis();
      ledCurrentState = !ledCurrentState;
      digitalWrite(LED_PIN, ledCurrentState);
    }
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}

// ================= Web 服务器处理 =================

void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

void handleArm() {
  systemState = STATE_ARMED;
  Serial.println("系统已布防");
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleDisarm() {
  systemState = STATE_DISARMED;
  Serial.println("系统已撤防");
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  
  // 初始化硬件
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  // 启动 Wi-Fi AP (使用你指定的账号密码)
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_pass);
  
  Serial.println("AP 启动成功");
  Serial.print("AP 名称: ");
  Serial.println(ap_ssid);
  Serial.print("AP IP 地址: ");
  Serial.println(WiFi.softAPIP());

  // 设置路由
  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);
  server.begin();
}

void loop() {
  server.handleClient();
  checkTouchSensor();
  handleLED();
}
