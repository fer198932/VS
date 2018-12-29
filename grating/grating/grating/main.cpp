// ������
#include "main.h"

int main()
{
	string fileName = 
		"H:\\work\\projects\\VS\\grating\\grating\\resource\\grating_data.csv";

#if _TEST_TIME
	// ����ʱ��
	time_t start, stop;
	start = time(nullptr);
#endif

	LogicAnalyzerData grating(fileName);

#if _FILTERING
	// �˲�
	grating.filtering(NOISE_US);
#endif

#if _TEST_TIME
	stop = time(nullptr);
	cout << "ͳ���ļ��������ѵ�ʱ�䣺" << stop - start << endl;
#endif

	cout << grating.getLineNum() << endl;
	cout << grating.getChannelNum() << endl;
	return 1;
}