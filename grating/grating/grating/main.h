#pragma once

#include <iostream>
#include <Windows.h>
#include "LogicAnalyzerData.h"
#include <time.h>

#define _TEST_TIME 0		// 测试运行时间的开关
#define _FILTERING 1		// 是否滤波

#define NOISE_US 20			// 噪声、尖波的脉宽，小于该脉宽的删掉，单位us，

using namespace std;