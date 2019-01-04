#pragma once

// #include "main.h"
#include <fstream>
#include <string>
#include <vector>
#include "myLib.h"

using namespace std;

// 通道的逻辑值，要么1，要么0 
enum logicValue
{
	low,
	high
};

// 时间轴+通道的高低电平
typedef struct {
	double time;
	logicValue chanValue;
} timeAndChanValue;

// 一行string里面的数据
typedef struct {
	double time;					//  时间轴，单位s
	vector<logicValue> chanValue;		// 各通道的值，值为0或1
} LADataPer;

class LogicAnalyzerData
{
public:
	LogicAnalyzerData(string fileName);
	~LogicAnalyzerData();
protected:
	// 文件操作
	ifstream inFile;	
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
	// 将字符串容器内的字符串转换为逻辑分析中对应的数字格式数据
	void vecStr2Digital(const vector<string>* vecStr, LADataPer* dataPer);
	// 将该通道的数据输出为单个文件，并压缩时间轴（0,1,0,1...）
	void makeSubGroup(unsigned int chan);
	// getline读取时跳过非数字项，返回第一个数字项的字符串（strLine）
	string skip2Digital(ifstream* pInFile);
};

