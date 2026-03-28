# ex04：触摸自锁开关（边缘检测 + 防抖）

## 文件

- 程序文件：[ex04.ino](ex04.ino)
- 演示视频：[开关.mp4](%E5%BC%80%E5%85%B3.mp4)
- 截图：[dee4e53e-c270-412e-b8ad-a9738f53b7f3.png](dee4e53e-c270-412e-b8ad-a9738f53b7f3.png)

## 实验说明

- 硬件引脚：GPIO27（TOUCH7）作为触摸输入，GPIO2 作为 LED 输出
- 核心：
	- 使用布尔状态变量 `ledState` 表示 LED 当前状态
	- 使用 rawTouched/stableTouched 的软件防抖（默认 60ms）过滤手抖/噪声
	- 仅在检测到“未触摸 -> 触摸”的瞬间翻转 `ledState`（边缘检测），实现“自锁”效果
	- 触摸阈值：`THRESHOLD = 750`（按板子/环境可微调）
- 预期效果：摸一下触摸引脚，LED 亮起并保持；松开手再摸一下，LED 熄灭
