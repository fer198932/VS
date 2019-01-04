#pragma once

// #include "main.h"
#include <fstream>
#include <string>
#include <vector>
#include "myLib.h"

using namespace std;

// ͨ�����߼�ֵ��Ҫô1��Ҫô0 
enum logicValue
{
	low,
	high
};

// ʱ����+ͨ���ĸߵ͵�ƽ
typedef struct {
	double time;
	logicValue chanValue;
} timeAndChanValue;

// һ��string���������
typedef struct {
	double time;					//  ʱ���ᣬ��λs
	vector<logicValue> chanValue;		// ��ͨ����ֵ��ֵΪ0��1
} LADataPer;

class LogicAnalyzerData
{
public:
	LogicAnalyzerData(string fileName);
	~LogicAnalyzerData();
protected:
	// �ļ�����
	ifstream inFile;	
	void openFile();
	bool closeFile();
private:
	// �ļ��ܵ���������ʼ��ʱ���㣨����鿴���ȣ�
	unsigned int lineNum;
	// �����ļ��ܵ�����
	void countLineNum();
public:
	// ����ļ�������
	unsigned int getLineNum();
private:
	// ����ͨ����������ǰ���ʱ����
	unsigned int channel;
private:
	// ͳ���м���ͨ����������ʱ����
	void countChannel();
public:
	// ���ͨ����
	unsigned int getChannelNum();
private:
	// �ļ���
	string fileName;
public:
	// �����˲����˲���Ľ��ļ����Ᵽ�棬������filenameָ����ĺ���ļ�
	void filtering(unsigned int threshold);
	// ���ַ��������ڵ��ַ���ת��Ϊ�߼������ж�Ӧ�����ָ�ʽ����
	void vecStr2Digital(const vector<string>* vecStr, LADataPer* dataPer);
	// ����ͨ�����������Ϊ�����ļ�����ѹ��ʱ���ᣨ0,1,0,1...��
	void makeSubGroup(unsigned int chan);
	// getline��ȡʱ��������������ص�һ����������ַ�����strLine��
	string skip2Digital(ifstream* pInFile);
};

