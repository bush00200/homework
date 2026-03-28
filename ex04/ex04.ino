#define TOUCH_PIN 27
#define LED_PIN 2
#define THRESHOLD 750 // 需要根据实际测试修改此阈值
const unsigned long DEBOUNCE_MS = 60; // 软件防抖时间，避免手抖/噪声导致重复触发

int touchValue;
bool ledState = false;

// 防抖相关：rawTouched 是瞬时读数，stableTouched 是防抖后的稳定状态
bool rawTouched = false;
bool stableTouched = false;
bool lastStableTouched = false;
unsigned long lastRawChangeMs = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ledState);
  
  // 使用轮询 + 软件防抖 + 边缘检测来实现“自锁”开关（更可控，也避免在 ISR 里做耗时操作）
}

void loop() {
  touchValue = touchRead(TOUCH_PIN);

  bool newRawTouched = (touchValue < THRESHOLD);

  // 1) 记录原始状态变化时刻
  if (newRawTouched != rawTouched) {
    rawTouched = newRawTouched;
    lastRawChangeMs = millis();
  }

  // 2) 原始状态保持不变超过防抖时间后，更新稳定状态
  if ((millis() - lastRawChangeMs) >= DEBOUNCE_MS) {
    stableTouched = rawTouched;
  }

  // 3) 边缘检测：只在“未触摸 -> 触摸”的瞬间翻转 LED
  if (!lastStableTouched && stableTouched) {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
    Serial.print("Toggle LED, ledState=");
    Serial.println(ledState);
  }
  lastStableTouched = stableTouched;

  // 可选：观察触摸值（频率别太高，避免串口刷屏）
  static unsigned long lastPrintMs = 0;
  if (millis() - lastPrintMs >= 200) {
    lastPrintMs = millis();
    Serial.print("Touch Value: ");
    Serial.println(touchValue);
  }

  delay(10);
}
