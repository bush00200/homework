/*
 * 项目名称: ESP32 SOS 求救信号 + 串口监视器输出
 * 描述: 使用 millis() 控制 LED 闪烁，并在串口实时打印对应的摩尔斯码和字母
 * 硬件: 引脚GPIO 2, 25, 26, 27
 */

// --- 引脚定义 ---
const int ledPins[] = {2, 26, 27, 25}; // 板载, R, G, Y
const int numLeds = 4;

// --- 时序常量 (毫秒) ---
const unsigned long TIME_DOT_ON = 200;   // 短闪亮
const unsigned long TIME_DASH_ON = 600;  // 长闪亮
const unsigned long TIME_GAP_OFF = 200;  // 闪之间的灭灯时间
const unsigned long TIME_LETTER_GAP = 500; // 字母(S与O)之间的停顿
const unsigned long TIME_WORD_GAP = 2000;  // 整组SOS结束后的停顿

// --- 状态机定义 ---
enum SosState {
  // S (短闪 3 次)
  STATE_S1_ON, STATE_S1_OFF,
  STATE_S2_ON, STATE_S2_OFF,
  STATE_S3_ON, STATE_S3_OFF,
  
  STATE_GAP_SO, // S 和 O 之间的间隔
  
  // O (长闪 3 次)
  STATE_O1_ON, STATE_O1_OFF,
  STATE_O2_ON, STATE_O2_OFF,
  STATE_O3_ON, STATE_O3_OFF,
  
  STATE_GAP_OS, // O 和 S 之间的间隔
  
  // S (短闪 3 次 - 第二轮)
  STATE_S4_ON, STATE_S4_OFF,
  STATE_S5_ON, STATE_S5_OFF,
  STATE_S6_ON, STATE_S6_OFF,
  
  STATE_WORD_PAUSE // 整组结束后的长停顿
};

SosState currentState = STATE_S1_ON;
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial); // 等待串口连接 (可选，防止启动时丢失第一行日志)
  
  // 初始化 LED
  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
  
  Serial.println("\n=== SOS Signal Monitor Started ===");
  Serial.println("Listening for Morse Code...");
  previousMillis = millis();
}

void loop() {
  unsigned long currentMillis = millis();
  unsigned long interval = getIntervalForState(currentState);

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    nextState();
  }
}

// 获取当前状态需要等待的时间
unsigned long getIntervalForState(SosState state) {
  switch (state) {
    // 所有 "ON" 状态
    case STATE_S1_ON: case STATE_S2_ON: case STATE_S3_ON:
    case STATE_S4_ON: case STATE_S5_ON: case STATE_S6_ON:
      return TIME_DOT_ON;
      
    case STATE_O1_ON: case STATE_O2_ON: case STATE_O3_ON:
      return TIME_DASH_ON;
      
    // 所有 "OFF" 状态 (闪烁间的间隔)
    case STATE_S1_OFF: case STATE_S2_OFF: case STATE_S3_OFF:
    case STATE_S4_OFF: case STATE_S5_OFF: case STATE_S6_OFF:
    case STATE_O1_OFF: case STATE_O2_OFF: case STATE_O3_OFF:
      return TIME_GAP_OFF;
      
    // 字母间隔
    case STATE_GAP_SO:
    case STATE_GAP_OS:
      return TIME_LETTER_GAP;
      
    // 单词间隔
    case STATE_WORD_PAUSE:
      return TIME_WORD_GAP;
      
    default:
      return 200;
  }
}

