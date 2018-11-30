// coder.cpp : 定义控制台应用程序的入口点 YJY 。
//

#include "stdafx.h"
#include <fstream>
#include <iostream>
#include <cassert>
#include <string>
#include "coder.h"
#include <vector>
#include <sstream> //多定义的类型则用于读写存储在内存中的string对象
#include <Windows.h>

using namespace std;

string readTxt(const string file);
void readCSV(const string file, vector<vector<string>> *pVecDataArray);
void writeCSV(const string file,const vector<vector<string>> *pVecDataArray);
void writeCSV(const string file, vector<vector<DataSeted>> *pVecDataArray, unsigned int dataNum);
void writeCSV(const string file, CoderData *pCoderData,						// 单组写入
	unsigned int channel, unsigned int frame);

void makeDataGroups(const string data, DataGroups * dataGroups, unsigned int dataNum);
void display1(vector<vector<DataSeted>> *pDataSeted, unsigned int dataNum);		// 显示用
void display2(CoderData *pCoderData, unsigned int length);		// 显示用

bool isNumber(const char *str);				// 判断字符串是否是数字
void ifPush2VecData(vector<vector<string>> *pVecDataArray, unsigned char push2VecFlag[], unsigned int timeAxisNum);
void setCoderDataNum(unsigned char* push2VecFlag);			// 设定iDataNum是否增加的函数

// 获得编码器输出在指定位置的值
unsigned char getCoderDataOfNAxis(vector<vector<string>> *pVecDataArray, unsigned int iPos);
// 指定位置的数据压入容器
void push2VecData(vector<CoderData> *nAxisCoderData, unsigned char push2VecFlag[], unsigned int iDataNum);
// 编码器字符串转化为数字
void makeCoderData2Digital(vector<vector<string>> *pVecDataArray, 
	vector<CoderData2Digital> *pCoderData2Digital);

// 编码器数据按轴分类
void getDataOfNaxisTotal(vector<CoderData2Digital> *pCoderData2Digital, 
	vector<float> *pTimeAxis, NAxisPhase pAxisPhaseTotal[]);

// 字符串按二进制读入
unsigned char string2Bin(const string str);

// 对编码器输出的数据进行滤波的，可以让threshold_us=25（25us的毛刺滤掉）
void filterCoderData(vector<float> *pTimeAxis, NAxisPhase nAxisPhaseTotal[], float threshold_us);

// 对编码器数据进行分组
void makeCoderData2Group(vector<float> *pTimeAxis, NAxisPhase nAxisPhaseTotal[], 
	vector<CoderData2Digital> *pCoderData2Digital, CoderData *pCoderDataGrouped);

