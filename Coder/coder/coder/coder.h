#pragma once
#include <string>
#include <vector>

using namespace std;

// 重要的宏定义
#define MAX_DATA_NUM 1024
#define N_AXIS_SIZE 30					// 一个轴包括的字符数
#define N_AXIS_SIZE_STRING 90			// 读入字符串后增加3倍
#define SETED_DATA_PER_TIME	40e-3		// 设定值的时间轴按40ms为单位
#define DUTY_RATIO_THRESHOLD 0.05		// 占空比阈值，小于该值的考虑丢掉
#define PULES_WIDTH_THRESHOLD 25		// 脉冲宽度，单位us，小于该宽度的认为是噪声。已知100kHz时，噪声基本上为10us
#define GROUPED_TIME 1					// 分组的判断参数，单位为秒

bool str_to_hex(const char *string, unsigned int* result, unsigned int len);
void str2strTemp(const char *data, char* dataTemp, unsigned char cursor);

// 彻底释放vector占用的内存的模板函数
template<typename T>
void releaseVec(T *pVec)
{
	T v;
	pVec->clear();
	pVec->swap(v);
//	cout << "vector内存释放成功" << endl;
}

class DataGroups
{
public:
	char xAxisData[N_AXIS_SIZE_STRING];
	char yAxisData[N_AXIS_SIZE_STRING];
	char zAxisData[N_AXIS_SIZE_STRING];
};

class DataSeted
{
private:
	int nAxisPlusNum;
	int nAxisClk;
	int nDir;

	int addSubClkSet[9];
	int subLastTime;
	int constTime;

public:
	DataSeted();
	void sData2classData(const char* data);
	void displayDataSeted(void);
//	void dataSeted2VecArray(vector<string> *pVecSetDataArray, unsigned int timeNum);
	friend void writeCSV(const string file, vector<vector<DataSeted>> *pVecDataArray, unsigned int dataNum);
	float setedClk2RS(unsigned int num);	// 将设定的频率CLK转换为转速
	float calSubLastTime(void);					// 计算最后一步消耗的时间
	int getConstTime(void);						// 返回匀速运动的时间，以40ms为单位
	bool checkSetedData(void);					// 检查数据是否满足规定格式				
	friend DataSeted* getMaxSetedData			// 获得最大的设定参数，以此参数为基准进行计算拐点等参数
		(DataSeted* setedData1, DataSeted* setedData2, DataSeted* setedData3);
};

DataSeted::DataSeted()
{
	nAxisPlusNum = 0;
	nAxisClk = 0;
	nDir = 0;
	memset(addSubClkSet, 0, sizeof(addSubClkSet));
	subLastTime = 0;
	constTime = 0;
}

// 将数据由字符串转换为数字后存入 DataSeted 类
void DataSeted::sData2classData(const char* data)
{
	const unsigned int sizeProc = 4;	// 每次处理4位的16进制数
	char dataTemp[sizeProc];
	unsigned int hexTemp;
	unsigned char cur = 0;

	// nAxisPlusNum 
	str2strTemp(data, dataTemp, cur++);
	str_to_hex(dataTemp, &hexTemp, sizeProc);
	this->nAxisPlusNum = hexTemp;

	str2strTemp(data, dataTemp, cur++);
	str_to_hex(dataTemp, &hexTemp, sizeProc);
	this->nAxisPlusNum += (hexTemp << 16);

	// nAxisClk
	str2strTemp(data, dataTemp, cur++);
	str_to_hex(dataTemp, &hexTemp, sizeProc);
	this->nAxisClk = hexTemp;

	// nDir
	str2strTemp(data, dataTemp, cur++);
	str_to_hex(dataTemp, &hexTemp, sizeProc);
	this->nDir = hexTemp;

	// addSubClkSet[9]
	for (size_t i = 0; i < 9; i++)
	{
		str2strTemp(data, dataTemp, cur++);
		str_to_hex(dataTemp, &hexTemp, sizeProc);
		this->addSubClkSet[i] = hexTemp;
	}

	// subLastTime
	str2strTemp(data, dataTemp, cur++);
	str_to_hex(dataTemp, &hexTemp, sizeProc);
	this->subLastTime = hexTemp;

	// constTime
	str2strTemp(data, dataTemp, cur++);
	str_to_hex(dataTemp, &hexTemp, sizeProc);
	this->constTime = hexTemp;
}

void DataSeted::displayDataSeted(void)
{
	cout << "nAxisPlusNum = " << nAxisPlusNum << endl;
	cout << "nAxisClk = " << nAxisClk << endl;
	cout << "nDir = " << nDir << endl;
//	cout << "addSubClkSet = %d" << addSubClkSet << endl;
	for (size_t i = 0; i < 9; i++)
	{
		cout << "addSubClkSet[" << i << "] = " << addSubClkSet[i] << endl;
	}
	cout << "subLastTime = " << subLastTime << endl;
	cout << "constTime = " << constTime << endl;
}

// 将设定的频率clk转换为点击转速的函数
float DataSeted::setedClk2RS(unsigned int num)
{
	float rs;		// 返回的转速

	// 先简单写
	rs = (float)addSubClkSet[num];			// 已知CLK设定为2000时，电机转速约为75r/min

	return rs;
}

// 返回匀速运动的时间，以40ms为单位
int DataSeted::getConstTime(void)
{
	int constVT = constTime;
	return constVT;
}

float DataSeted::calSubLastTime(void)
{
	float lastTime;

	lastTime = (float)(((float)subLastTime / 0xffff) * SETED_DATA_PER_TIME);

	return lastTime;
}

