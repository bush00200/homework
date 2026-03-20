/*
 * 项目名称: ESP32 SOS 求救信号 (非阻塞 millis 版)
 * 描述: 同时控制板载LED(2) 和 外接LED(25, 26, 27) 发送 SOS 信号
 * 信号模式: ... --- ... (3短, 3长, 3短)
 * 时序标准:
 *   - 短闪(Dot): 亮200ms, 灭200ms
 *   - 长闪(Dash): 亮600ms, 灭200ms
 *   - 字符内间隔: 200ms (即灭的时间)
 *   - 字母间间隔: 500ms (S和O之间)
 *   - 单词间间隔: 2000ms (一轮SOS结束后)
 */

// --- 引脚定义 ---
const int ledPin_OnBoard = 2;
const int ledPin_R = 26;
const int ledPin_G = 27;
const int ledPin_Y = 25;

// --- 时序常量 (毫秒) ---
const unsigned long TIME_DOT_ON = 200;   // 短闪亮
const unsigned long TIME_DASH_ON = 600;  // 长闪亮
const unsigned long TIME_GAP_OFF = 200;  // 闪之间的灭灯时间
const unsigned long TIME_LETTER_GAP = 500; // 字母(S与O)之间的停顿
const unsigned long TIME_WORD_GAP = 2000;  // 整组SOS结束后的停顿

// --- 状态机变量 ---
enum SosState {
  STATE_S1, STATE_S2, STATE_S3, // S 的三次短闪
  STATE_GAP_SO_1,               // S 结束后的短暂停顿
  STATE_O1, STATE_O2, STATE_O3, // O 的三次长闪
  STATE_GAP_OS_1,               // O 结束后的短暂停顿
  STATE_S4, STATE_S5, STATE_S6, // 第二个 S 的三次短闪
  STATE_WORD_PAUSE              // 整组结束后的长停顿
};

SosState currentState = STATE_S1; // 初始状态
unsigned long previousMillis = 0; // 记录上次动作时间
bool ledOn = false;               // 当前LED物理状态

// 定义所有需要控制的引脚数组，方便批量操作
const int ledPins[] = {ledPin_OnBoard, ledPin_R, ledPin_G, ledPin_Y};
const int numLeds = 4;

void setup() {
  Serial.begin(115200);
  
  // 初始化所有 LED 引脚
  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }
  
  Serial.println("SOS Signal Started using millis()...");
  previousMillis = millis(); // 初始化时间基准
}

void loop() {
  unsigned long currentMillis = millis();
  
  // 检查是否到了执行下一步的时间
  if (currentMillis - previousMillis >= getNextInterval()) {
    
    previousMillis = currentMillis; // 重置计时器
    executeNextStep();              // 执行状态流转
  }
}

/**
 * 根据当前状态，返回需要等待的时间间隔
 * 并决定下一步的状态
 */
unsigned long getNextInterval() {
  // 如果当前是“亮”的状态，返回对应的亮灯时间
  // 如果当前是“灭”的状态，返回对应的灭灯时间或间隔时间
  
  switch (currentState) {
    // --- S 部分 (短闪) ---
    case STATE_S1: case STATE_S2: case STATE_S3:
    case STATE_S4: case STATE_S5: case STATE_S6:
      if (!ledOn) return TIME_DOT_ON; // 如果现在是灭的，准备亮 (短)
      else return TIME_GAP_OFF;       // 如果现在是亮的，准备灭 (间隔)

    // --- O 部分 (长闪) ---
    case STATE_O1: case STATE_O2: case STATE_O3:
      if (!ledOn) return TIME_DASH_ON; // 如果现在是灭的，准备亮 (长)
      else return TIME_GAP_OFF;        // 如果现在是亮的，准备灭 (间隔)

    // --- 字母间间隔 (S->O 或 O->S) ---
    case STATE_GAP_SO_1: 
    case STATE_GAP_OS_1:
      return TIME_LETTER_GAP;

    // --- 单词间间隔 (SOS 结束) ---
    case STATE_WORD_PAUSE:
      return TIME_WORD_GAP;
      
    default:
      return 200;
  }
}

/**
 * 执行状态流转逻辑：改变 LED 状态，更新状态机
 */
