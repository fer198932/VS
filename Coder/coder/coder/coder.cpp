// coder.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <fstream>
#include <iostream>
#include <cassert>
#include <string>
#include "coder.h"
#include <vector>
#include <sstream> //�ඨ������������ڶ�д�洢���ڴ��е�string����

using namespace std;

string readTxt(const string file);
void readCSV(const string file, vector<vector<string>> *pVecDataArray);
void writeCSV(const string file,const vector<vector<string>> *pVecDataArray);
void writeCSV(const string file, vector<vector<DataSeted>> *pVecDataArray, unsigned int dataNum);
void makeDataGroups(const string data, DataGroups * dataGroups, unsigned int dataNum);
void display1(vector<vector<DataSeted>> *pDataSeted, unsigned int dataNum);		// ��ʾ��

// �Ա�������������ݽ����˲��ģ�������threshold_us=25��25us��ë���˵���
void filterCoderData(vector<vector<string>> *pVecDataArray, float threshold_us);	

int main()
{
	const string testFile = "H:\\work\\projects\\STM32\\OIMRobot-400F407-3_20181119_Coder_Young\\Feedback_information.txt";
	const string file = "H:\\work\\projects\\VS\\Coder\\coder\\resouce\\DataSeted.txt";		// �趨����
	const string fileCSV = "H:\\work\\projects\\VS\\Coder\\coder\\resouce\\Time_RS.csv";		// ʵ������

	string sDataFile;
	sDataFile = readTxt(testFile);
//	int dataNum = sDataFile.size(); 
	unsigned int dataNum = sDataFile.size() % 270;	// ���ݱ���Ϊ270�ı���
	if (0 != dataNum)		// �ж������Ƿ�Խ��
	{
		cout << "���ݲ��ԣ�����" << endl;	
		return 0;
	}
	dataNum = ((sDataFile.size() - 1) / 3 + 1) / 90;	// �ж���������

	DataGroups *dataGroups = new DataGroups[dataNum];	// ע�����Ҫ�ͷ�
	makeDataGroups(sDataFile, dataGroups, dataNum);
	
	vector<vector<DataSeted>> dataSeted;					// �����ݷ������������ڶ�̬�����С
	dataSeted.resize(3, vector<DataSeted>(dataNum));		// row��x��y��z�᣻ col������������
	// ��ֵ
	DataSeted dataSetedTemp;							
	for (size_t i = 0; i < dataNum; i++)
	{
		dataSetedTemp.sData2classData(dataGroups[i].xAxisData);		// x������
		dataSeted[0][i] = dataSetedTemp;

		dataSetedTemp.sData2classData(dataGroups[i].yAxisData);		// y������
		dataSeted[1][i] = dataSetedTemp;

		dataSetedTemp.sData2classData(dataGroups[i].zAxisData);		// z������
		dataSeted[2][i] = dataSetedTemp;
	}
	delete[] dataGroups;							// û���ˣ��ͷŵ�

	writeCSV(file, &dataSeted, dataNum);			// ���趨�Ĳ���д��CSV�ļ�

	display1(&dataSeted, dataNum);
	cout << endl;






	// csv�ļ�������ʵ�ⲿ��
	vector<vector<string>> vecDataArray;	// ����CSV���ݵĽṹ
	readCSV(fileCSV, &vecDataArray);
//	filterCoderData(&vecDataArray, 1000);			// �Ա�������������ݽ�������˲�,25us�Ĳ�ȥ��
//	writeCSV(fileCSV, &vecDataArray);








//	cout << x << endl;
//	cout << dataNum << endl;

	return 0;
}

// ��ȡtxt�ļ��ĺ���
string readTxt(const string file)		
{	
	ifstream infile;
	infile.open(file);
	assert(infile.is_open());		// ��ʧ��

	string data;
	getline(infile, data);

	infile.close();

	return data;
}

// ��ȡcsv�ļ��ĺ���
void readCSV(const string file, vector<vector<string>> *pVecDataArray)
{
	ifstream infile;
	infile.open(file, ios::in);
	assert(infile.is_open());		// ��ʧ��

	string lineStr;
	while (getline(infile, lineStr))
	{
		// ��ɶ�ά��ṹ
		stringstream ss(lineStr);
		string str;
		vector<string> lineArray;
		// ���ն��Ÿ���
		while (getline(ss, str, ','))
			lineArray.push_back(str);
		pVecDataArray->push_back(lineArray);	// ÿһ��vector���ݶ��ŵ�vecDataArray��ȥ
	}
	infile.close();

	/* //������
	for (size_t i = 0; i<(*pVecDataArray).size(); i++)
	{
		for (size_t j = 0; j<(*pVecDataArray)[i].size(); j++)
			cout << (*pVecDataArray)[i][j] << "  ";
		cout << endl;
	} */
}

