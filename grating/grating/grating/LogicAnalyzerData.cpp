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
//	cout << "����������" << endl;
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
	return true;
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
// ʹ�ñ��취���Ƚ����ݲ��Ϊ�ļ����ٽ������
void LogicAnalyzerData::filtering(unsigned int threshold)
{
	openFile();
	// ����ļ���
	string fileNameFilter = fileRename(fileName, "filter");
	ofstream outFile;
	outFile.open(fileNameFilter, ios::out);


	// �˲��С���
#if _DISPLAY_SCHEDULE
	cout << "�˲��У� 00%";
	const size_t steps = lineNum / 100;
	size_t curPos = 0;
	size_t stepsCnt = 0;
#else
	cout << "�˲���..." << endl;
#endif

	// ��һ�����ݴ���
	string strLine1st;
	vector<string> vecStr1st;
	do
	{
		// ���������������С�����迼���ͷ��ڴ�
		vecStr1st.clear();
		getline(inFile, strLine1st);
		cutStrByChar(strLine1st.c_str(), ',', &vecStr1st);		// ע���ַ�����һ���ո�
#if _DISPLAY_SCHEDULE
		// ��ʾ�����õ� �������и�BUG������������
		curPos++;
		stepsCnt++;
#endif
	} while (!(isNumber(vecStr1st[0].c_str())));
	// ��һ�����ݷ��������б���
	LADataPer dataPer1st;
	vecStr2Digital(&vecStr1st, &dataPer1st);
	// �����ݱ������̬���飬�Ա���бȽ�
	timeAndChanValue *dataPre = new timeAndChanValue[channel];
	for (size_t i = 0; i < channel; i++)
	{
		dataPre[i].time = dataPer1st.time;
		dataPre[i].chanValue = dataPer1st.chanValue[i];
	}

	// ��ȡ��һ�����ݣ��������ж���һ������д��ǰ�Ƿ����
	string strLine;
	while (getline(inFile, strLine))
	{
#if _DISPLAY_SCHEDULE
		// ��ʾ�����õ�
		curPos++;
		stepsCnt++;
		if (steps == stepsCnt)
		{
			displaySchedules(curPos, lineNum);
			stepsCnt = 0;
		}
#endif
		// �ָ��ַ���
		vector<string> vecStr;				// ע���ʼ���������
		cutStrByChar(strLine.c_str(), ',', &vecStr);
		// �ַ������ֻ�
		LADataPer dataPerLine;
		vecStr2Digital(&vecStr, &dataPerLine);

		// ��ͨ�����ݱȽ� 
		for (size_t i = 0; i < channel; i++)
		{
			if (dataPerLine.chanValue[i] != dataPre[i].chanValue)
			{
				// С����ֵ���˵�
				double diff = ((dataPerLine.time - dataPre[i].time)*1e6);	// ��λus
				if (diff < threshold)
				{
					if (0 == dataPre[i].chanValue)
					{
						strReplace(&strLine1st, i, ',', '1');
					}
					else if(1 == dataPre[i].chanValue)
					{
						strReplace(&strLine1st, i, ',', '0');
					}
					else
					{
						strLine1st = "x,x,x";
					}
				}
				else
				{
					// ���������� dataPre ��ŵ�ֵ
					dataPre[i].time = dataPerLine.time;
					dataPre[i].chanValue = dataPerLine.chanValue[i];
				}
			}
		}

		// �������д��outfile
		outFile << strLine1st << endl;
		// ���� ǰһ�� strLine1st
		strLine1st = strLine;
	}
	// д�����һ������(���һ�����ݲ����޸�)
	outFile << strLine1st << endl;

#if _DISPLAY_SCHEDULE
	// ��ʾ����
	cout << "\b\b\b" << "100%" << endl;
#endif

	// �ͷŶ�̬����
	delete[] dataPre;
	// �ر��ļ���
	outFile.close();
	closeFile();

	// �ض�λ���fileName��Ա
	fileName = fileNameFilter;
}


// ���ַ��������ڵ��ַ���ת��Ϊ�߼������ж�Ӧ�����ָ�ʽ����
void LogicAnalyzerData::vecStr2Digital(const vector<string>* vecStr, LADataPer* dataPer)
{
	// ���ݷ���ṹ��
	dataPer->time = strtod((*vecStr)[0].c_str(), nullptr);					// ʱ����
	for (size_t i = 1; i < (*vecStr).size(); i++)		// �ߵ͵�ƽֵ
	{
		if (0 == stoi((*vecStr)[i]))
		{
			dataPer->chanValue.push_back(low);
		}
		else if (1 == stoi((*vecStr)[i]))
		{
			dataPer->chanValue.push_back(high);
		}
		else
		{
			cout << "�ߵ͵�ƽֵ�����������飡" << endl;
		}
	}
}


// ����ͨ�����������Ϊ�����ļ�����ѹ��ʱ���ᣨ0,1,0,1...��
void LogicAnalyzerData::makeSubGroup(unsigned int chan)
{
	openFile();

	// �ļ�������
	string fileNameFilter = "filter_" + to_string(chan);
	fileNameFilter = fileRename(fileName, fileNameFilter);
	// ���ļ���
	ofstream outFile;
	outFile.open(fileNameFilter);

	string strLine;
	//  ��һ�����ݵ�����ֵ
	vector<string> vecStrPre;
	do
	{
		vecStrPre.clear();				// ������������������󣬲����ͷ��ڴ�
		getline(inFile, strLine);
		cutStrByChar(strLine.c_str(), ',', &vecStrPre);
	} while (!(isNumber(vecStrPre[0].c_str())));
	outFile << vecStrPre[0] << ", ";
	outFile << vecStrPre[chan + 1] << endl;

	// ��������ѭ������ 
	while (getline(inFile, strLine))
	{
		// �����ŷָ�
		vector<string> vecStr;
		cutStrByChar(strLine.c_str(), ',', &vecStr);
		if (!(isNumber(vecStr[0].c_str())))				// �������־���������ѭ��
			continue;
		// �ߵ͵�ƽ�仯ʱ�ٸ�ֵ
		if (vecStr[chan + 1] != vecStrPre[chan+1])
		{
			outFile << vecStr[0] << ", ";
			outFile << vecStr[chan + 1] << endl;
			// ���� vecStrPre �Ա���һ�αȽ�
			vecStrPre[0] = vecStr[0];
			vecStrPre[chan + 1] = vecStr[chan + 1];
		}			
	}

	outFile.close();
	closeFile();
}


// getline��ȡʱ��������������ص�һ����������ַ�����strLine��
string LogicAnalyzerData::skip2Digital(ifstream* pInFile)
{
	string strLine;
	vector<string> vecStr;

	do
	{
		vecStr.clear();		// ������������ݲ��󣬲����ͷ��ڴ�
		getline(inFile, strLine);
		cutStrByChar(strLine.c_str(), ',', &vecStr);		// ע���ַ�����һ���ո�
	} while (!(isNumber(vecStr[0].c_str())));

	return strLine;
}
