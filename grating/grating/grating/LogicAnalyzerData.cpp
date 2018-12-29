#include "LogicAnalyzerData.h"


LogicAnalyzerData::LogicAnalyzerData(string file)
	: lineNum(0)
	, channel(0)
{
	fileName = file;

	countLineNum();
	countChannel();
}


LogicAnalyzerData::~LogicAnalyzerData()
{
//	inFile.close();
}


// 文件操作
void LogicAnalyzerData::openFile()
{
	inFile.open(fileName, ios::in);

	if (!inFile)
	{
		cout << "文件打开失败！" << endl;
		exit(0);
	}	
}


bool LogicAnalyzerData::closeFile()
{
	inFile.close();
	return false;
}


// 计算文件总的行数
void LogicAnalyzerData::countLineNum()
{
	openFile();

	lineNum = 0;

	// // 经比较，这个方法速度慢很多
	//string temp;
	//while (getline(inFile, temp))
	//{
	//	lineNum++;
	//}

	char c;
	while (inFile.get(c))
	{
		if ('\n' == c)
			lineNum++;
	}

	closeFile();

	if (0 == lineNum)
	{
		cout << "空白文件！" << endl;
		exit(0);
	}
}


// 获得文件总行数
unsigned int LogicAnalyzerData::getLineNum()
{
	return lineNum;
}


// 统计有几个通道，不包括时间轴
void LogicAnalyzerData::countChannel()
{
	openFile();

	channel = 0;			// 保险点，虽然构造函数中已初始化

	string strTemp;
	getline(inFile, strTemp);
	
	unsigned int length = strTemp.length();
	if (0 == length)
	{
		cout << "读取通道数有误！" << endl;
		exit(0);
	}

	// 根据逗号判断通道数
	for (size_t i = 0; i < length; i++)
	{
		if (',' == strTemp[i])
			channel++;
	}
	closeFile();

	if (0 == channel)
	{
		cout << "通道数为0！" << endl;
		exit(0);
	}
}


// 获得通道数
unsigned int LogicAnalyzerData::getChannelNum()
{
	return channel;
}


// 数据滤波，滤波后的将文件另外保存，并更改filename指向更改后的文件
void LogicAnalyzerData::filtering(unsigned int threshold)
{
	openFile();

	/////////////////////////// byYJY

	for (size_t i = 0; i < channel; i++)
	{

	}
}