int main()
{
	const string testFile = "H:\\work\\projects\\STM32\\OIMRobot-400F407-3_20181119_Coder_Young\\Feedback_information.txt";
	const string file = "H:\\work\\projects\\VS\\Coder\\coder\\resouce\\DataSeted.txt";		// 设定参数
	// 实测数据路径
	const string fileCSV_Test = "H:\\work\\projects\\VS\\Coder\\coder\\resouce\\Time_RS.csv";
	const string fileCSV[3] = { "H:\\work\\projects\\VS\\Coder\\coder\\resouce\\Time_RS_X.csv",
								"H:\\work\\projects\\VS\\Coder\\coder\\resouce\\Time_RS_Y.csv",
								"H:\\work\\projects\\VS\\Coder\\coder\\resouce\\Time_RS_Z.csv" };
		

	string sDataFile;
	sDataFile = readTxt(file);
//	int dataNum = sDataFile.size(); 
	unsigned int dataNum = sDataFile.size() % 270;	// 数据必须为270的倍数
	if (0 != dataNum)		// 判断数据是否越界
	{
		cout << "数据不对，请检查" << endl;	
		return 0;
	}
	dataNum = ((sDataFile.size() - 1) / 3 + 1) / 90;	// 有多少组数据
	cout << "设置参数部分：" << endl;
	cout << "共有" << dataNum << "组数据！！！" << endl;

	DataGroups *dataGroups = new DataGroups[dataNum];	// 注意后面要释放
	makeDataGroups(sDataFile, dataGroups, dataNum);
	
	vector<vector<DataSeted>> dataSeted;					// 将数据放入容器，便于动态管理大小
	dataSeted.resize(3, vector<DataSeted>(dataNum));		// row：x、y、z轴； col：多少组数据
	// 赋值
	DataSeted dataSetedTemp;							
	for (size_t i = 0; i < dataNum; i++)
	{
		dataSetedTemp.sData2classData(dataGroups[i].xAxisData);		// x轴数据
		dataSeted[0][i] = dataSetedTemp;

		dataSetedTemp.sData2classData(dataGroups[i].yAxisData);		// y轴数据
		dataSeted[1][i] = dataSetedTemp;

		dataSetedTemp.sData2classData(dataGroups[i].zAxisData);		// z轴数据
		dataSeted[2][i] = dataSetedTemp;
	}
	delete[] dataGroups;							// 没用了，释放掉

	writeCSV(file, &dataSeted, dataNum);			// 将设定的参数写入CSV文件

//	display1(&dataSeted, dataNum);
	cout << endl;

	cout << "/////////////////////////////////////////////////////////////////" << endl << endl;;
	cout << "实测部分：" << endl;

	// csv文件操作，实测部分
	vector<vector<string>> vecDataArray;	// 保存CSV数据的结构
	readCSV(fileCSV_Test, &vecDataArray);
	cout << "共有数据：" << vecDataArray.size() << endl << endl;

	// string 转化为 digital
	vector<CoderData2Digital> coderData2Digital;
	makeCoderData2Digital(&vecDataArray, &coderData2Digital);
	releaseVec<vector<vector<string>>>(&vecDataArray);			// 释放 vecDataArray 的空间

	// 数据按轴分开
	vector<float> timeAxis;		// 时间轴的数据
	NAxisPhase nAxisPhaseTotal[6];	//  存放各轴的数据
	getDataOfNaxisTotal(&coderData2Digital, &timeAxis, nAxisPhaseTotal);

	// 各轴滤波
	filterCoderData(&timeAxis, nAxisPhaseTotal, (float)PULES_WIDTH_THRESHOLD);	

	// 对数据进行分组
	CoderData coderData;		// 数据分组
	makeCoderData2Group(&timeAxis, nAxisPhaseTotal, &coderData2Digital, &coderData);
	releaseVec<vector<float>>(&timeAxis);
	releaseVec<vector<CoderData2Digital>>(&coderData2Digital);	// 释放 coderData2Digital 的空间
	// 释放 nAxisPhaseTotal 的内存，  还没写
//	display2(&coderData, 6);
	
	// 数据写入CSV文件观察
	writeCSV(fileCSV_Test, &coderData, 0, 1);		// 0通道0帧输出看看

	// 数据比较，主要是 dataSeted 和 coderData 的比较， 先看距离
	string pStrTemp;
	compareSetedCoder(&dataSeted, &coderData, 1, 0, 1, &pStrTemp);		// X轴0通道
	writeCSV(fileCSV_Test, &pStrTemp);






	

	return 0;
}

// 显示数据
void display2(CoderData *pCoderData, unsigned int length)
{
	cout << endl << "/////////////////////////////////////////" << endl;
	for (size_t i = 0; i < length; i++)
	{
		unsigned int size = (*pCoderData).nAxisValue[i].size();
		if (0 == size)
		{
			continue;
		}
		cout << "第" << i << "通道数据：" << endl;		
		for (size_t j = 0; j < size; j++)
		{
			unsigned int sizeVec = (*pCoderData).nAxisValue[i][j].timeIndex.size();
			cout << "第" << j + 1 << "帧数据： start = " << (int)(*pCoderData).nAxisValue[i][j].start << endl;
			for (size_t k = 0; k < sizeVec; k++)
			{
				cout << (*pCoderData).timeAxis[(*pCoderData).nAxisValue[i][j].timeIndex[k]] << "--";
			}
			cout << endl << endl;
		}
	}
}