// 检查数据是否满足规定格式
bool DataSeted::checkSetedData(void)
{
	// 脉冲和频率要么都为零，要么都不为零
	if ( ((0 == nAxisPlusNum) && (0 == nAxisClk)) )
	{
		if (0 != addSubClkSet[0])
			return false;
	}
	else if ( ((0 != nAxisPlusNum) && (0 != nAxisClk)) )
	{
		// 加减速各段的时钟设置是否正确
		if (0 == addSubClkSet[0])
			return false;
		for (size_t i = 1; i < 7; i++)		// 加减速最多7段
		{
			if (0 == addSubClkSet[i])
				break;
			if (addSubClkSet[i-1] > addSubClkSet[i])
				return false;
		}
	}
	else
		return false;

	return true;
}

// 获得最大的设定参数，以此参数为基准进行计算拐点等参数
DataSeted* getMaxSetedData(DataSeted* setedData1, 
	DataSeted* setedData2, DataSeted* setedData3)
{
	DataSeted* maxSetedData;
	maxSetedData = (setedData1->nAxisClk > setedData2->nAxisClk) ? setedData1 : setedData2;
	maxSetedData = (maxSetedData->nAxisClk > setedData3->nAxisClk) ? maxSetedData : setedData3;
	return maxSetedData;
}

// 将一个数据依次压入容器中
//void DataSeted::dataSeted2VecArray(vector<string> *pVecSetDataArray, unsigned int timeNum)
//{
//	// 计算有几个拐点
//	//unsigned char inflectionPoint = 0;
//	//for (size_t i = 0; i < 8; i++)
//	//{
//	//	if (0 == addSubClkSet[i])
//	//		break;
//	//	else
//	//		inflectionPoint++;
//	//}
//	//inflectionPoint *= 4;
//
//	//string strTemp;
//	//// 开始两个点
//	//strTemp = to_string(40e-3 * timeNum++);	
//	//strTemp += ",";
//	//strTemp += to_string(0);
//	//strTemp += ",";
//	//strTemp += to_string(0);
//	//strTemp += ",";
//	//strTemp += to_string(0);
//	//pVecSetDataArray->push_back(strTemp);
//
//	//strTemp = to_string(40e-3 * timeNum++);
//	//strTemp += ",";
//	//strTemp += to_string(addSubClkSet[0]);
//	//strTemp += ",";
//	//strTemp += to_string(0);
//	//strTemp += ",";
//	//strTemp += to_string(0);
//	//pVecSetDataArray->push_back(strTemp);
//
//
//	//for (size_t i = 0; i < inflectionPoint/2; i++)
//	//{
//	//	strTemp = to_string(40e-3 * timeNum++);		// to_string 精度为6位
//	//	strTemp += ",";
//	//	strTemp += to_string();					// 从零开始加速
//	//}
//
//
//}

/* ***************************************************************************?
函数名称:?str_to_hex?
函数功能:?字符串转换为十六进制?
输入参数:?string?字符串?result?十六进制?len?字符串的长度。?
输出参数:?无?
**************************************************************************** */
bool str_to_hex(const char *string, unsigned int* result, unsigned int len)
{
	// 检查数据是否正确
	if (4 != len)
	{
		cout << "十六进制数据长度有误" << endl;
		return false;
	}
	for (size_t i = 0; i < len; i++)
	{
		if (!((string[i] >= '0' && string[i] <= '9') ||
			(string[i] >= 'a' && string[i] <= 'f') ||
			(string[i] >= 'A' && string[i] <= 'F')))
		{
			cout << "十六进制数据有误" << endl;
			return false;
		}
	}

	char* format = "%4X";				// 转换几位的十六进制
	sscanf_s(string, format, result);

	return true;
}

// 切割字符串
void str2strTemp(const char *data, char* dataTemp, unsigned char cursor)
{
	for (size_t i = 0; i < 6; i++)
	{
		dataTemp[0] = data[3 + cursor * 6];
		dataTemp[1] = data[4 + cursor * 6];
		dataTemp[2] = data[0 + cursor * 6];
		dataTemp[3] = data[1 + cursor * 6];
	}
}

// 保存编码器某一轴某一相数据的结构体
typedef struct
{
	unsigned char start;		// 起始位：0或者1
	vector<int> timeIndex;		// 时间轴的索引
	// 结构体注意初始化
} NAxisPhase;

// 实测数据的类
class CoderData
{
public:
	CoderData();
	~CoderData();
	void setTimeAxis(vector<float> *pTimeAxis);
	friend void makeCoderData2Group(vector<float> *pTimeAxis, 
		NAxisPhase nAxisPhaseTotal[],	CoderData *pCoderDataGrouped);
	friend void display2(CoderData *pCoderData, unsigned int length);
private:
	vector<float> timeAxis;		// 时间轴的数据
	vector<NAxisPhase> nAxisValue[6];		// X、Y、Z轴A、B相
};

CoderData::CoderData()
{
	NAxisPhase temp;
	// 初始化
	for (size_t i = 0; i < 6; i++)	// 2相3轴
	{
		nAxisValue[i].push_back(temp);
	}
}

CoderData::~CoderData()
{
}

void CoderData::setTimeAxis(vector<float> *pTimeAxis)
{
	timeAxis = *pTimeAxis;
}

// 保存编码器string格式转化为的数字格式
typedef struct 
{
	float timeAxis;				// 时间轴
	unsigned char nAxisValue;	// 各轴的值
} CoderData2Digital;

