#pragma once

#include <iostream>
#include <Windows.h>
#include "LogicAnalyzerData.h"
#include <time.h>
#include "Grating.h"

#define _TEST_TIME 0			// 测试运行时间的开关
#define _FILTERING 0			// 是否滤波
#define _DISPLAY_SCHEDULE 1		// 是否显示处理进度

#define NOISE_US 15			// 噪声、尖波的脉宽，小于该脉宽的删掉，单位us，

using namespace std;