// 获得一个二进制数在某一位的值
inline unsigned char getBinValue(const unsigned char value, unsigned int iPos)
{
	unsigned char result;
	result = (value & (0b1 << (iPos - 1))) >> (iPos - 1);
	return result;
}

// 编码器数据按轴分类
void getDataOfNaxisTotal(vector<CoderData2Digital> *pCoderData2Digital,
	vector<float> *pTimeAxis, NAxisPhase pAxisPhaseTotal[])
{
	unsigned int size = pCoderData2Digital->size();
	// 初始化开始值
	pTimeAxis->push_back((*pCoderData2Digital)[0].timeAxis);
	for (size_t i = 0; i < 6; i++)			// 2相3轴，6个数据
	{
		pAxisPhaseTotal[i].start = getBinValue((*pCoderData2Digital)[0].nAxisValue, (6 - i));
	}	

	for (size_t i = 1; i < size; i++)
	{
		pTimeAxis->push_back((*pCoderData2Digital)[i].timeAxis);
		
		for (size_t j = 0; j < 6; j++)		// 2相3轴，6个数据
		{
			unsigned char preValue = getBinValue((*pCoderData2Digital)[i-1].nAxisValue, (6-j));
			unsigned char curValue = getBinValue((*pCoderData2Digital)[i].nAxisValue, (6 - j));
			if (preValue != curValue)
			{
				pAxisPhaseTotal[j].timeIndex.push_back(i);
			}
		}
	}
	cout << endl;
}

// 手动对vector进行赋值,目标容器会先被释放内存
void assignVec(vector<int> *pDest, vector<int> *pSrc, unsigned int start, unsigned int end)
{
	unsigned int size = pSrc->size();
	if ( (start > end) || ((end - start) > size) )
	{
		cout << "VECTOR赋值有误，请检查！" << endl;
	}
	else
	{
		releaseVec(pDest);
		for (size_t i = start; i < end; i++)
		{
			pDest->push_back((*pSrc)[i]);
		}
	}
}

// 对编码器数据进行分组
void makeCoderData2Group(vector<float> *pTimeAxis, NAxisPhase nAxisPhaseTotal[], 
	vector<CoderData2Digital> *pCoderData2Digital, CoderData *pCoderDataGrouped)
{
	cout << "开始分组：" << endl;
	pCoderDataGrouped->setTimeAxis(pTimeAxis);
	
	for (size_t i = 0; i < 6; i++)		// 2相3轴 
	{
		unsigned int size = nAxisPhaseTotal[i].timeIndex.size();		
		if (10 > size)										// 如果数据小于一定量，可以直接删除
		{
			releaseVec<vector<int>>(&(nAxisPhaseTotal[i].timeIndex));
			continue;
		}
		cout << "第" << i + 1 << "轴数据的原始尺寸为：" << size << endl;

		unsigned int iDataNum = 0;
		unsigned int length = 0;
		NAxisPhase nAxisPhaseTemp;
		for (size_t j = 0; j < size-1; j++)
		{
			nAxisPhaseTemp.timeIndex.push_back(nAxisPhaseTotal[i].timeIndex[j]);
			float timeAxisTemp = (*pTimeAxis)[nAxisPhaseTotal[i].timeIndex[j + 1]] -
				(*pTimeAxis)[nAxisPhaseTotal[i].timeIndex[j]];
			if (GROUPED_TIME < timeAxisTemp)
			{
				// 先判断长度是否合适
				length = nAxisPhaseTemp.timeIndex.size();
				if (5 > length)		// 数据太少不记入
				{
					cout << "第" << iDataNum+1 << "组的数据太少，请检查！" << endl;
					releaseVec(&(nAxisPhaseTemp.timeIndex));	// 释放掉
//					iDataNum++;
					continue;
				}	

				// start 值确定
				unsigned int index = nAxisPhaseTotal[i].timeIndex[j + 1 - length];
				nAxisPhaseTemp.start = getBinValue((*pCoderData2Digital)[index].nAxisValue, (6-i));

				// 压入容器
				(*pCoderDataGrouped).nAxisValue[i].push_back(nAxisPhaseTemp);
				releaseVec(&(nAxisPhaseTemp.timeIndex));

				iDataNum++;
			}
		}

		// 最后一组数据写入
		nAxisPhaseTemp.timeIndex.push_back(nAxisPhaseTotal[i].timeIndex[size-1]);
		// 先判断长度是否合适
		length = nAxisPhaseTemp.timeIndex.size();
		if (5 > length)		// 数据太少不记入
		{
			cout << "第" << iDataNum + 1 << "组的数据太少，请检查！" << endl;
			releaseVec(&(nAxisPhaseTemp.timeIndex));	// 释放掉
			//iDataNum++;
			continue;
		}
		// start 值确定
		unsigned int index = nAxisPhaseTotal[i].timeIndex[size - length];
		nAxisPhaseTemp.start = getBinValue((*pCoderData2Digital)[index].nAxisValue, (6 - i));
		// 压入容器
		(*pCoderDataGrouped).nAxisValue[i].push_back(nAxisPhaseTemp);
		releaseVec(&(nAxisPhaseTemp.timeIndex));

		iDataNum++;

		cout << "分成了" << iDataNum << "组!" << endl;
		cout << endl;
	}

	// 打印
//	for (size_t i = 0; i < 50/*sizeOfTime*/; i++)
//	{
////		cout << (*pCoderDataGrouped).timeAxis[i] << endl;
//		cout << pCoderDataGrouped->nAxisValue[0][0].timeIndex[i] << endl;
//	}
}

