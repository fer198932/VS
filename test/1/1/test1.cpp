#include <iostream>
#include <fstream> //�����д�������ļ�������
#include <vector>
#include <sstream> //�ඨ������������ڶ�д�洢���ڴ��е�string����
using namespace std;

void main()
{
	//д�ļ�
	ofstream outFile; //����ļ���(������ļ�)
	outFile.open("data1.csv", ios::out);//��ģʽ��ʡ��
	outFile << "name" << ',' << "age" << "," << "hobby" << endl;
	outFile << "xiaoming" << ',' << 18 << "," << "music" << endl;
	outFile << "Mike" << ',' << 21 << "," << "football" << endl;
	outFile << "Tom" << ',' << 23 << "," << "basketball" << endl;

	//���ļ�  
	ifstream inFile("data1.csv", ios::in);//inFile����fstream,ifstreamΪ�����ļ���(���ļ�����)
	string lineStr;
	vector<vector<string>> strArray;
	while (getline(inFile, lineStr)) //getline����sstream
	{
		//��ӡ�����ַ���
		//cout<<lineStr<<endl;
		//��ɶ�ά��ṹ
		stringstream ss(lineStr);//����sstream
		string str;
		vector<string> lineArray;
		//���ն��ŷָ�
		while (getline(ss, str, ','))
			lineArray.push_back(str);//һ��������vector����
									 //cout<<lineArray[0]<<endl;
		strArray.push_back(lineArray);//ÿһ��vector���ݶ��ŵ�strArray��ȥ
	}
	//������
	for (int i = 0; i<strArray.size(); i++)
	{
		for (int j = 0; j<strArray[i].size(); j++)
			cout << strArray[i][j] << "  ";
		cout << endl;
	}
	getchar();//�û�������ַ�������ڼ��̻������У�ֱ���û����س�Ϊֹ����ֹ����һ��������
}
