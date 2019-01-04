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
//	cout << "析构函数！" << endl;
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
	return true;
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
// 使用笨办法，先将数据拆分为文件，再进行组合
void LogicAnalyzerData::filtering(unsigned int threshold)
{
	openFile();
	// 输出文件流
	string fileNameFilter = fileRename(fileName, "filter");
	ofstream outFile;
	outFile.open(fileNameFilter, ios::out);


	// 滤波中……
#if _DISPLAY_SCHEDULE
	cout << "滤波中： 00%";
	const size_t steps = lineNum / 100;
	size_t curPos = 0;
	size_t stepsCnt = 0;
#else
	cout << "滤波中..." << endl;
#endif

	// 第一行数据处理
	string strLine1st;
	vector<string> vecStr1st;
	do
	{
		// 清空容器，数据量小，不需考虑释放内存
		vecStr1st.clear();
		getline(inFile, strLine1st);
		cutStrByChar(strLine1st.c_str(), ',', &vecStr1st);		// 注意字符串多一个空格
#if _DISPLAY_SCHEDULE
		// 显示进度用的 （这里有个BUG，但不管它）
		curPos++;
		stepsCnt++;
#endif
	} while (!(isNumber(vecStr1st[0].c_str())));
	// 第一行数据放入数组中保存
	LADataPer dataPer1st;
	vecStr2Digital(&vecStr1st, &dataPer1st);
	// 将数据保存进动态数组，以便进行比较
	timeAndChanValue *dataPre = new timeAndChanValue[channel];
	for (size_t i = 0; i < channel; i++)
	{
		dataPre[i].time = dataPer1st.time;
		dataPre[i].chanValue = dataPer1st.chanValue[i];
	}

	// 读取下一组数据，并依此判断上一组数据写入前是否更改
	string strLine;
	while (getline(inFile, strLine))
	{
#if _DISPLAY_SCHEDULE
		// 显示进度用的
		curPos++;
		stepsCnt++;
		if (steps == stepsCnt)
		{
			displaySchedules(curPos, lineNum);
			stepsCnt = 0;
		}
#endif
		// 分割字符串
		vector<string> vecStr;				// 注意初始化清空容器
		cutStrByChar(strLine.c_str(), ',', &vecStr);
		// 字符串数字化
		LADataPer dataPerLine;
		vecStr2Digital(&vecStr, &dataPerLine);

		// 各通道数据比较 
		for (size_t i = 0; i < channel; i++)
		{
			if (dataPerLine.chanValue[i] != dataPre[i].chanValue)
			{
				// 小于阈值的滤掉
				double diff = ((dataPerLine.time - dataPre[i].time)*1e6);	// 单位us
				if (diff < threshold)
				{
					if (0 == dataPre[i].chanValue)
					{
						strReplace(&strLine1st, i, ',', '1');
					}
					else if(1 == dataPre[i].chanValue)
					{
						strReplace(&strLine1st, i, ',', '0');
					}
					else
					{
						strLine1st = "x,x,x";
					}
				}
				else
				{
					// 处理完后更新 dataPre 存放的值
					dataPre[i].time = dataPerLine.time;
					dataPre[i].chanValue = dataPerLine.chanValue[i];
				}
			}
		}

		// 处理完后写入outfile
		outFile << strLine1st << endl;
		// 更新 前一个 strLine1st
		strLine1st = strLine;
	}
	// 写入最后一组数据(最后一组数据不做修改)
	outFile << strLine1st << endl;

#if _DISPLAY_SCHEDULE
	// 显示进度
	cout << "\b\b\b" << "100%" << endl;
#endif

	// 释放动态数组
	delete[] dataPre;
	// 关闭文件流
	outFile.close();
	closeFile();

	// 重定位类的fileName成员
	fileName = fileNameFilter;
}


// 将字符串容器内的字符串转换为逻辑分析中对应的数字格式数据
void LogicAnalyzerData::vecStr2Digital(const vector<string>* vecStr, LADataPer* dataPer)
{
	// 数据放入结构体
	dataPer->time = strtod((*vecStr)[0].c_str(), nullptr);					// 时间轴
	for (size_t i = 1; i < (*vecStr).size(); i++)		// 高低电平值
	{
		if (0 == stoi((*vecStr)[i]))
		{
			dataPer->chanValue.push_back(low);
		}
		else if (1 == stoi((*vecStr)[i]))
		{
			dataPer->chanValue.push_back(high);
		}
		else
		{
			cout << "高低电平值数据有误，请检查！" << endl;
		}
	}
}


// 将该通道的数据输出为单个文件，并压缩时间轴（0,1,0,1...）
void LogicAnalyzerData::makeSubGroup(unsigned int chan)
{
	openFile();

	// 文件名构造
	string fileNameFilter = "filter_" + to_string(chan);
	fileNameFilter = fileRename(fileName, fileNameFilter);
	// 打开文件流
	ofstream outFile;
	outFile.open(fileNameFilter);

	string strLine;
	//  第一行数据单独赋值
	vector<string> vecStrPre;
	do
	{
		vecStrPre.clear();				// 清空容器，数据量不大，不用释放内存
		getline(inFile, strLine);
		cutStrByChar(strLine.c_str(), ',', &vecStrPre);
	} while (!(isNumber(vecStrPre[0].c_str())));
	outFile << vecStrPre[0] << ", ";
	outFile << vecStrPre[chan + 1] << endl;

	// 其余数据循环处理 
	while (getline(inFile, strLine))
	{
		// 按逗号分割
		vector<string> vecStr;
		cutStrByChar(strLine.c_str(), ',', &vecStr);
		if (!(isNumber(vecStr[0].c_str())))				// 不是数字就跳出本次循环
			continue;
		// 高低电平变化时再赋值
		if (vecStr[chan + 1] != vecStrPre[chan+1])
		{
			outFile << vecStr[0] << ", ";
			outFile << vecStr[chan + 1] << endl;
			// 更新 vecStrPre 以便下一次比较
			vecStrPre[0] = vecStr[0];
			vecStrPre[chan + 1] = vecStr[chan + 1];
		}			
	}

	outFile.close();
	closeFile();
}


// getline读取时跳过非数字项，返回第一个数字项的字符串（strLine）
string LogicAnalyzerData::skip2Digital(ifstream* pInFile)
{
	string strLine;
	vector<string> vecStr;

	do
	{
		vecStr.clear();		// 清空容器，数据不大，不用释放内存
		getline(inFile, strLine);
		cutStrByChar(strLine.c_str(), ',', &vecStr);		// 注意字符串多一个空格
	} while (!(isNumber(vecStr[0].c_str())));

	return strLine;
}
