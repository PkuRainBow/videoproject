#pragma once
#include <iostream>
#include <string>
#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;

class UserVideoAbstraction{	
private:
	VideoAbstraction *userVB;
public:
	__declspec(dllexport) UserVideoAbstraction(const char* inputpath, const char* out_path, const char* log_path, const char* config_path,const char* index_path,const char* videoname, const char* midname, int scale);
	__declspec(dllexport) UserVideoAbstraction();
	__declspec(dllexport) ~UserVideoAbstraction();

	__declspec(dllexport) void UserAbstraction(Mat& currentFrame, int frameIndex);							//���б��������ĺ���
	__declspec(dllexport) int UsersaveConfigInfo();														//����ʣ���͹����Ϣ+�˶����е�������Ϣ
	__declspec(dllexport) void Usercompound(int compoundNum, const char* outputpath);							// ��ƵժҪ�ĺ���
	__declspec(dllexport) void Usercompound(int compoundNum, const char* outputpath, int frameCount);			// ��ƵժҪ�ĺ���
	__declspec(dllexport) void UserfreeObject();															// ��ʽ�ͷ��ڴ�ռ�
	__declspec(dllexport) void UsersetGpu(bool isgpu);													// �����Ƿ�ʹ��GPU
	__declspec(dllexport) void UsersetROI(Rect rectroi);											// �����Ƿ�ѡ�����Ȥ������
	__declspec(dllexport) void UsersetIndex(bool isindex);
	__declspec(dllexport) void UsersetMinArea(int area);
};