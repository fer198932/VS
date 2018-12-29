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


// �ļ�����
void LogicAnalyzerData::openFile()
{
	inFile.open(fileName, ios::in);

	if (!inFile)
	{
		cout << "�ļ���ʧ�ܣ�" << endl;
		exit(0);
	}	
}


bool LogicAnalyzerData::closeFile()
{
	inFile.close();
	return false;
}


// �����ļ��ܵ�����
void LogicAnalyzerData::countLineNum()
{
	openFile();

	lineNum = 0;

	// // ���Ƚϣ���������ٶ����ܶ�
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
		cout << "�հ��ļ���" << endl;
		exit(0);
	}
}


// ����ļ�������
unsigned int LogicAnalyzerData::getLineNum()
{
	return lineNum;
}


// ͳ���м���ͨ����������ʱ����
void LogicAnalyzerData::countChannel()
{
	openFile();

	channel = 0;			// ���յ㣬��Ȼ���캯�����ѳ�ʼ��

	string strTemp;
	getline(inFile, strTemp);
	
	unsigned int length = strTemp.length();
	if (0 == length)
	{
		cout << "��ȡͨ��������" << endl;
		exit(0);
	}

	// ���ݶ����ж�ͨ����
	for (size_t i = 0; i < length; i++)
	{
		if (',' == strTemp[i])
			channel++;
	}
	closeFile();

	if (0 == channel)
	{
		cout << "ͨ����Ϊ0��" << endl;
		exit(0);
	}
}


// ���ͨ����
unsigned int LogicAnalyzerData::getChannelNum()
{
	return channel;
}


// �����˲����˲���Ľ��ļ����Ᵽ�棬������filenameָ����ĺ���ļ�
void LogicAnalyzerData::filtering(unsigned int threshold)
{
	openFile();

	/////////////////////////// byYJY

	for (size_t i = 0; i < channel; i++)
	{

	}
}
