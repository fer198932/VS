// 主函数
#include "main.h"

int main()
{
	string fileName = 
		"H:\\work\\projects\\VS\\grating\\grating\\resource\\grating_data.csv";

#if _TEST_TIME
	// 测试时间
	time_t start, stop;
	start = time(nullptr);
#endif

	LogicAnalyzerData grating(fileName);

#if _FILTERING
	// 滤波
	grating.filtering(NOISE_US);
#endif

#if _TEST_TIME
	stop = time(nullptr);
	cout << "统计文件行数花费的时间：" << stop - start << endl;
#endif

	cout << grating.getLineNum() << endl;
	cout << grating.getChannelNum() << endl;
	return 1;
}