// 字符串按二进制读入
unsigned char string2Bin(const string str)
{
	unsigned char temp = 0b0;	
	for (size_t i = 0; i < str.size(); i++)
	{
		if ('1' == str[i])
		{
			temp |= (0b1 << (str.size() - i - 1));
		}
		else if ('0' == str[i])
		{

		}
		else
		{
			cout << "二进制数据有误！" << endl;
			temp = 0b10000000;
			break;
		}
	}


	return temp;
}

// 编码器字符串转化为数字
void makeCoderData2Digital(vector<vector<string>> *pVecDataArray,
	vector<CoderData2Digital> *pCoderData2Digital)
{
	//  删掉vecDataArray负数和非数字的字符串
	unsigned int iCursor = 0;
	while ( !isNumber((*pVecDataArray)[iCursor][0].c_str()) || '-' == (*pVecDataArray)[iCursor][0][0] )
		iCursor++;
	(*pVecDataArray).erase((*pVecDataArray).begin(), (*pVecDataArray).begin() + iCursor);

	// 数据转换
	CoderData2Digital data2DigTemp;
	for (size_t i = 0; i < pVecDataArray->size(); i++)
	{
		data2DigTemp.timeAxis = (float)atof((*pVecDataArray)[i][0].c_str());
		data2DigTemp.nAxisValue = getCoderDataOfNAxis(pVecDataArray, i);
		pCoderData2Digital->push_back(data2DigTemp);

		/*cout << (*pCoderData2Digital)[i].timeAxis << ":";
		cout << (int)(*pCoderData2Digital)[i].nAxisValue << endl;*/
	}
}

// 判断字符串是否是数字
bool isNumber(const char *str)				
{
	float f;
	int n = sscanf_s(str, "%f", &f);
	return n != 0;
}

// 读取txt文件的函数
string readTxt(const string file)		
{	
	ifstream infile;
	infile.open(file);
	assert(infile.is_open());		// 打开失败

	string data;
	getline(infile, data);

	infile.close();

	return data;
}

// 读取csv文件的函数
void readCSV(const string file, vector<vector<string>> *pVecDataArray)
{
	ifstream infile;
	infile.open(file, ios::in);
	assert(infile.is_open());		// 打开失败

	string lineStr;
	while (getline(infile, lineStr))
	{
		// 存成二维表结构
		stringstream ss(lineStr);
		string str;
		vector<string> lineArray;
		// 按照逗号隔开
		while (getline(ss, str, ','))
			lineArray.push_back(str);
		pVecDataArray->push_back(lineArray);	// 每一行vector数据都放到vecDataArray中去
	}
	infile.close();

	 //输出结果
	//for (size_t i = /*(*pVecDataArray).size()*/0; i<50; i++)
	//{
	//	for (size_t j = 0; j<(*pVecDataArray)[i].size(); j++)
	//		cout << (*pVecDataArray)[i][j] << "  ";
	//	cout << endl;
	//} 
}

