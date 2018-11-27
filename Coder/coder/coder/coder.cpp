// coder.cpp : �������̨Ӧ�ó������ڵ� YJY ��
//

#include "stdafx.h"
#include <fstream>
#include <iostream>
#include <cassert>
#include <string>
#include "coder.h"
#include <vector>
#include <sstream> //�ඨ������������ڶ�д�洢���ڴ��е�string����
#include <Windows.h>

using namespace std;

string readTxt(const string file);
void readCSV(const string file, vector<vector<string>> *pVecDataArray);
void writeCSV(const string file,const vector<vector<string>> *pVecDataArray);
void writeCSV(const string file, vector<vector<DataSeted>> *pVecDataArray, unsigned int dataNum);
void makeDataGroups(const string data, DataGroups * dataGroups, unsigned int dataNum);
void display1(vector<vector<DataSeted>> *pDataSeted, unsigned int dataNum);		// ��ʾ��

bool isNumber(const char *str);				// �ж��ַ����Ƿ�������
void ifPush2VecData(vector<vector<string>> *pVecDataArray, unsigned char push2VecFlag[], unsigned int timeAxisNum);
void setCoderDataNum(unsigned char* push2VecFlag);			// �趨iDataNum�Ƿ����ӵĺ���

// ��ñ����������ָ��λ�õ�ֵ
unsigned char getCoderDataOfNAxis(vector<vector<string>> *pVecDataArray, unsigned int iPos);
// ָ��λ�õ�����ѹ������
void push2VecData(vector<CoderData> *nAxisCoderData, unsigned char push2VecFlag[], unsigned int iDataNum);
// �������ַ���ת��Ϊ����
void makeCoderData2Digital(vector<vector<string>> *pVecDataArray, 
	vector<CoderData2Digital> *pCoderData2Digital);

// �ַ����������ƶ���
unsigned char string2Bin(const string str);

// �Ա�������������ݽ����˲��ģ�������threshold_us=25��25us��ë���˵���
void filterCoderData(vector<CoderData2Digital> *pVecDataArray, float threshold_us);

// �Ա��������ݽ��з���
void makeCoderData2Group(vector<CoderData2Digital> *pCoderData2Digital, vector<CoderData> *pCoderDataGrouped);

int main()
{
	const string testFile = "H:\\work\\projects\\STM32\\OIMRobot-400F407-3_20181119_Coder_Young\\Feedback_information.txt";
	const string file = "H:\\work\\projects\\VS\\Coder\\coder\\resouce\\DataSeted.txt";		// �趨����
	// ʵ������·��
	const string fileCSV_Test = "H:\\work\\projects\\VS\\Coder\\coder\\resouce\\Time_RS.csv";
	const string fileCSV[3] = { "H:\\work\\projects\\VS\\Coder\\coder\\resouce\\Time_RS_X.csv",
								"H:\\work\\projects\\VS\\Coder\\coder\\resouce\\Time_RS_Y.csv",
								"H:\\work\\projects\\VS\\Coder\\coder\\resouce\\Time_RS_Z.csv" };
		

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
	cout << "���ò������֣�" << endl;
	cout << "����" << dataNum << "�����ݣ�����" << endl;

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

//	display1(&dataSeted, dataNum);
	cout << endl;





	cout << "/////////////////////////////////////////////////////////////////" << endl << endl;;
	cout << "ʵ�ⲿ�֣�" << endl;

	for (size_t i = 0; i < 1/*3*/; i++)		// ѭ�����Σ��ֱ���X��Y��Z�������
	{
		// csv�ļ�������ʵ�ⲿ��
		vector<vector<string>> vecDataArray;	// ����CSV���ݵĽṹ
		readCSV(fileCSV_Test, &vecDataArray);

		// string ת��Ϊ digital
		vector<CoderData2Digital> coderData2Digital;
		makeCoderData2Digital(&vecDataArray, &coderData2Digital);

		// �Ա�������������ݽ�������˲�,С��20us�Ĳ�ȥ�����ȹر��˲�
//		filterCoderData(&coderData2Digital, PULES_WIDTH_THRESHOLD);	
	//	writeCSV(fileCSV, &vecDataArray);

		// �����ݽ��з���
		vector<CoderData> coderDataGrouped;		// ���ݷ���
		makeCoderData2Group(&coderData2Digital, &coderDataGrouped);




	}
	





	////  ɾ��vecDataArray�����ͷ����ֵ��ַ���
	//unsigned int iCursor = 0;
	//while ( !isNumber(vecDataArray[iCursor][0].c_str()) || '-' == vecDataArray[iCursor][0][0] )
	//	iCursor++;
	//vecDataArray.erase(vecDataArray.begin(), vecDataArray.begin() + iCursor);

	//vector<CoderData> nAxisCoderData[3];			// x��y��z���ʵ������
	//vector<float> vTimeAxis;						// ʱ��������������������е�ʱ���

	//// �Ƿ�ѹ�������ı�ǣ�0b0000��
	//// ǰ��λ��10-A��ѹ�������01-B��ѹ�������11-������������ѹ��
	//// ����λ��1-���ݿ�д�룬0-���ݲ���д��

	//unsigned char push2VecFlag[] = {0, 0, 0};
	//unsigned int iDataNum[] = {0, 0, 0};				// ������ʾ�ж��������ݣ�X��Y��Z��

	//// ����ѹ������
	//float fTimeDataTemp = (float)atof(vecDataArray[0][0].c_str());	// 0���������Ϊ�ο�
	//vTimeAxis.push_back(fTimeDataTemp);
	//for (size_t i = 1; i < 50/*vecDataArray.size()-1*/; i++)
	//{
	//	fTimeDataTemp = (float)atof(vecDataArray[i][0].c_str());
	//	vTimeAxis.push_back(fTimeDataTemp);

	//	// ��־��λ
	//	for (size_t i = 0; i < 3; i++)
	//		push2VecFlag[i] = 0;
	//	// ����ʱ���������Ƿ����ѹ�� nAxisCoderData
	//	ifPush2VecData(&vecDataArray, push2VecFlag, i);
	//	// ����iDataNum
	//	setCoderDataNum(push2VecFlag);
	//	// ����д��
	//	push2VecData(nAxisCoderData, push2VecFlag, iDataNum);

	//	


	//	cout << vTimeAxis[i] << endl;
	//}



	

	return 0;
}

