/*
 * 项目名称: ESP32 三色 LED + 板载 LED 同步闪烁 (1Hz)
 * 描述: 使用 millis() 函数同时控制三色 LED 与板载 LED 稳定闪烁
 * 硬件连接:
 *   - 红色 LED (R): GPIO 26
 *   - 绿色 LED (G): GPIO 27
 *   - 黄色 LED (Y): GPIO 25
 *   - 板载 LED: GPIO 2 (常见 ESP32 DevKit 板载灯)
 * 频率: 1Hz (亮 500ms, 灭 500ms)
 */

// --- 引脚定义 ---
const int ledPin_R = 26; // 红色
const int ledPin_G = 27; // 绿色
const int ledPin_Y = 25; // 黄色
const int ledPin = 2;    // 板载 LED

// --- 时间设置 ---
const long interval = 500; // 闪烁间隔 500ms (0.5秒亮 + 0.5秒灭 = 1Hz)

// --- 红色 LED 状态变量 ---
unsigned long previousMillis_R = 0;
int ledState_R = LOW;

// --- 绿色 LED 状态变量 ---
unsigned long previousMillis_G = 0;
int ledState_G = LOW;

// --- 黄色 LED 状态变量 ---
unsigned long previousMillis_Y = 0;
int ledState_Y = LOW;

// --- 板载 LED 状态变量 ---
unsigned long previousMillis_Onboard = 0;
int ledState_Onboard = LOW;

void setup() {
  // 初始化串口
  Serial.begin(115200);
  
  // 设置所有 LED 引脚为输出模式
  pinMode(ledPin_R, OUTPUT);
  pinMode(ledPin_G, OUTPUT);
  pinMode(ledPin_Y, OUTPUT);
  pinMode(ledPin, OUTPUT);
  
  // 初始状态全部关闭
  digitalWrite(ledPin_R, LOW);
  digitalWrite(ledPin_G, LOW);
  digitalWrite(ledPin_Y, LOW);
  digitalWrite(ledPin, LOW);
  
  Serial.println("程序启动：R(26), G(27), Y(25), 板载LED(2) 将以 1Hz 同步闪烁...");
}

void loop() {
  // 获取当前系统时间
  unsigned long currentMillis = millis();

  // ================= 处理红色 LED =================
  if (currentMillis - previousMillis_R >= interval) {
    previousMillis_R = currentMillis; // 更新上次时间
    
    // 翻转状态
    ledState_R = !ledState_R; 
    digitalWrite(ledPin_R, ledState_R);
  }

  // ================= 处理绿色 LED =================
  if (currentMillis - previousMillis_G >= interval) {
    previousMillis_G = currentMillis; // 更新上次时间
    
    // 翻转状态
    ledState_G = !ledState_G;
    digitalWrite(ledPin_G, ledState_G);
  }

  // ================= 处理黄色 LED =================
  if (currentMillis - previousMillis_Y >= interval) {
    previousMillis_Y = currentMillis; // 更新上次时间
    
    // 翻转状态
    ledState_Y = !ledState_Y;
    digitalWrite(ledPin_Y, ledState_Y);
  }

  // ================= 处理板载 LED =================
  if (currentMillis - previousMillis_Onboard >= interval) {
    previousMillis_Onboard = currentMillis; // 更新上次时间

    // 翻转状态
    ledState_Onboard = !ledState_Onboard;
    digitalWrite(ledPin, ledState_Onboard);
  }

  // 注意：这里没有 delay()，程序会极速循环检查上述四个条件。
  // 只有当时间差满足时，才会执行 digitalWrite，否则瞬间跳过。
  // 这使得四个灯看起来是“同时”闪烁的，且不会互相阻塞。
}