// 将设定参数解析为字符串进行输出 rs:第一级转速，lastTime：最后运行的时间，
string dataProc2outFile(unsigned int &timeNum, float rs[3], float lastTime)
{
	timeNum--;				// 注意这里最后时间要减一点，实际可能跑不完
	string strTemp;
	strTemp = to_string(lastTime + SETED_DATA_PER_TIME * timeNum);
	strTemp += ",";
	strTemp += to_string(rs[0]);		// x轴
	strTemp += ",";
	strTemp += to_string(rs[1]);		// y轴
	strTemp += ",";
	strTemp += to_string(rs[2]);		// z轴
	strTemp += '\n';			// 回车符

	strTemp += to_string(lastTime + SETED_DATA_PER_TIME * timeNum++);
	strTemp += ",";
	strTemp += to_string(0);		// x轴
	strTemp += ",";
	strTemp += to_string(0);		// y轴
	strTemp += ",";
	strTemp += to_string(0);		// z轴
	strTemp += '\n';			// 回车符

	return strTemp;
}
string dataProc2outFile(unsigned int &timeNum, float rs[3])
{
	string strTemp;
	strTemp = to_string(SETED_DATA_PER_TIME * timeNum);
	strTemp += ",";
	strTemp += to_string(0);		// x轴
	strTemp += ",";
	strTemp += to_string(0);		// y轴
	strTemp += ",";
	strTemp += to_string(0);		// z轴
	strTemp += '\n';			// 回车符

	strTemp += to_string(SETED_DATA_PER_TIME * timeNum++);
	strTemp += ",";
	strTemp += to_string(rs[0]);		// x轴
	strTemp += ",";
	strTemp += to_string(rs[1]);		// y轴
	strTemp += ",";
	strTemp += to_string(rs[2]);		// z轴
	strTemp += '\n';			// 回车符

	return strTemp;
}
string dataProc2outFile(unsigned int &timeNum, unsigned int cnt)
{
	string strTemp;
	// 时间轴之间的空格，便于观察
	for (size_t k = 0; k < cnt; k++)
	{
		strTemp = to_string(SETED_DATA_PER_TIME * timeNum++);			// 时间轴递增
		strTemp += ",";
		strTemp += to_string(0);										// x轴的转速
		strTemp += ",";
		strTemp += to_string(0);										// y轴的转速
		strTemp += ",";
		strTemp += to_string(0);										// z轴的转速
		strTemp += "\n";
	}
	return strTemp;
}
string dataProc2outFile(vector<vector<DataSeted>> *pVecDataArray, unsigned int iDataNum,
	unsigned char infPointNum, unsigned int &timeNum)
{
	string strTemp = "";
	unsigned char length = (infPointNum - 4) / 4;		//  拐点的计算方法
	float rsTemp;										// 转速
	// 上升段
	for (size_t i = 0; i < length; i++)					
	{
		//  第一个点
		strTemp += to_string(SETED_DATA_PER_TIME * timeNum);
		strTemp += ",";
		rsTemp = (*pVecDataArray)[0][iDataNum].setedClk2RS(i);	// x轴
		strTemp += to_string(rsTemp);
		strTemp += ",";
		rsTemp = (*pVecDataArray)[1][iDataNum].setedClk2RS(i);	// y轴
		strTemp += to_string(rsTemp);
		strTemp += ",";
		rsTemp = (*pVecDataArray)[2][iDataNum].setedClk2RS(i);	// z轴
		strTemp += to_string(rsTemp);
		strTemp += "\n";

		// 第二个点
		strTemp += to_string(SETED_DATA_PER_TIME * timeNum++);
		strTemp += ",";
		rsTemp = (*pVecDataArray)[0][iDataNum].setedClk2RS(i+1);	// x轴
		strTemp += to_string(rsTemp);
		strTemp += ",";
		rsTemp = (*pVecDataArray)[1][iDataNum].setedClk2RS(i+1);	// y轴
		strTemp += to_string(rsTemp);
		strTemp += ",";
		rsTemp = (*pVecDataArray)[2][iDataNum].setedClk2RS(i+1);	// z轴
		strTemp += to_string(rsTemp);
		strTemp += "\n";
	}	

	// 中间段
	timeNum += ((*pVecDataArray)[0][iDataNum].getConstTime()-1);	// 这里没有问题，三个轴的ConstTime是一样的

	// 下降段
	for (size_t i = length; i > 0; i--)					// 上升段
	{
		//  第一个点
		strTemp += to_string(SETED_DATA_PER_TIME * timeNum);
		strTemp += ",";
		rsTemp = (*pVecDataArray)[0][iDataNum].setedClk2RS(i);	// x轴
		strTemp += to_string(rsTemp);
		strTemp += ",";
		rsTemp = (*pVecDataArray)[1][iDataNum].setedClk2RS(i);	// y轴
		strTemp += to_string(rsTemp);
		strTemp += ",";
		rsTemp = (*pVecDataArray)[2][iDataNum].setedClk2RS(i);	// z轴
		strTemp += to_string(rsTemp);
		strTemp += "\n";

		// 第二个点
		strTemp += to_string(SETED_DATA_PER_TIME * timeNum++);
		strTemp += ",";
		rsTemp = (*pVecDataArray)[0][iDataNum].setedClk2RS(i-1);	// x轴
		strTemp += to_string(rsTemp);
		strTemp += ",";
		rsTemp = (*pVecDataArray)[1][iDataNum].setedClk2RS(i-1);	// y轴
		strTemp += to_string(rsTemp);
		strTemp += ",";
		rsTemp = (*pVecDataArray)[2][iDataNum].setedClk2RS(i-1);	// z轴
		strTemp += to_string(rsTemp);
		strTemp += "\n";
	}

	return strTemp;
}