// �Ա��������ݽ��з���
void makeCoderData2Group(vector<CoderData2Digital> *pCoderData2Digital, vector<CoderData> *pCoderDataGrouped)
{
	unsigned int dataOfDigital = pCoderData2Digital->size();
	unsigned int iDataNum = 0;		// �ֳɶ�����

	vector<CoderData2Digital>::iterator iterDig = pCoderData2Digital->begin();

	while ( iterDig <= pCoderData2Digital->end() )
	{


		iterDig++;
	}

}

// �ַ����������ƶ���
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
			cout << "��������������" << endl;
			temp = 0b10000000;
			break;
		}
	}


	return temp;
}

// �������ַ���ת��Ϊ����
void makeCoderData2Digital(vector<vector<string>> *pVecDataArray,
	vector<CoderData2Digital> *pCoderData2Digital)
{
	//  ɾ��vecDataArray�����ͷ����ֵ��ַ���
	unsigned int iCursor = 0;
	while ( !isNumber((*pVecDataArray)[iCursor][0].c_str()) || '-' == (*pVecDataArray)[iCursor][0][0] )
		iCursor++;
	(*pVecDataArray).erase((*pVecDataArray).begin(), (*pVecDataArray).begin() + iCursor);

	// ����ת��
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

// �ж��ַ����Ƿ�������
bool isNumber(const char *str)				
{
	float f;
	int n = sscanf_s(str, "%f", &f);
	return n != 0;
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

	 //������
	//for (size_t i = /*(*pVecDataArray).size()*/0; i<50; i++)
	//{
	//	for (size_t j = 0; j<(*pVecDataArray)[i].size(); j++)
	//		cout << (*pVecDataArray)[i][j] << "  ";
	//	cout << endl;
	//} 
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
	timeNum += ((*pVecDataArray)[0][iDataNum].getConstTime()-1);	// ����û�����⣬�������ConstTime��һ����

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
	DataSeted* pMaxSetedData;				// �������Ĳ�������Ϊ�����׼
	for (size_t i = 0; i < dataNum; i++)
	{
		// ��ÿ��� DataSeted ���ݽ���Ԥ��������д��CSV�ļ�
		for (size_t j = 0; j < 3; j++)							// ���������������Ƿ�淶
			if ( !((*pVecDataArray)[j][i].checkSetedData()) )
			{
				cout << "��" << i + 1 << "�������������飡" << endl;
				return;
			}
		pMaxSetedData = getMaxSetedData(&(*pVecDataArray)[0][i], 
			&(*pVecDataArray)[1][i], &(*pVecDataArray)[2][i]);

		// ����յ���
		unsigned char inflectionPoint = 0;
		for (size_t j = 0; j < 8; j++)
		{
			if (0 == (*pMaxSetedData).addSubClkSet[j])
				break;
			else
				inflectionPoint++;
		}
		if (0 == inflectionPoint || 32/4 < inflectionPoint)	// �յ���/4 ������(0,8]֮��
		{
			cout << "��" << i+1 << "��յ�������������" << endl;
			outFile.close();
			return;
		}

		inflectionPoint *= 4;

		// ����ת��
		float rsTemp[3];		// ���x��y��z�趨�ĵ�һ��ת��
		for (size_t j = 0; j < 3; j++)
			rsTemp[j] = (*pVecDataArray)[j][i].setedClk2RS(0);
		// ���һ���ĳ���ʱ��
		float lastTime;
		lastTime = (*pMaxSetedData).calSubLastTime();		

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
	const size_t clo = (*pVecDataArray)[0].size();		// ������������⣬�Ȳ��� byYJY

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
void filterCoderData(vector<CoderData2Digital> *pVecDataArray, float threshold_us)
{
	cout << "�˲�ǰ������:" << pVecDataArray->size() << endl;
	// ��ʼ�˲�
	cout << "�����˲�...";
	vector<CoderData2Digital>::iterator iter = pVecDataArray->begin();
	unsigned int cursor = 0;

	while ( iter < (pVecDataArray->end()-1) )
	{
		float timeAxisTemp = (float)(((*pVecDataArray)[cursor + 1].timeAxis -
			(*pVecDataArray)[cursor].timeAxis ) * 1e6);	// תΪus������Ƚ�--- С��Խ�� byYJY

		if ( threshold_us > timeAxisTemp )					// ��������С����ֵ
		{
			pVecDataArray->erase(iter, iter+1);		// vectorɾ����ʱ��ķ���Դ�ܶ࣬����˲�ʱ��ϴ�ʱ���ܾ�
			cout << ".";
		}
		else
		{
			cursor += 2;			// �۲����ݣ���2����
		}
		iter = pVecDataArray->begin() + cursor;
	}
	cout << endl;
	cout << "�˲������������У� " << pVecDataArray->size() << endl;
}

//void filterCoderData(vector<vector<string>> *pVecDataArray, float threshold_us)	
//{
//	// ɾ��������Ҫ���Ԫ�أ����ַ�		
//	vector<vector<string>>::iterator cursor = pVecDataArray->begin();
//	unsigned int iter = 0;
//	pVecDataArray->erase(pVecDataArray->begin() + iter); // ��һ�����ַ���ɾ��
//	cursor = pVecDataArray->begin();
//
//	// ��ʼ�˲�
//	cout << "�����˲����ȴ�ʱ������е㳤...";
//	while (cursor < (pVecDataArray->end()-2)) // iter != 20  != (pVecDataArray->end())
//	{
//		double str2FlTemp1, str2FlTemp2;
//		str2FlTemp1 = atof((*pVecDataArray)[iter][0].c_str()) * 1e6;	// Ҫ�Ƚ�stringתΪchar*,��תΪus
//		str2FlTemp2 = atof((*pVecDataArray)[iter+2][0].c_str()) * 1e6;
//		
//		if (str2FlTemp1 < 0 || (str2FlTemp2 - str2FlTemp1) < threshold_us)
//		{
//			pVecDataArray->erase(cursor, cursor+1);	// vectorɾ����ʱ��ķ���Դ�ܶ࣬����˲�ʱ��ϴ�ʱ���ܾ�
//			cout << ".";
//		}
//		else {
//			iter+=2;		// �۲����ݣ���2����
//		}
////		cout << str2FlTemp1 << endl;
//		cursor = pVecDataArray->begin() + iter;
//	}
//
//	//������
//	//for (size_t i = 0; i<(*pVecDataArray).size(); i++)
//	//{
//	//	for (size_t j = 0; j<(*pVecDataArray)[i].size(); j++)
//	//		cout << (*pVecDataArray)[i][j] << "----";
//	//	cout << endl;
//	//}
//}

// ��ñ����������ָ��λ�õ�ֵ
unsigned char getCoderDataOfNAxis(vector<vector<string>> *pVecDataArray, unsigned int iPos)
{
	unsigned char nAxisValue = 0b10000000;
	if (iPos < 0 || iPos >= pVecDataArray->size())
	{
		cout << "��ñ���������ʱ��λ�ó�����Χ�����飡" << endl;
		return nAxisValue;
	}

	string strTemp = "";
	for (size_t i = 1; i < (*pVecDataArray)[iPos].size(); i++)
			strTemp += (*pVecDataArray)[iPos][i][1];			// ��0���ǿո�
	//while (6 != strTemp.size())		// 3�ᣬÿ��A��B�࣬��6��
	//	strTemp += "0";
	
	nAxisValue = string2Bin(strTemp);

	return nAxisValue;
}

// �ж�ʵ�������Ƿ����ѹ�������ĺ�������Ҫ��
void ifPush2VecData(vector<vector<string>> *pVecDataArray,
	unsigned char push2VecFlag[], unsigned int timeAxisNum)
{
	unsigned char prevValues = getCoderDataOfNAxis(pVecDataArray, timeAxisNum - 1);	//  ���ǰһ������
	unsigned char curValues = getCoderDataOfNAxis(pVecDataArray, timeAxisNum);		// ��õ�ǰ����
	unsigned char nextValues = getCoderDataOfNAxis(pVecDataArray, timeAxisNum + 1);	// ��ú�һ������

	unsigned char judgeTemp = 0;

	judgeTemp = curValues ^ prevValues;
	switch (judgeTemp)
	{
	case 0b100000:		// Xa ���ʸ�

		break;
	case 0b010000:

		break;

		///////////��������������ж�  byYJY

	default:
		break;
	}

	if (curValues ^ prevValues)		// �ó��������λ��
	{

	}

}

// ָ��λ�õ�����ѹ������
void push2VecData(vector<CoderData> *nAxisCoderData, unsigned char push2VecFlag[], unsigned int iDataNum[])
{

	switch (1)
	{
	////////////����Ļ�Ҫд///////// byYJY

	default:
		break;
	}

	
}

void setCoderDataNum(unsigned char* push2VecFlag)			// �趨iDataNum�Ƿ����ӵĺ���
{

}



