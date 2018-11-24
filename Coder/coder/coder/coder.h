#pragma once
#include <string>
#include <vector>

using namespace std;

#define MAX_DATA_NUM 1024
#define N_AXIS_SIZE 30				// һ����������ַ���
#define N_AXIS_SIZE_STRING 90		// �����ַ���������3��
#define SETED_DATA_PER_TIME	40e-3	// �趨ֵ��ʱ���ᰴ40msΪ��λ

bool str_to_hex(const char *string, unsigned int* result, unsigned int len);
void str2strTemp(const char *data, char* dataTemp, unsigned char cursor);

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
	float calSubLastTime(void);					// �������һ�����ĵ�ʱ��
	int getConstTime(void);						// ���������˶���ʱ�䣬��40msΪ��λ
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

// ���������˶���ʱ�䣬��40msΪ��λ
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