// 设定参数写入CSV文件
void writeCSV(const string file, vector<vector<DataSeted>> *pVecDataArray, unsigned int dataNum)
{
	//  文件名构造
	// H:\\work\\projects\\VS\\Coder\\coder\\resouce\\DataSeted.txt
	string fileName = file.substr(0, (file.size() - 4));
	fileName.insert(fileName.find_last_of("\\"), "\\output");
	fileName += "_proc.csv";
	ofstream outFile;
	outFile.open(fileName, ios::out);
	assert(outFile.is_open());		// 打开失败

	outFile << "Time" << "," << "X" << "," << "Y" << "," << "Z" << endl;
	// 将设定参数解析后写入容器
	unsigned int timeNum = 0;			// 时间轴，重要
	vector<string> vecSetDataArray;
	DataSeted* pMaxSetedData;				// 定义最大的参数，作为计算标准
	for (size_t i = 0; i < dataNum; i++)
	{
		// 对每组的 DataSeted 数据进行预处理后才能写入CSV文件
		for (size_t j = 0; j < 3; j++)							// 检查三个轴的数据是否规范
			if ( !((*pVecDataArray)[j][i].checkSetedData()) )
			{
				cout << "第" << i + 1 << "组数据有误，请检查！" << endl;
				return;
			}
		pMaxSetedData = getMaxSetedData(&(*pVecDataArray)[0][i], 
			&(*pVecDataArray)[1][i], &(*pVecDataArray)[2][i]);

		// 计算拐点数
		unsigned char inflectionPoint = 0;
		for (size_t j = 0; j < 8; j++)
		{
			if (0 == (*pMaxSetedData).addSubClkSet[j])
				break;
			else
				inflectionPoint++;
		}
		if (0 == inflectionPoint || 32/4 < inflectionPoint)	// 拐点数/4 必须在(0,8]之间
		{
			cout << "第" << i+1 << "组拐点数量计算有误" << endl;
			outFile.close();
			return;
		}

		inflectionPoint *= 4;

		// 初级转速
		float rsTemp[3];		// 存放x、y、z设定的第一级转速
		for (size_t j = 0; j < 3; j++)
			rsTemp[j] = (*pVecDataArray)[j][i].setedClk2RS(0);
		// 最后一步的持续时间
		float lastTime;
		lastTime = (*pMaxSetedData).calSubLastTime();		

		// 每组数据解析后放入文件流
		string strTemp;
		// 开始两个点
		strTemp = dataProc2outFile(timeNum, rsTemp);
		outFile << strTemp;

		// 中间
		strTemp = dataProc2outFile(pVecDataArray, i, inflectionPoint, timeNum);
		outFile << strTemp;

		// 最后两个点
		strTemp = dataProc2outFile(timeNum, rsTemp, lastTime);
		outFile << strTemp;

		// 留一段时间轴空白
		strTemp = dataProc2outFile(timeNum, 30);
		outFile << strTemp;	
	}

	outFile.close();
}