// ���趨��������Ϊ�ַ���������� rs:��һ��ת�٣�lastTime��������е�ʱ�䣬
string dataProc2outFile(unsigned int &timeNum, float rs[3], float lastTime)
{
	timeNum--;				// ע���������ʱ��Ҫ��һ�㣬ʵ�ʿ����ܲ���
	string strTemp;
	strTemp = to_string(lastTime + SETED_DATA_PER_TIME * timeNum);
	strTemp += ",";
	strTemp += to_string(rs[0]);		// x��
	strTemp += ",";
	strTemp += to_string(rs[1]);		// y��
	strTemp += ",";
	strTemp += to_string(rs[2]);		// z��
	strTemp += '\n';			// �س���

	strTemp += to_string(lastTime + SETED_DATA_PER_TIME * timeNum++);
	strTemp += ",";
	strTemp += to_string(0);		// x��
	strTemp += ",";
	strTemp += to_string(0);		// y��
	strTemp += ",";
	strTemp += to_string(0);		// z��
	strTemp += '\n';			// �س���

	return strTemp;
}
string dataProc2outFile(unsigned int &timeNum, float rs[3])
{
	string strTemp;
	strTemp = to_string(SETED_DATA_PER_TIME * timeNum);
	strTemp += ",";
	strTemp += to_string(0);		// x��
	strTemp += ",";
	strTemp += to_string(0);		// y��
	strTemp += ",";
	strTemp += to_string(0);		// z��
	strTemp += '\n';			// �س���

	strTemp += to_string(SETED_DATA_PER_TIME * timeNum++);
	strTemp += ",";
	strTemp += to_string(rs[0]);		// x��
	strTemp += ",";
	strTemp += to_string(rs[1]);		// y��
	strTemp += ",";
	strTemp += to_string(rs[2]);		// z��
	strTemp += '\n';			// �س���

	return strTemp;
}
string dataProc2outFile(unsigned int &timeNum, unsigned int cnt)
{
	string strTemp;
	// ʱ����֮��Ŀո񣬱��ڹ۲�
	for (size_t k = 0; k < cnt; k++)
	{
		strTemp = to_string(SETED_DATA_PER_TIME * timeNum++);			// ʱ�������
		strTemp += ",";
		strTemp += to_string(0);										// x���ת��
		strTemp += ",";
		strTemp += to_string(0);										// y���ת��
		strTemp += ",";
		strTemp += to_string(0);										// z���ת��
		strTemp += "\n";
	}
	return strTemp;
}
string dataProc2outFile(vector<vector<DataSeted>> *pVecDataArray, unsigned int iDataNum,
	unsigned char infPointNum, unsigned int &timeNum)
{
	string strTemp = "";
	unsigned char length = (infPointNum - 4) / 4;		//  �յ�ļ��㷽��
	float rsTemp;										// ת��
	// ������
	for (size_t i = 0; i < length; i++)					
	{
		//  ��һ����
		strTemp += to_string(SETED_DATA_PER_TIME * timeNum);
		strTemp += ",";
		rsTemp = (*pVecDataArray)[0][iDataNum].setedClk2RS(i);	// x��
		strTemp += to_string(rsTemp);
		strTemp += ",";
		rsTemp = (*pVecDataArray)[1][iDataNum].setedClk2RS(i);	// y��
		strTemp += to_string(rsTemp);
		strTemp += ",";
		rsTemp = (*pVecDataArray)[2][iDataNum].setedClk2RS(i);	// z��
		strTemp += to_string(rsTemp);
		strTemp += "\n";

		// �ڶ�����
		strTemp += to_string(SETED_DATA_PER_TIME * timeNum++);
		strTemp += ",";
		rsTemp = (*pVecDataArray)[0][iDataNum].setedClk2RS(i+1);	// x��
		strTemp += to_string(rsTemp);
		strTemp += ",";
		rsTemp = (*pVecDataArray)[1][iDataNum].setedClk2RS(i+1);	// y��
		strTemp += to_string(rsTemp);
		strTemp += ",";
		rsTemp = (*pVecDataArray)[2][iDataNum].setedClk2RS(i+1);	// z��
		strTemp += to_string(rsTemp);
		strTemp += "\n";
	}	

	// �м��
	timeNum += ((*pVecDataArray)[0][iDataNum].getConstTime()-1);

	// �½���
	for (size_t i = length; i > 0; i--)					// ������
	{
		//  ��һ����
		strTemp += to_string(SETED_DATA_PER_TIME * timeNum);
		strTemp += ",";
		rsTemp = (*pVecDataArray)[0][iDataNum].setedClk2RS(i);	// x��
		strTemp += to_string(rsTemp);
		strTemp += ",";
		rsTemp = (*pVecDataArray)[1][iDataNum].setedClk2RS(i);	// y��
		strTemp += to_string(rsTemp);
		strTemp += ",";
		rsTemp = (*pVecDataArray)[2][iDataNum].setedClk2RS(i);	// z��
		strTemp += to_string(rsTemp);
		strTemp += "\n";

		// �ڶ�����
		strTemp += to_string(SETED_DATA_PER_TIME * timeNum++);
		strTemp += ",";
		rsTemp = (*pVecDataArray)[0][iDataNum].setedClk2RS(i-1);	// x��
		strTemp += to_string(rsTemp);
		strTemp += ",";
		rsTemp = (*pVecDataArray)[1][iDataNum].setedClk2RS(i-1);	// y��
		strTemp += to_string(rsTemp);
		strTemp += ",";
		rsTemp = (*pVecDataArray)[2][iDataNum].setedClk2RS(i-1);	// z��
		strTemp += to_string(rsTemp);
		strTemp += "\n";
	}

	return strTemp;
}

