// 呼吸灯 PWM + 触摸自锁三档速度切换
// 触摸引脚：GPIO27（ESP32 Touch T7）

#include <Arduino.h>

#if defined(ARDUINO_ARCH_ESP32)
#include "esp32-hal-ledc.h"
#include "esp32-hal-touch.h"
#endif

// 定义LED引脚
const int ledPin = 2;

// 触摸引脚
const int touchPin = 27;

// 设置PWM属性
const int freq = 5000;      // 频率 5000Hz
const int resolution = 8;   // 分辨率 8位 (0-255)

// 速度档位：1 -> 2 -> 3 -> 1 ...
int speedLevel = 1;

// 触摸阈值与去抖
const uint16_t touchThreshold = 750;

// 参考 ex04：rawTouched 是瞬时读数，stableTouched 是防抖后的稳定状态
bool rawTouched = false;
bool stableTouched = false;
bool lastStableTouched = false;
unsigned long lastRawChangeMs = 0;
const unsigned long debounceMs = 60;

static int delayForSpeed(int level) {
  switch (level) {
    case 1: return 12; // 慢
    case 2: return 6;  // 中
    case 3: return 2;  // 快
    default: return 8;
  }
}

static int stepForSpeed(int level) {
  switch (level) {
    case 1: return 1; // 每次+1
    case 2: return 2; // 每次+2
    case 3: return 5; // 每次+5
    default: return 1;
  }
}

static uint16_t readTouchAvg(int pin, int samples) {
  uint32_t sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += (uint16_t)touchRead(pin);
    delay(10);
  }
  return (uint16_t)(sum / (uint32_t)samples);
}

static void calibrateTouch() {
  // 保留：仅用于串口打印参考值，不参与阈值计算。
  const uint16_t touchBaseline = readTouchAvg(touchPin, 30);
  Serial.print("Touch baseline: ");
  Serial.print(touchBaseline);
  Serial.print("  fixed threshold: ");
  Serial.println(touchThreshold);
}

static void handleTouchToggle() {
  const uint16_t touchValue = (uint16_t)touchRead(touchPin);
  const bool newRawTouched = (touchValue < touchThreshold);

  // 1) 记录原始状态变化时刻
  if (newRawTouched != rawTouched) {
    rawTouched = newRawTouched;
    lastRawChangeMs = millis();
  }

  // 2) 原始状态保持不变超过防抖时间后，更新稳定状态
  if ((millis() - lastRawChangeMs) >= debounceMs) {
    stableTouched = rawTouched;
  }

  // 3) 边缘检测：只在“未触摸 -> 触摸”的瞬间切换档位
  if (!lastStableTouched && stableTouched) {
    speedLevel++;
    if (speedLevel > 3) speedLevel = 1;

    Serial.print("Touch toggle success. Speed level = ");
    Serial.print(speedLevel);
    Serial.print("  (delay=");
    Serial.print(delayForSpeed(speedLevel));
    Serial.print("ms, step=");
    Serial.print(stepForSpeed(speedLevel));
    Serial.println(")");
  }
  lastStableTouched = stableTouched;
}

void setup() {
  Serial.begin(115200);
  delay(200);

  // 【新版用法】直接将引脚、频率和分辨率绑定
  ledcAttach(ledPin, freq, resolution);

  calibrateTouch();
  Serial.println("Breathing LED started. Touch GPIO27 to change speed (1/2/3). ");
}

void loop() {
  // 逐渐变亮
  int duty = 0;
  while (duty <= 255) {
    handleTouchToggle();

    ledcWrite(ledPin, duty);
    delay(delayForSpeed(speedLevel));

    duty += stepForSpeed(speedLevel);
  }

  // 逐渐变暗
  duty = 255;
  while (duty >= 0) {
    handleTouchToggle();

    ledcWrite(ledPin, duty);
    delay(delayForSpeed(speedLevel));

    duty -= stepForSpeed(speedLevel);
  }

  Serial.println("Breathing cycle completed");
}