// 实测数据写入csv文件的函数
void writeCSV(const string file, const vector<vector<string>> *pVecDataArray)
{
	//  文件名构造
	string fileName = file.substr(0, (file.size()-4));
	fileName += "_filter.csv";
	ofstream outFile;
	outFile.open(fileName, ios::out);
	assert(outFile.is_open());		// 打开失败

	const size_t row = (*pVecDataArray).size();
	const size_t clo = (*pVecDataArray)[0].size();		// 这里可能有问题，先不改 byYJY

	size_t i, j;
	for (i = 0; i < row; i++)
	{
		for (j = 0; j < clo-1; j++)
		{
			outFile << (*pVecDataArray)[i][j] << ",";
		}
		outFile << (*pVecDataArray)[i][j] << endl;
	}
	outFile.close();
}

void writeCSV(const string file, const CoderData *pCoderData, unsigned int black)	// 总的写入	
{

}

void writeCSV(const string file, CoderData *pCoderData,						// 单组写入
	unsigned int channel, unsigned int frame)
{
	//  文件名构造
	string fileName = file.substr(0, (file.size() - 4));
	fileName.insert(fileName.find_last_of("\\"), "\\output");
	fileName += "_c" + to_string(channel) + "_f" + to_string(frame+1) + ".csv";
	ofstream outFile;
	outFile.open(fileName, ios::out);
	assert(outFile.is_open());		// 打开失败

	outFile << "Time" << "," << ("_c" + to_string(channel) + "_f" + to_string(frame+1)) << endl;
	NAxisPhase nAxisPhase = (*pCoderData).nAxisValue[channel][frame];
	unsigned int length = nAxisPhase.timeIndex.size();

	for (size_t i = 0; i < length; i++)
	{
		unsigned int index = nAxisPhase.timeIndex[i];
		float rsTemp = (*pCoderData).getCoderRS(channel, frame, i);
		outFile << (*pCoderData).timeAxis[index] << "," << rsTemp << endl;	// byYJY
	}
	outFile.close();
}

// 对读取的数据进行分组
void makeDataGroups(const string data, DataGroups * dataGroups, unsigned int dataNum)
{
	for (size_t i = 0; i < dataNum; i++)
	{
		for (size_t j = 0; j < N_AXIS_SIZE_STRING; j++)
		{
			dataGroups[i].xAxisData[j] = 
				data[0 + j + i * 3 * N_AXIS_SIZE_STRING];						// x轴
			dataGroups[i].yAxisData[j] = 
				data[N_AXIS_SIZE_STRING + j + i * 3 * N_AXIS_SIZE_STRING];		// y轴
			dataGroups[i].zAxisData[j] = 
				data[2 * N_AXIS_SIZE_STRING + j + i * 3 * N_AXIS_SIZE_STRING];	// z轴
		}
	}
}

void display1(vector<vector<DataSeted>> *pDataSeted, unsigned int dataNum)
{
	for (size_t i = 0; i < dataNum; i++)
	{
		cout << "第%d组： " << i+1 << endl;
		cout << "x轴： " << endl;
		(*pDataSeted)[0][i].displayDataSeted();
		cout << "y轴： " << endl;
		(*pDataSeted)[1][i].displayDataSeted();
		cout << "z轴： " << endl;
		(*pDataSeted)[2][i].displayDataSeted();
		cout << endl;
	}
}