// 状态流转与动作执行
void nextState() {
  bool shouldPrintDot = false;
  bool shouldPrintDash = false;
  bool shouldPrintLetterS = false;
  bool shouldPrintLetterO = false;
  bool shouldPrintPause = false;

  // 1. 执行动作 (亮/灭) 并标记打印任务
  switch (currentState) {
    // --- S 部分 (短闪) ---
    case STATE_S1_ON: digitalWriteAll(HIGH); break;
    case STATE_S1_OFF: 
      digitalWriteAll(LOW); 
      shouldPrintDot = true; 
      break;
      
    case STATE_S2_ON: digitalWriteAll(HIGH); break;
    case STATE_S2_OFF: 
      digitalWriteAll(LOW); 
      shouldPrintDot = true; 
      break;
      
    case STATE_S3_ON: digitalWriteAll(HIGH); break;
    case STATE_S3_OFF: 
      digitalWriteAll(LOW); 
      shouldPrintDot = true; 
      shouldPrintLetterS = true; // S 结束
      break;

    // --- 间隔 S -> O ---
    case STATE_GAP_SO:
      // 纯等待，无动作
      break;

    // --- O 部分 (长闪) ---
    case STATE_O1_ON: digitalWriteAll(HIGH); break;
    case STATE_O1_OFF: 
      digitalWriteAll(LOW); 
      shouldPrintDash = true; 
      break;
      
    case STATE_O2_ON: digitalWriteAll(HIGH); break;
    case STATE_O2_OFF: 
      digitalWriteAll(LOW); 
      shouldPrintDash = true; 
      break;
      
    case STATE_O3_ON: digitalWriteAll(HIGH); break;
    case STATE_O3_OFF: 
      digitalWriteAll(LOW); 
      shouldPrintDash = true; 
      shouldPrintLetterO = true; // O 结束
      break;

    // --- 间隔 O -> S ---
    case STATE_GAP_OS:
      break;

    // --- 第二轮 S ---
    case STATE_S4_ON: digitalWriteAll(HIGH); break;
    case STATE_S4_OFF: 
      digitalWriteAll(LOW); 
      shouldPrintDot = true; 
      break;
      
    case STATE_S5_ON: digitalWriteAll(HIGH); break;
    case STATE_S5_OFF: 
      digitalWriteAll(LOW); 
      shouldPrintDot = true; 
      break;
      
    case STATE_S6_ON: digitalWriteAll(HIGH); break;
    case STATE_S6_OFF: 
      digitalWriteAll(LOW); 
      shouldPrintDot = true; 
      shouldPrintLetterS = true; // 第二个 S 结束
      break;

    // --- 长停顿 ---
    case STATE_WORD_PAUSE:
      shouldPrintPause = true;
      break;
  }

  // 2. 执行串口打印 (根据标记)
  if (shouldPrintDot) Serial.print(".");
  if (shouldPrintDash) Serial.print("-");
  
  if (shouldPrintLetterS) Serial.print(" [S] ");
  if (shouldPrintLetterO) Serial.print(" [O] ");
  
  if (shouldPrintPause) {
    Serial.println("--- [SOS COMPLETE] ---");
    // 换行，准备下一轮
  } else if (shouldPrintLetterS || shouldPrintLetterO) {
    // 字母打印后不需要立即换行，保持在一行显示 ... [S] --- [O] ...
    // 但为了清晰，我们可以选择不换行，让它们连在一起
  }
  
  // 强制刷新串口缓冲区，确保立刻看到输出
  Serial.flush();

  // 3. 推进到下一个状态
  advanceStateEnum();
}

// 纯粹的状态序号推进逻辑
void advanceStateEnum() {
  switch (currentState) {
    case STATE_S1_ON: currentState = STATE_S1_OFF; break;
    case STATE_S1_OFF: currentState = STATE_S2_ON; break;
    case STATE_S2_ON: currentState = STATE_S2_OFF; break;
    case STATE_S2_OFF: currentState = STATE_S3_ON; break;
    case STATE_S3_ON: currentState = STATE_S3_OFF; break;
    case STATE_S3_OFF: currentState = STATE_GAP_SO; break;
    
    case STATE_GAP_SO: currentState = STATE_O1_ON; break;
    
    case STATE_O1_ON: currentState = STATE_O1_OFF; break;
    case STATE_O1_OFF: currentState = STATE_O2_ON; break;
    case STATE_O2_ON: currentState = STATE_O2_OFF; break;
    case STATE_O2_OFF: currentState = STATE_O3_ON; break;
    case STATE_O3_ON: currentState = STATE_O3_OFF; break;
    case STATE_O3_OFF: currentState = STATE_GAP_OS; break;
    
    case STATE_GAP_OS: currentState = STATE_S4_ON; break;
    
    case STATE_S4_ON: currentState = STATE_S4_OFF; break;
    case STATE_S4_OFF: currentState = STATE_S5_ON; break;
    case STATE_S5_ON: currentState = STATE_S5_OFF; break;
    case STATE_S5_OFF: currentState = STATE_S6_ON; break;
    case STATE_S6_ON: currentState = STATE_S6_OFF; break;
    case STATE_S6_OFF: currentState = STATE_WORD_PAUSE; break;
    
    case STATE_WORD_PAUSE: currentState = STATE_S1_ON; break;
  }
}

void digitalWriteAll(int value) {
  for (int i = 0; i < numLeds; i++) {
    digitalWrite(ledPins[i], value);
  }
}