// �趨����д��CSV�ļ�
void writeCSV(const string file, vector<vector<DataSeted>> *pVecDataArray, unsigned int dataNum)
{
	//  �ļ�������
	string fileName = file.substr(0, (file.size() - 4));
	fileName += "_proc.csv";
	ofstream outFile;
	outFile.open(fileName, ios::out);
	assert(outFile.is_open());		// ��ʧ��

	outFile << "Time" << "," << "X" << "," << "Y" << "," << "Z" << endl;
	// ���趨����������д������
	unsigned int timeNum = 0;			// ʱ���ᣬ��Ҫ
	vector<string> vecSetDataArray;
	for (size_t i = 0; i < dataNum; i++)
	{
		// ����յ���
		unsigned char inflectionPoint = 0;
		for (size_t j = 0; j < 8; j++)
		{
			if (0 == (*pVecDataArray)[0][i].addSubClkSet[j])
				break;
			else
				inflectionPoint++;
		}
		inflectionPoint *= 4;

		// ����ת��
		float rsTemp[3];		// ���x��y��z�趨�ĵ�һ��ת��
		for (size_t j = 0; j < 3; j++)
			rsTemp[j] = (*pVecDataArray)[j][i].setedClk2RS(0);
		// ���һ���ĳ���ʱ��
		float lastTime;
		lastTime = (*pVecDataArray)[0][i].calSubLastTime();

		// ÿ�����ݽ���������ļ���
		string strTemp;
		// ��ʼ������
		strTemp = dataProc2outFile(timeNum, rsTemp);
		outFile << strTemp;

		// �м�
		strTemp = dataProc2outFile(pVecDataArray, i, inflectionPoint, timeNum);
		outFile << strTemp;

		// ���������
		strTemp = dataProc2outFile(timeNum, rsTemp, lastTime);
		outFile << strTemp;

		// ��һ��ʱ����հ�
		strTemp = dataProc2outFile(timeNum, 30);
		outFile << strTemp;

		//// �ֶ��趨��ʼ��������(�ṹ���趨�ò�̫�á�����)
		//strTemp = to_string(SETED_DATA_PER_TIME * timeNum);			
		//strTemp += ",";
		//strTemp += to_string(0);										// x���ת��
		//strTemp += ",";
		//strTemp += to_string(0);										// y���ת��
		//strTemp += ",";
		//strTemp += to_string(0);										// z���ת��
		//outFile << strTemp << endl;

		//strTemp = to_string(SETED_DATA_PER_TIME * timeNum++);			// ʱ�������
		//strTemp += ",";
		//rsTemp = (*pVecDataArray)[0][i].setedClk2RS(0);					// x���ת��
		//strTemp += to_string(rsTemp);
		//strTemp += ",";
		//rsTemp = (*pVecDataArray)[1][i].setedClk2RS(0);					// y���ת��
		//strTemp += to_string(rsTemp);
		//strTemp += ",";
		//rsTemp = (*pVecDataArray)[2][i].setedClk2RS(0);					// z���ת��
		//strTemp += to_string(rsTemp);
		//outFile << strTemp << endl;

		//for (size_t j = 0; j < (inflectionPoint-4)/4; j++)				// ע��յ���㷨
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
		//// ���������
		//strTemp = to_string((*pVecDataArray)[0][i].calSubLastTime() + SETED_DATA_PER_TIME * timeNum);
		//strTemp += ",";
		//rsTemp = (*pVecDataArray)[0][i].setedClk2RS(0);					// x���ת��
		//strTemp += to_string(rsTemp);
		//strTemp += ",";
		//rsTemp = (*pVecDataArray)[1][i].setedClk2RS(0);					// y���ת��
		//strTemp += to_string(rsTemp);
		//strTemp += ",";
		//rsTemp = (*pVecDataArray)[2][i].setedClk2RS(0);					// z���ת��
		//strTemp += to_string(rsTemp);
		//outFile << strTemp << endl;

		//strTemp = to_string((*pVecDataArray)[0][i].calSubLastTime() + 
		//	SETED_DATA_PER_TIME * timeNum++);	// ʱ�������
		//strTemp += ",";
		//strTemp += to_string(0);										// x���ת��
		//strTemp += ",";
		//strTemp += to_string(0);										// y���ת��
		//strTemp += ",";
		//strTemp += to_string(0);										// z���ת��
		//outFile << strTemp << endl;

		//// ʱ����֮��Ŀո񣬱��ڹ۲�
		//for (size_t k = 0; k < 10; k++)
		//{
		//	strTemp = to_string(SETED_DATA_PER_TIME * timeNum++);			// ʱ�������
		//	strTemp += ",";
		//	strTemp += to_string(0);										// x���ת��
		//	strTemp += ",";
		//	strTemp += to_string(0);										// y���ת��
		//	strTemp += ",";
		//	strTemp += to_string(0);										// z���ת��
		//	outFile << strTemp << endl;
		//}		
	}



	outFile.close();
}

