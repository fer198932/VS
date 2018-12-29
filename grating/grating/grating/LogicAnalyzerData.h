#pragma once

#include "main.h"
#include <fstream>
#include <string>

using namespace std;

class LogicAnalyzerData
{
public:
	LogicAnalyzerData(string fileName);
	~LogicAnalyzerData();
private:
	ifstream inFile;

	// �ļ�����
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
};

