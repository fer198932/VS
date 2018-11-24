// coder.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <fstream>
#include <iostream>
#include <cassert>
#include <string>
#include "coder.h"
#include <vector>
#include <sstream> //多定义的类型则用于读写存储在内存中的string对象

using namespace std;

string readTxt(const string file);
void readCSV(const string file, vector<vector<string>> *pVecDataArray);
void writeCSV(const string file,const vector<vector<string>> *pVecDataArray);
void writeCSV(const string file, vector<vector<DataSeted>> *pVecDataArray, unsigned int dataNum);
void makeDataGroups(const string data, DataGroups * dataGroups, unsigned int dataNum);
void display1(vector<vector<DataSeted>> *pDataSeted, unsigned int dataNum);		// 显示用

// 对编码器输出的数据进行滤波的，可以让threshold_us=25（25us的毛刺滤掉）
void filterCoderData(vector<vector<string>> *pVecDataArray, float threshold_us);	

int main()
{
	const string testFile = "H:\\work\\projects\\STM32\\OIMRobot-400F407-3_20181119_Coder_Young\\Feedback_information.txt";
	const string file = "H:\\work\\projects\\VS\\Coder\\coder\\resouce\\DataSeted.txt";		// 设定参数
	const string fileCSV = "H:\\work\\projects\\VS\\Coder\\coder\\resouce\\Time_RS.csv";		// 实测数据

	string sDataFile;
	sDataFile = readTxt(testFile);
//	int dataNum = sDataFile.size(); 
	unsigned int dataNum = sDataFile.size() % 270;	// 数据必须为270的倍数
	if (0 != dataNum)		// 判断数据是否越界
	{
		cout << "数据不对，请检查" << endl;	
		return 0;
	}
	dataNum = ((sDataFile.size() - 1) / 3 + 1) / 90;	// 有多少组数据

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

	display1(&dataSeted, dataNum);
	cout << endl;






	// csv文件操作，实测部分
	vector<vector<string>> vecDataArray;	// 保存CSV数据的结构
	readCSV(fileCSV, &vecDataArray);
//	filterCoderData(&vecDataArray, 1000);			// 对编码器输出的数据进行软件滤波,25us的波去掉
//	writeCSV(fileCSV, &vecDataArray);








//	cout << x << endl;
//	cout << dataNum << endl;

	return 0;
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

	/* //输出结果
	for (size_t i = 0; i<(*pVecDataArray).size(); i++)
	{
		for (size_t j = 0; j<(*pVecDataArray)[i].size(); j++)
			cout << (*pVecDataArray)[i][j] << "  ";
		cout << endl;
	} */
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
	timeNum += ((*pVecDataArray)[0][iDataNum].getConstTime()-1);

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
	string fileName = file.substr(0, (file.size() - 4));
	fileName += "_proc.csv";
	ofstream outFile;
	outFile.open(fileName, ios::out);
	assert(outFile.is_open());		// 打开失败

	outFile << "Time" << "," << "X" << "," << "Y" << "," << "Z" << endl;
	// 将设定参数解析后写入容器
	unsigned int timeNum = 0;			// 时间轴，重要
	vector<string> vecSetDataArray;
	for (size_t i = 0; i < dataNum; i++)
	{
		// 计算拐点数
		unsigned char inflectionPoint = 0;
		for (size_t j = 0; j < 8; j++)
		{
			if (0 == (*pVecDataArray)[0][i].addSubClkSet[j])
				break;
			else
				inflectionPoint++;
		}
		inflectionPoint *= 4;

		// 初级转速
		float rsTemp[3];		// 存放x、y、z设定的第一级转速
		for (size_t j = 0; j < 3; j++)
			rsTemp[j] = (*pVecDataArray)[j][i].setedClk2RS(0);
		// 最后一步的持续时间
		float lastTime;
		lastTime = (*pVecDataArray)[0][i].calSubLastTime();

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

		//// 手动设定开始的两个点(结构体设定得不太好。。。)
		//strTemp = to_string(SETED_DATA_PER_TIME * timeNum);			
		//strTemp += ",";
		//strTemp += to_string(0);										// x轴的转速
		//strTemp += ",";
		//strTemp += to_string(0);										// y轴的转速
		//strTemp += ",";
		//strTemp += to_string(0);										// z轴的转速
		//outFile << strTemp << endl;

		//strTemp = to_string(SETED_DATA_PER_TIME * timeNum++);			// 时间轴递增
		//strTemp += ",";
		//rsTemp = (*pVecDataArray)[0][i].setedClk2RS(0);					// x轴的转速
		//strTemp += to_string(rsTemp);
		//strTemp += ",";
		//rsTemp = (*pVecDataArray)[1][i].setedClk2RS(0);					// y轴的转速
		//strTemp += to_string(rsTemp);
		//strTemp += ",";
		//rsTemp = (*pVecDataArray)[2][i].setedClk2RS(0);					// z轴的转速
		//strTemp += to_string(rsTemp);
		//outFile << strTemp << endl;

		//for (size_t j = 0; j < (inflectionPoint-4)/4; j++)				// 注意拐点的算法
		//{
		//	strTemp = to_string(SETED_DATA_PER_TIME * timeNum);
		//	strTemp += ",";
		//	rsTemp = (*pVecDataArray)[0][i].setedClk2RS(j + 1);
		//	strTemp += to_string(rsTemp);
		//	strTemp += ",";
		//	rsTemp = (*pVecDataArray)[1][i].setedClk2RS(j + 1);
		//	strTemp += to_string(rsTemp);
		//	strTemp += ",";
		//	rsTemp = (*pVecDataArray)[2][i].setedClk2RS(j + 1);
		//	strTemp += to_string(rsTemp);
		//	outFile << strTemp << endl;

		//	strTemp = to_string(SETED_DATA_PER_TIME * timeNum++);
		//	strTemp += ",";
		//	rsTemp = (*pVecDataArray)[0][i].setedClk2RS(j + 1);
		//	strTemp += to_string(rsTemp);
		//	strTemp += ",";
		//	rsTemp = (*pVecDataArray)[1][i].setedClk2RS(j + 1);
		//	strTemp += to_string(rsTemp);
		//	strTemp += ",";
		//	rsTemp = (*pVecDataArray)[2][i].setedClk2RS(j + 1);
		//	strTemp += to_string(rsTemp);
		//	outFile << strTemp << endl;
		//}
		//
		//// 最后两个点
		//strTemp = to_string((*pVecDataArray)[0][i].calSubLastTime() + SETED_DATA_PER_TIME * timeNum);
		//strTemp += ",";
		//rsTemp = (*pVecDataArray)[0][i].setedClk2RS(0);					// x轴的转速
		//strTemp += to_string(rsTemp);
		//strTemp += ",";
		//rsTemp = (*pVecDataArray)[1][i].setedClk2RS(0);					// y轴的转速
		//strTemp += to_string(rsTemp);
		//strTemp += ",";
		//rsTemp = (*pVecDataArray)[2][i].setedClk2RS(0);					// z轴的转速
		//strTemp += to_string(rsTemp);
		//outFile << strTemp << endl;

		//strTemp = to_string((*pVecDataArray)[0][i].calSubLastTime() + 
		//	SETED_DATA_PER_TIME * timeNum++);	// 时间轴递增
		//strTemp += ",";
		//strTemp += to_string(0);										// x轴的转速
		//strTemp += ",";
		//strTemp += to_string(0);										// y轴的转速
		//strTemp += ",";
		//strTemp += to_string(0);										// z轴的转速
		//outFile << strTemp << endl;

		//// 时间轴之间的空格，便于观察
		//for (size_t k = 0; k < 10; k++)
		//{
		//	strTemp = to_string(SETED_DATA_PER_TIME * timeNum++);			// 时间轴递增
		//	strTemp += ",";
		//	strTemp += to_string(0);										// x轴的转速
		//	strTemp += ",";
		//	strTemp += to_string(0);										// y轴的转速
		//	strTemp += ",";
		//	strTemp += to_string(0);										// z轴的转速
		//	outFile << strTemp << endl;
		//}		
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
	const size_t clo = (*pVecDataArray)[0].size();

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
void filterCoderData(vector<vector<string>> *pVecDataArray, float threshold_us)	
{
	// 删除不符合要求的元素，如字符		
	vector<vector<string>>::iterator cursor = pVecDataArray->begin();
	unsigned int iter = 0;
	pVecDataArray->erase(pVecDataArray->begin() + iter); // 第一行是字符，删掉
	cursor = pVecDataArray->begin();

	// 开始滤波
	cout << "正在滤波，等待时间可能有点长...";
	while (cursor < (pVecDataArray->end()-2)) // iter != 20  != (pVecDataArray->end())
	{
		double str2FlTemp1, str2FlTemp2;
		str2FlTemp1 = atof((*pVecDataArray)[iter][0].c_str()) * 1e6;	// 要先将string转为char*,再转为us
		str2FlTemp2 = atof((*pVecDataArray)[iter+2][0].c_str()) * 1e6;
		
		if (str2FlTemp1 < 0 || (str2FlTemp2 - str2FlTemp1) < threshold_us)
		{
			pVecDataArray->erase(cursor, cursor+1);	// vector删除的时候耗费资源很多，如果滤波时间较大，时间会很久
			cout << ".";
		}
		else {
			iter+=2;		// 观察数据，按2递增
		}
//		cout << str2FlTemp1 << endl;
		cursor = pVecDataArray->begin() + iter;
	}

	//输出结果
	//for (size_t i = 0; i<(*pVecDataArray).size(); i++)
	//{
	//	for (size_t j = 0; j<(*pVecDataArray)[i].size(); j++)
	//		cout << (*pVecDataArray)[i][j] << "----";
	//	cout << endl;
	//}
}


