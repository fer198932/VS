#include <iostream>
#include <fstream> //定义读写已命名文件的类型
#include <vector>
#include <sstream> //多定义的类型则用于读写存储在内存中的string对象
using namespace std;

void main()
{
	//写文件
	ofstream outFile; //输出文件流(输出到文件)
	outFile.open("data1.csv", ios::out);//打开模式可省略
	outFile << "name" << ',' << "age" << "," << "hobby" << endl;
	outFile << "xiaoming" << ',' << 18 << "," << "music" << endl;
	outFile << "Mike" << ',' << 21 << "," << "football" << endl;
	outFile << "Tom" << ',' << 23 << "," << "basketball" << endl;

	//读文件  
	ifstream inFile("data1.csv", ios::in);//inFile来自fstream,ifstream为输入文件流(从文件读入)
	string lineStr;
	vector<vector<string>> strArray;
	while (getline(inFile, lineStr)) //getline来自sstream
	{
		//打印整行字符串
		//cout<<lineStr<<endl;
		//存成二维表结构
		stringstream ss(lineStr);//来自sstream
		string str;
		vector<string> lineArray;
		//按照逗号分隔
		while (getline(ss, str, ','))
			lineArray.push_back(str);//一行数据以vector保存
									 //cout<<lineArray[0]<<endl;
		strArray.push_back(lineArray);//每一行vector数据都放到strArray中去
	}
	//输出结果
	for (int i = 0; i<strArray.size(); i++)
	{
		for (int j = 0; j<strArray[i].size(); j++)
			cout << strArray[i][j] << "  ";
		cout << endl;
	}
	getchar();//用户输入的字符被存放在键盘缓冲区中，直到用户按回车为止（防止程序一闪而过）
}
