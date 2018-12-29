#pragma once

#include "main.h"
#include <fstream>
#include <string>

using namespace std;

class LogicAnalyzerData
{
public:
	LogicAnalyzerData(string fileName);
	~LogicAnalyzerData();
private:
	ifstream inFile;

	// 文件操作
	void openFile();
	bool closeFile();
private:
	// 文件总的行数，初始化时计算（方便查看进度）
	unsigned int lineNum;
	// 计算文件总的行数
	void countLineNum();
public:
	// 获得文件总行数
	unsigned int getLineNum();
private:
	// 几个通道，不包括前面的时间轴
	unsigned int channel;
private:
	// 统计有几个通道，不包括时间轴
	void countChannel();
public:
	// 获得通道数
	unsigned int getChannelNum();
private:
	// 文件名
	string fileName;
public:
	// 数据滤波，滤波后的将文件另外保存，并更改filename指向更改后的文件
	void filtering(unsigned int threshold);
};

