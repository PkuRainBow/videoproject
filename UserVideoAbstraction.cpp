#include "VideoAbstraction.h"
#include "UserVideoAbstraction.h"

UserVideoAbstraction::UserVideoAbstraction(const char* inputpath, const char* out_path, const char* log_path, const char* config_path,const char* index_path,const char* videoname, const char* midname, int scale)
{
	string t1=inputpath;
	string t2=out_path;
	string t3=log_path;
	string t4=config_path;
	string t5=index_path;
	string t6=videoname;
	string t7=midname;
	userVB =new VideoAbstraction(t1,t2,t3,t4,t5,t6,t7,scale);
}

UserVideoAbstraction::UserVideoAbstraction(){};

UserVideoAbstraction::~UserVideoAbstraction(){
	//userVB->~VideoAbstraction();
	delete userVB;
};

//ǰ�������뺯��
void UserVideoAbstraction::UserAbstraction(Mat& currentFrame, int frameIndex){
	userVB->Abstraction(currentFrame, frameIndex);
}

//����ǰ��������Ĳ�����͹����Ϣ��������Ϣ/������Ч֡��֡��
int UserVideoAbstraction::UsersaveConfigInfo(){
	//userVB->saveRemainObject();
	userVB->saveConfigInfo();
	return userVB->ObjectCubeNumber;
}

//��Ƶѹ������ �����ֽܷ׶δ���
void UserVideoAbstraction::Usercompound(int compoundNum, const char* outputpath){
	userVB->motionToCompound=compoundNum;
	string path = outputpath;
	userVB->LoadConfigInfo();
	userVB->compound(path);
}

//��Ƶѹ�������������ڷֽ׶δ���frameCount ����Ƶǰ��������׶α����ֵ
void UserVideoAbstraction::Usercompound(int compoundNum, const char* outputpath, int frameCount){
	userVB->motionToCompound=compoundNum;
	string path = outputpath;
	userVB->LoadConfigInfo(frameCount);
	userVB->compound(path);
}

//��ʽ��������
void UserVideoAbstraction::UserfreeObject(){
	userVB->freeObject();
}

void UserVideoAbstraction::UsersetGpu(bool isgpu){
	userVB->setGpu(isgpu);
}

void UserVideoAbstraction::UsersetROI(Rect rectroi){	
	//userVB->setROI(isroi);
	userVB->useROI=true;
	//userVB->frameHeight=rectroi.height;
	//userVB->frameWidth=rectroi.width;
	//userVB->rectROI.width=rectroi.width;
	//userVB->rectROI.height=rectroi.height;
	userVB->rectROI.x=rectroi.x;
	userVB->rectROI.y=rectroi.y;
}

void UserVideoAbstraction::UsersetIndex(bool isindex){
	userVB->setGpu(isindex);
}

void UserVideoAbstraction::UsersetMinArea(int minarea){
	userVB->objectarea=minarea;
	cout<<"area"<<userVB->objectarea;
}

void UserVideoAbstraction::UserGetKeyFrame(string filepath, int frameCount){
	userVB->LoadConfigInfo(frameCount);
	userVB->getKeyFrame(filepath);
}