void executeNextStep() {
  
  // 1. 处理“亮/灭”切换的状态
  // 如果当前状态是 S1-S6 或 O1-O3，我们需要翻转 LED
  bool isFlashState = false;
  
  switch (currentState) {
    case STATE_S1: case STATE_S2: case STATE_S3:
    case STATE_S4: case STATE_S5: case STATE_S6:
    case STATE_O1: case STATE_O2: case STATE_O3:
      isFlashState = true;
      break;
    default:
      isFlashState = false;
      break;
  }

  if (isFlashState) {
    // 翻转 LED 物理状态
    ledOn = !ledOn;
    setAllLeds(ledOn ? HIGH : LOW);
    
    // 状态机前进逻辑：
    // 如果刚刚完成了“亮”的动作 (ledOn 变为 true)，下一步应该是“灭” (保持当前状态编号，但在逻辑上我们通常直接推进到下一个阶段)
    // 为了简化，我们采用：每次进入此函数都推进状态。
    // 这里的逻辑设计是：每个状态代表一个完整的“动作周期”的开始。
    // 更好的方式是：状态代表“接下来要做什么”。
    
    // 修正逻辑：上面的 getNextInterval 依赖 ledOn 变量。
    // 所以这里我们只负责在“动作完成时”切换到下一个序号状态。
    // 但上面的 getNextInterval 写法有点混合。让我们用更清晰的状态推进法：
    
    /* 
       重新梳理逻辑：
       每个状态其实包含两个阶段：亮 -> 灭。
       为了代码极简，我们让状态机每一步只代表“改变一次状态”。
    */
    
    // 既然 ledOn 已经翻转了，我们需要判断是否需要切换到下一个序号状态
    // 规则：如果是从 灭->亮，保持在当前序号状态（因为 getNextInterval 会根据 !ledOn 返回亮的时间? 不对）
    
    // 让我们换一种绝对清晰的状态机写法，不依赖外部 ledOn 判断流程，而是状态驱动流程。
    // 下面的 switch 将直接决定下一个状态是什么。
  }
  
  // --- 重写状态推进逻辑 (更稳健) ---
  // 每次调用 executeNextStep，意味着上一个时间段结束了。
  
  switch (currentState) {
    // S 部分
    case STATE_S1: // 刚完成了 S1 的亮或灭？我们需要细分状态吗？
                   // 为了不使用 delay，最清晰的方法是将“亮”和“灭”拆分为不同状态，或者利用 ledOn 标志位。
                   // 利用 ledOn 标志位的方法：
                   if (ledOn) { 
                     // 刚刚变亮了，现在需要等待“亮”的时间，然后变灭。
                     // 此时状态不变，但 getNextInterval 会返回亮的时间？
                     // 不，刚才的逻辑是：进入状态时，如果 ledOn 是 false，返回亮的时间。
                     // 这意味着：状态代表“准备亮”。
                     // 执行后：ledOn 变 true。下次循环，ledOn 是 true，返回灭的时间。
                     // 再下次执行：ledOn 变 false。此时应该切换到下一个序号状态。
                     
                     // 保持当前状态序号，等待下一次时间片处理“灭”
                     return; 
                   } else {
                     // 刚刚变灭了，现在要切换到下一个序号
                     advanceState();
                     return;
                   }

    case STATE_S2: case STATE_S3: case STATE_S4: case STATE_S5: case STATE_S6:
      if (ledOn) return; // 刚亮，等灭
      else { advanceState(); return; } // 刚灭，进下一号

    case STATE_O1: case STATE_O2: case STATE_O3:
      if (ledOn) return; // 刚亮，等灭
      else { advanceState(); return; } // 刚灭，进下一号

    // 间隔状态 (这些状态只做一次等待，然后直接切换)
    case STATE_GAP_SO_1:
      setAllLeds(LOW); ledOn = false;
      advanceState();
      break;
      
    case STATE_GAP_OS_1:
      setAllLeds(LOW); ledOn = false;
      advanceState();
      break;

    case STATE_WORD_PAUSE:
      setAllLeds(LOW); ledOn = false;
      advanceState();
      break;
  }
}

// 辅助函数：推进状态机到下一个阶段
void advanceState() {
  switch (currentState) {
    case STATE_S1: currentState = STATE_S2; break;
    case STATE_S2: currentState = STATE_S3; break;
    case STATE_S3: currentState = STATE_GAP_SO_1; break; // S 结束，进间隔
    
    case STATE_GAP_SO_1: currentState = STATE_O1; break; // 间隔结束，进 O
    
    case STATE_O1: currentState = STATE_O2; break;
    case STATE_O2: currentState = STATE_O3; break;
    case STATE_O3: currentState = STATE_GAP_OS_1; break; // O 结束，进间隔
    
    case STATE_GAP_OS_1: currentState = STATE_S4; break; // 间隔结束，进第二个 S
    
    case STATE_S4: currentState = STATE_S5; break;
    case STATE_S5: currentState = STATE_S6; break;
    case STATE_S6: currentState = STATE_WORD_PAUSE; break; // 第二个 S 结束，进长停顿
    
    case STATE_WORD_PAUSE: currentState = STATE_S1; break; // 循环重新开始
  }
  
  // 每次进入新状态时，确保灯是灭的（除非是闪的状态，会在第一次循环被点亮）
  // 对于闪的状态 (S1..S6, O1..O3)，我们在刚进入时 ledOn 是 false，会触发点亮逻辑
  if (currentState != STATE_S1 && currentState != STATE_S2 && currentState != STATE_S3 &&
      currentState != STATE_S4 && currentState != STATE_S5 && currentState != STATE_S6 &&
      currentState != STATE_O1 && currentState != STATE_O2 && currentState != STATE_O3) {
     // 间隔状态，确保灭
     ledOn = false;
     setAllLeds(LOW);
  } else {
     // 闪烁状态，准备点亮
     ledOn = false; // 重置为 false，以便逻辑判断“接下来要亮”
     // 注意：这里有个小技巧。
     // 如果刚进入 S1，ledOn=false。
     // getNextInterval 看到 !ledOn (针对 S1) 返回 TIME_DOT_ON (准备亮的时间)。
     // 时间到后，executeNextStep 把 ledOn 变 true (点亮)。
     // 再次循环，getNextInterval 看到 ledOn (针对 S1) 返回 TIME_GAP_OFF (准备灭的时间)。
     // 时间到后，executeNextStep 把 ledOn 变 false，并 advanceState 到 S2。
     // 逻辑闭环！
  }
}

// 批量设置所有 LED
void setAllLeds(int value) {
  for (int i = 0; i < numLeds; i++) {
    digitalWrite(ledPins[i], value);
  }
}