// ʵ������д��csv�ļ��ĺ���
void writeCSV(const string file, const vector<vector<string>> *pVecDataArray)
{
	//  �ļ�������
	string fileName = file.substr(0, (file.size()-4));
	fileName += "_filter.csv";
	ofstream outFile;
	outFile.open(fileName, ios::out);
	assert(outFile.is_open());		// ��ʧ��

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

// �Զ�ȡ�����ݽ��з���
void makeDataGroups(const string data, DataGroups * dataGroups, unsigned int dataNum)
{
	for (size_t i = 0; i < dataNum; i++)
	{
		for (size_t j = 0; j < N_AXIS_SIZE_STRING; j++)
		{
			dataGroups[i].xAxisData[j] = 
				data[0 + j + i * 3 * N_AXIS_SIZE_STRING];						// x��
			dataGroups[i].yAxisData[j] = 
				data[N_AXIS_SIZE_STRING + j + i * 3 * N_AXIS_SIZE_STRING];		// y��
			dataGroups[i].zAxisData[j] = 
				data[2 * N_AXIS_SIZE_STRING + j + i * 3 * N_AXIS_SIZE_STRING];	// z��
		}
	}
}

void display1(vector<vector<DataSeted>> *pDataSeted, unsigned int dataNum)
{
	for (size_t i = 0; i < dataNum; i++)
	{
		cout << "��%d�飺 " << i+1 << endl;
		cout << "x�᣺ " << endl;
		(*pDataSeted)[0][i].displayDataSeted();
		cout << "y�᣺ " << endl;
		(*pDataSeted)[1][i].displayDataSeted();
		cout << "z�᣺ " << endl;
		(*pDataSeted)[2][i].displayDataSeted();
		cout << endl;
	}
}

// �Ա�������������ݽ����˲��ģ�������threshold_us=25��25us��ë���˵���
void filterCoderData(vector<vector<string>> *pVecDataArray, float threshold_us)	
{
	// ɾ��������Ҫ���Ԫ�أ����ַ�		
	vector<vector<string>>::iterator cursor = pVecDataArray->begin();
	unsigned int iter = 0;
	pVecDataArray->erase(pVecDataArray->begin() + iter); // ��һ�����ַ���ɾ��
	cursor = pVecDataArray->begin();

	// ��ʼ�˲�
	cout << "�����˲����ȴ�ʱ������е㳤...";
	while (cursor < (pVecDataArray->end()-2)) // iter != 20  != (pVecDataArray->end())
	{
		double str2FlTemp1, str2FlTemp2;
		str2FlTemp1 = atof((*pVecDataArray)[iter][0].c_str()) * 1e6;	// Ҫ�Ƚ�stringתΪchar*,��תΪus
		str2FlTemp2 = atof((*pVecDataArray)[iter+2][0].c_str()) * 1e6;
		
		if (str2FlTemp1 < 0 || (str2FlTemp2 - str2FlTemp1) < threshold_us)
		{
			pVecDataArray->erase(cursor, cursor+1);	// vectorɾ����ʱ��ķ���Դ�ܶ࣬����˲�ʱ��ϴ�ʱ���ܾ�
			cout << ".";
		}
		else {
			iter+=2;		// �۲����ݣ���2����
		}
//		cout << str2FlTemp1 << endl;
		cursor = pVecDataArray->begin() + iter;
	}

	//������
	//for (size_t i = 0; i<(*pVecDataArray).size(); i++)
	//{
	//	for (size_t j = 0; j<(*pVecDataArray)[i].size(); j++)
	//		cout << (*pVecDataArray)[i][j] << "----";
	//	cout << endl;
	//}
}


