#include <WiFi.h>
#include <WebServer.h>

// ================= 配置区域 =================
const char* ap_ssid = "ESP32-bush00200";
const char* ap_pass = "88888888";

const int TOUCH_PIN = 27; // 触摸传感器引脚

WebServer server(80);

// ================= 网页界面生成 (HTML + CSS + JS) =================
String makePage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 实时仪表盘</title>
  <style>
    body { 
      font-family: 'Segoe UI', sans-serif; 
      background-color: #1a1a1a; 
      color: #fff; 
      display: flex; 
      justify-content: center; 
      align-items: center; 
      height: 100vh; 
      margin: 0; 
    }
    .dashboard-card {
      background: #2d2d2d;
      padding: 40px;
      border-radius: 20px;
      text-align: center;
      box-shadow: 0 10px 30px rgba(0,0,0,0.5);
      border: 1px solid #444;
      width: 300px;
    }
    h2 { margin-top: 0; color: #aaa; font-size: 1.2rem; letter-spacing: 1px; }
    .value-display {
      font-size: 4rem;
      font-weight: bold;
      color: #00d2ff;
      margin: 20px 0;
      text-shadow: 0 0 20px rgba(0, 210, 255, 0.4);
      font-variant-numeric: tabular-nums; /* 防止数字跳动时宽度变化 */
    }
    .unit { font-size: 1rem; color: #888; }
    .status-dot {
      height: 10px;
      width: 10px;
      background-color: #00ff88;
      border-radius: 50%;
      display: inline-block;
      margin-right: 5px;
      box-shadow: 0 0 5px #00ff88;
    }
  </style>
</head>
<body>

  <div class="dashboard-card">
    <h2><span class="status-dot"></span>触摸传感器数值</h2>
    <!-- 这里显示数值 -->
    <div id="sensorValue" class="value-display">--</div>
    <span class="unit">电容感应值 (0-1000+)</span>
  </div>

  <script>
    // AJAX 核心逻辑
    function updateDashboard() {
      // 1. 创建请求对象
      var xhttp = new XMLHttpRequest();
      
      // 2. 定义请求成功后的回调函数
      xhttp.onreadystatechange = function() {
        // readyState 4 表示请求完成，status 200 表示服务器响应正常
        if (this.readyState == 4 && this.status == 200) {
          // 3. 将服务器返回的文本（传感器数值）填入 HTML 元素中
          document.getElementById("sensorValue").innerHTML = this.responseText;
        }
      };
      
      // 4. 打开连接：GET 请求 "/data" 接口
      xhttp.open("GET", "/data", true);
      
      // 5. 发送请求
      xhttp.send();
    }

    // 设置定时器，每 200 毫秒执行一次 updateDashboard 函数
    // 5Hz 的刷新率对于人眼观察模拟量变化非常流畅
    setInterval(updateDashboard, 200);
  </script>

</body>
</html>
)rawliteral";
  return html;
}

// ================= 服务器处理函数 =================

// 1. 处理主页请求：返回网页界面
void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

// 2. 处理数据请求：只返回纯数字文本
void handleData() {
  int touchValue = touchRead(TOUCH_PIN);
  // 将读取到的整数转换为字符串发送回去
  server.send(200, "text/plain", String(touchValue));
}

void setup() {
  Serial.begin(115200);
  
  // 启动 Wi-Fi AP
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_pass);
  
  Serial.println("AP 启动成功");
  Serial.print("IP 地址: ");
  Serial.println(WiFi.softAPIP());

  // 注册路由
  server.on("/", handleRoot);    // 访问 / 显示网页
  server.on("/data", handleData); // 访问 /data 获取数值
  
  server.begin();
  Serial.println("HTTP 服务器已启动");
}

void loop() {
  server.handleClient(); // 监听客户端请求
}