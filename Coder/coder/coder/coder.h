#pragma once
#include <string>
#include <vector>

using namespace std;

// ��Ҫ�ĺ궨��
#define MAX_DATA_NUM 1024
#define N_AXIS_SIZE 30					// һ����������ַ���
#define N_AXIS_SIZE_STRING 90			// �����ַ���������3��
#define SETED_DATA_PER_TIME	40e-3		// �趨ֵ��ʱ���ᰴ40msΪ��λ
#define DUTY_RATIO_THRESHOLD 0.05		// ռ�ձ���ֵ��С�ڸ�ֵ�Ŀ��Ƕ���
#define PULES_WIDTH_THRESHOLD 25		// �����ȣ���λus��С�ڸÿ�ȵ���Ϊ����������֪100kHzʱ������������Ϊ10us
#define GROUPED_TIME 1					// ������жϲ�������λΪ��
#define ADD_SUB_TIME 400				// �Ӽ����Ӳ���ʱ�䣬Ĭ����40ms��������Ϊ40��10=400

bool str_to_hex(const char *string, unsigned int* result, unsigned int len);
void str2strTemp(const char *data, char* dataTemp, unsigned char cursor);

// �����ͷ�vectorռ�õ��ڴ��ģ�庯��
template<typename T>
void releaseVec(T *pVec)
{
	T v;
	pVec->clear();
	pVec->swap(v);
//	cout << "vector�ڴ��ͷųɹ�" << endl;
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
	float setedClk2RS(unsigned int num);	// ���趨��Ƶ��CLKת��Ϊת��
	float plusNum2Angle();					//  ���趨������ת��Ϊת���ĽǶ�
	float calSubLastTime(void);					// �������һ�����ĵ�ʱ��
	int getConstTime(void);						// ���������˶���ʱ�䣬��40msΪ��λ
	bool checkSetedData(void);					// ��������Ƿ�����涨��ʽ				
	friend DataSeted* getMaxSetedData			// ��������趨�������Դ˲���Ϊ��׼���м���յ�Ȳ���
		(DataSeted* setedData1, DataSeted* setedData2, DataSeted* setedData3);
	friend unsigned int getSetedAxisNum(vector<vector<DataSeted>> *pDataSeted, 
		unsigned int nAxis, vector<int> *pIndex);
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

// ���������ַ���ת��Ϊ���ֺ���� DataSeted ��
void DataSeted::sData2classData(const char* data)
{
	const unsigned int sizeProc = 4;	// ÿ�δ���4λ��16������
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

// ���趨��Ƶ��clkת��Ϊ���ת�ٵĺ���
float DataSeted::setedClk2RS(unsigned int num)
{
	float rs;		// ���ص�ת��

	// �ȼ�д
	rs = (float)addSubClkSet[num];			// ��֪CLK�趨Ϊ2000ʱ�����ת��ԼΪ75r/min

	return rs;
}

float DataSeted::plusNum2Angle()					//  ���趨������ת��Ϊת���ĽǶ�
{
	float angle;
	// �����߹��ľ��룬����Ϊ4mm
	angle = (float)(4.0 * nAxisPlusNum * 1.8 / (360.0 * 8.0));

	return angle;
}

// ���������˶���ʱ�䣬��40msΪ��λ
int DataSeted::getConstTime(void)
{
	int constVT = constTime;
	return constVT;
}

float DataSeted::calSubLastTime(void)
{
	float lastTime;

	lastTime = (float)(((float)subLastTime / ADD_SUB_TIME) * SETED_DATA_PER_TIME);

	return lastTime;
}

// ��������Ƿ�����涨��ʽ
bool DataSeted::checkSetedData(void)
{
	// �����Ƶ��Ҫô��Ϊ�㣬Ҫô����Ϊ��
	if ( ((0 == nAxisPlusNum) && (0 == nAxisClk)) )
	{
		if (0 != addSubClkSet[0])
			return false;
	}
	else if ( ((0 != nAxisPlusNum) && (0 != nAxisClk)) )
	{
		// �Ӽ��ٸ��ε�ʱ�������Ƿ���ȷ
		if (0 == addSubClkSet[0])
			return false;
		for (size_t i = 1; i < 7; i++)		// �Ӽ������7��
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

// ��������趨�������Դ˲���Ϊ��׼���м���յ�Ȳ���
DataSeted* getMaxSetedData(DataSeted* setedData1, 
	DataSeted* setedData2, DataSeted* setedData3)
{
	DataSeted* maxSetedData;
	maxSetedData = (setedData1->nAxisClk > setedData2->nAxisClk) ? setedData1 : setedData2;
	maxSetedData = (maxSetedData->nAxisClk > setedData3->nAxisClk) ? maxSetedData : setedData3;
	return maxSetedData;
}

// ��һ����������ѹ��������
//void DataSeted::dataSeted2VecArray(vector<string> *pVecSetDataArray, unsigned int timeNum)
//{
//	// �����м����յ�
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
//	//// ��ʼ������
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
//	//	strTemp = to_string(40e-3 * timeNum++);		// to_string ����Ϊ6λ
//	//	strTemp += ",";
//	//	strTemp += to_string();					// ���㿪ʼ����
//	//}
//
//
//}

/* ***************************************************************************?
��������:?str_to_hex?
��������:?�ַ���ת��Ϊʮ������?
�������:?string?�ַ���?result?ʮ������?len?�ַ����ĳ��ȡ�?
�������:?��?
**************************************************************************** */
bool str_to_hex(const char *string, unsigned int* result, unsigned int len)
{
	// ��������Ƿ���ȷ
	if (4 != len)
	{
		cout << "ʮ���������ݳ�������" << endl;
		return false;
	}
	for (size_t i = 0; i < len; i++)
	{
		if (!((string[i] >= '0' && string[i] <= '9') ||
			(string[i] >= 'a' && string[i] <= 'f') ||
			(string[i] >= 'A' && string[i] <= 'F')))
		{
			cout << "ʮ��������������" << endl;
			return false;
		}
	}

	char* format = "%4X";				// ת����λ��ʮ������
	sscanf_s(string, format, result);

	return true;
}

// �и��ַ���
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

// ���������ĳһ��ĳһ�����ݵĽṹ��
typedef struct
{
	unsigned char start;		// ��ʼλ��0����1
	vector<int> timeIndex;		// ʱ���������
	// �ṹ��ע���ʼ��
} NAxisPhase;

// ���������string��ʽת��Ϊ�����ָ�ʽ
typedef struct
{
	float timeAxis;				// ʱ����
	unsigned char nAxisValue;	// �����ֵ
} CoderData2Digital;

// ʵ�����ݵ���
class CoderData
{
public:
	CoderData();
	~CoderData();
	void setTimeAxis(vector<float> *pTimeAxis);
	friend void makeCoderData2Group(vector<float> *pTimeAxis, NAxisPhase nAxisPhaseTotal[], 
		vector<CoderData2Digital> *pCoderData2Digital, CoderData *pCoderDataGrouped);
	friend void display2(CoderData *pCoderData, unsigned int length);
	friend void writeCSV(const string file, CoderData *pCoderData,
		unsigned int channel, unsigned int frame);
	// ���ת��
	float getCoderRS(unsigned int channel, unsigned int frame, unsigned int time);

	friend void compareSetedCoder(vector<vector<DataSeted>> *pDataSeted,
		CoderData *pCoderData, unsigned int nAxis, unsigned int channel, string *str);
	friend void compareSetedCoder(vector<vector<DataSeted>> *pDataSeted, CoderData *pCoderData,
		unsigned int nAxis, unsigned int channel1, unsigned int channel2, string *str);		// 2��Ƶ
private:
	vector<float> timeAxis;		// ʱ���������
	vector<NAxisPhase> nAxisValue[6];		// X��Y��Z��A��B��
};

CoderData::CoderData()
{
	//NAxisPhase temp;
	//// ��ʼ��
	//for (size_t i = 0; i < 6; i++)	// 2��3��
	//{
	//	nAxisValue[i].push_back(temp);
	//}
}

CoderData::~CoderData()
{
}

void CoderData::setTimeAxis(vector<float> *pTimeAxis)
{
	timeAxis = *pTimeAxis;
}

// �����������ת��
// ���㹫ʽΪ rs = 60/(600*T) ��	T : ��������������
float CoderData::getCoderRS(unsigned int channel,
	unsigned int frame, unsigned int time)
{
	float rs = 0.0;
	float t, timeCur, timePre, timeNext;
	unsigned int length = (*this).nAxisValue[channel][frame].timeIndex.size();
	if (time < 0 || time > length)
	{
		cout << "ת�ټ����������飡" << endl;
		return rs;
	}

	unsigned int indexCur = (*this).nAxisValue[channel][frame].timeIndex[time];
	timeCur = this->timeAxis[indexCur];
	unsigned int indexPre, indexNext;
	
	if (0 == time)
	{
		indexNext = (*this).nAxisValue[channel][frame].timeIndex[time+1];
		timeNext = this->timeAxis[indexNext];
		t = timeNext - timeCur;
		rs = (float)(1.0 / ( 10.0 * 2.0 * t));
		return rs;
	}
	if ( (length-1) == time)
	{
		indexPre = (*this).nAxisValue[channel][frame].timeIndex[time - 1];
		timePre = this->timeAxis[indexPre];
		t = timeCur - timePre;
		rs = (float)(1.0 / (10.0 * 2.0 * t));
		return rs;
	}

	indexPre = (*this).nAxisValue[channel][frame].timeIndex[time - 1];
	indexNext = (*this).nAxisValue[channel][frame].timeIndex[time + 1];
	timeNext = this->timeAxis[indexNext];	
	timePre = this->timeAxis[indexPre];

	t = timeNext - timePre;
	rs = (float)(1.0 / (10.0 * t));

	return rs;
}

// ����趨���ݵ�ĳһ���˶���֡��
unsigned int getSetedAxisNum(vector<vector<DataSeted>> *pDataSeted, unsigned int nAxis, vector<int> *pIndex)
{
	unsigned int num = 0;
	unsigned int size = (*pDataSeted)[nAxis].size();
	unsigned int plusNum, clk;

	releaseVec(pIndex);		// ���һ��

	for (size_t i = 0; i < size; i++)
	{
		plusNum = (*pDataSeted)[nAxis][i].nAxisPlusNum;
		clk = (*pDataSeted)[nAxis][i].nAxisClk;
		if ((0 != plusNum) && (0 != clk))
		{
			num++;
			pIndex->push_back(i);
		}			
	}
	return num;
}

// �������ݶԱȵĺ������������ǵ��˶����룬��λmm
void compareSetedCoder(vector<vector<DataSeted>> *pDataSeted, 
	CoderData *pCoderData, unsigned int nAxis, unsigned int channel, string *str)
{
	vector<int> indexSeted;		// ������������ݵ��������
	unsigned int sizeSeted = getSetedAxisNum(pDataSeted, nAxis, &indexSeted);
	unsigned int sizeCoder = (*pCoderData).nAxisValue[channel].size();

	if ( !(((0 == nAxis) || (1 == nAxis) || (2 == nAxis)) && ((0 == channel) || 
		(1 == channel) || (2 == channel) || (3 == channel) || (4 == channel) || (5 == channel))) )
	{
		*str = "���������������ԱȺ�����";
		cout << *str << endl;
		return;
	}

	if (sizeSeted != sizeCoder)
	{
		*str = "�趨������ʵ���֡�����Ǻϣ��������ݣ�";
		cout << *str << endl;
		return;
	}

	*str = "֡��,�趨��ֵ,ʵ���ֵ,(��λ��mm)\n";
	for (size_t i = 0; i < sizeSeted; i++)
	{
		*str += "��" + to_string(i + 1) + "��,";

		unsigned int index = indexSeted[i];
		float temp = (*pDataSeted)[nAxis][index].plusNum2Angle();
		*str += to_string(temp) + ",";

		unsigned int coderPlusNum = (*pCoderData).nAxisValue[channel][i].timeIndex.size();	// ʵ��������
		temp = (float)(4.0 * coderPlusNum / 600.0 / 2.0);			// ����Ϊ4mm
		*str += to_string(temp) + "\n";
	}
	cout << "��" << sizeSeted << "����������˶Աȣ���鿴��" << endl << endl;
}

void compareSetedCoder(vector<vector<DataSeted>> *pDataSeted, CoderData *pCoderData, 
	unsigned int nAxis, unsigned int channel1, unsigned int channel2, string *str)	// 2��Ƶ
{
	vector<int> indexSeted;		// ������������ݵ��������
	unsigned int sizeSeted = getSetedAxisNum(pDataSeted, nAxis, &indexSeted);
	unsigned int sizeCoder1 = (*pCoderData).nAxisValue[channel1].size();		// ��һ��ͨ��
	unsigned int sizeCoder2 = (*pCoderData).nAxisValue[channel2].size();		// ��2��ͨ��

	if (!(((0 == nAxis) || (1 == nAxis) || (2 == nAxis)) && 
		((0 == channel1) || (1 == channel1) || (2 == channel1) || 
			(3 == channel1) || (4 == channel1) || (5 == channel1)) && 
		((0 == channel2) || (1 == channel2) || (2 == channel2) || (3 == channel2) || 
			(4 == channel2) || (5 == channel2))))
	{
		*str = "���������������ԱȺ�����";
		cout << *str << endl;
		return;
	}

	if ( (sizeSeted != sizeCoder1) || (sizeSeted != sizeCoder2) || (sizeCoder1 != sizeCoder2))
	{
		*str = "�趨������ʵ���֡�����Ǻϣ��������ݣ�";
		cout << *str << endl;
		return;
	}

	*str = "֡��,�趨��ֵ,ͨ��һ,ͨ����,2��Ƶ,(��λ��mm)\n";
	for (size_t i = 0; i < sizeSeted; i++)
	{
		*str += "��" + to_string(i + 1) + "��,";

		unsigned int index = indexSeted[i];
		float temp = (*pDataSeted)[nAxis][index].plusNum2Angle();
		*str += to_string(temp) + ",";

		unsigned int coderPlusNum1 = (*pCoderData).nAxisValue[channel1][i].timeIndex.size();	// ʵ��������
		temp = (float)(4.0 * coderPlusNum1 / 600.0 / 2.0);			// ����Ϊ4mm
		*str += to_string(temp) + ",";

		unsigned int coderPlusNum2 = (*pCoderData).nAxisValue[channel2][i].timeIndex.size();	// ʵ��������
		temp = (float)(4.0 * coderPlusNum2 / 600.0 / 2.0);			// ����Ϊ4mm
		*str += to_string(temp) + ",";

		temp = (float)( 4.0 * (coderPlusNum1 + coderPlusNum2) / 1200.0 / 2.0 );					// 2��Ƶ
		*str += to_string(temp) + "\n";
	}
	cout << "��" << sizeSeted << "����������˶Աȣ���鿴��" << endl << endl;
}

// �ַ���д��CSV�ļ�
void writeCSV(const string file, string* str )
{
	//  �ļ�������
	string fileName = file.substr(0, (file.size() - 4));
	fileName.insert(fileName.find_last_of("\\"), "\\output");
	fileName += "_compared.csv";
	ofstream outFile;
	outFile.open(fileName, ios::out);
	assert(outFile.is_open());		// ��ʧ��

	outFile << *str;
	outFile.close();
}