// 对编码器输出的数据进行滤波的，可以让threshold_us=25（25us的毛刺滤掉）
// 滤波不能处理最后两个数据，回头再修改 byYJY
void filterCoderData(vector<float> *pTimeAxis, NAxisPhase nAxisPhaseTotal[], float threshold_us)
{
	for (size_t i = 0; i < 6; i++)	// 2相3轴
	{
		unsigned int size = nAxisPhaseTotal[i].timeIndex.size();
		if (10 > size)										// 如果数据小于一定量，可以直接删除
		{
			releaseVec<vector<int>>(&(nAxisPhaseTotal[i].timeIndex));
			continue;
		}
			
		cout << "滤波前数据量:" << size << endl;
		// 开始滤波
		cout << "正在滤波...";
		vector<int>::iterator iter = nAxisPhaseTotal[i].timeIndex.begin();
		unsigned int cursor = 0;

		while (iter < (nAxisPhaseTotal[i].timeIndex.end() - 2 ))
		{
			// 转为us，方便比较--- 小心越界 byYJY
			float timeAxisTemp = (float)((((*pTimeAxis)[nAxisPhaseTotal[i].timeIndex[cursor + 1]])
				- ((*pTimeAxis)[nAxisPhaseTotal[i].timeIndex[cursor]])) * 1e6);	

			if (threshold_us > timeAxisTemp)
			{
				nAxisPhaseTotal[i].timeIndex.erase(iter, iter + 2);
			}
			else {
				cursor++;
			}

			iter = nAxisPhaseTotal[i].timeIndex.begin() + cursor;
		}
		cout << endl;
		cout << "滤波后数据量还有： " << nAxisPhaseTotal[i].timeIndex.size() << endl;
		cout << endl;
	}		
}

// 获得编码器输出在指定位置的值
unsigned char getCoderDataOfNAxis(vector<vector<string>> *pVecDataArray, unsigned int iPos)
{
	unsigned char nAxisValue = 0b10000000;
	if (iPos < 0 || iPos >= pVecDataArray->size())
	{
		cout << "获得编码器数据时，位置超出范围，请检查！" << endl;
		return nAxisValue;
	}

	string strTemp = "";
	for (size_t i = 1; i < (*pVecDataArray)[iPos].size(); i++)
			strTemp += (*pVecDataArray)[iPos][i][1];			// 第0个是空格
	//while (6 != strTemp.size())		// 3轴，每轴A、B相，共6个
	//	strTemp += "0";
	
	nAxisValue = string2Bin(strTemp);

	return nAxisValue;
}

// 判断实测数据是否可以压入容器的函数，重要！
void ifPush2VecData(vector<vector<string>> *pVecDataArray,
	unsigned char push2VecFlag[], unsigned int timeAxisNum)
{
	unsigned char prevValues = getCoderDataOfNAxis(pVecDataArray, timeAxisNum - 1);	//  获得前一个数据
	unsigned char curValues = getCoderDataOfNAxis(pVecDataArray, timeAxisNum);		// 获得当前数据
	unsigned char nextValues = getCoderDataOfNAxis(pVecDataArray, timeAxisNum + 1);	// 获得后一个数据

	unsigned char judgeTemp = 0;

	judgeTemp = curValues ^ prevValues;
	switch (judgeTemp)
	{
	case 0b100000:		// Xa 有资格

		break;
	case 0b010000:

		break;

		///////////还有其它情况的判断  byYJY

	default:
		break;
	}

	if (curValues ^ prevValues)		// 得出有跳变的位置
	{

	}

}

// 指定位置的数据压入容器
void push2VecData(vector<CoderData> *nAxisCoderData, unsigned char push2VecFlag[], unsigned int iDataNum[])
{

	switch (1)
	{
	////////////后面的还要写///////// byYJY

	default:
		break;
	}

	
}

void setCoderDataNum(unsigned char* push2VecFlag)			// 设定iDataNum是否增加的函数
{

}



