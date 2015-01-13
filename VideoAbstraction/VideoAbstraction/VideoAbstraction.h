#include <opencv2\opencv.hpp>
#include <iostream>
#include <string>
#include <windows.h>
#include <direct.h>
#include <time.h>
#include <queue>
#include <vector>
#include "QPBO.h"
#undef ERROR
#include "glog/logging.h"
#undef min
#undef max
#include "opencv2/gpu/gpu.hpp"
#include <boost/thread/thread.hpp>
#include "boost/filesystem.hpp"
#include <boost/lexical_cast.hpp>
/*
 *Boost Filesystem���ṩ�˶�·�����ļ���Ŀ¼���в�ѯ�Ͳ����ṩ�˿�һֱ�Ĺ��� ���е����ݶ��������ռ� boost::filesystem ��
 * boost::lexical_cast<string>(i)  �ѣ����е���������ת���< >�е���������
 */
namespace fs = boost::filesystem;

#define CV_CVX_WHITE    CV_RGB(0xff,0xff,0xff)
#define CV_CVX_BLACK    CV_RGB(0x00,0x00,0x00)
#define MAX_SYNOPSIS_LENGTH 400
using namespace cv;
using namespace std;

vector<bool> matToVector(Mat &input);

//�˶�����ṹ��
struct ObjectCube{
	int start;						//��ʼ��Ӧ��֡���
	int end;							//������Ӧ��֡���
	vector<vector<bool>> objectMask;	//�˶����ж�Ӧ��boolֵ����
};

class VideoAbstraction{
public:
	VideoAbstraction(string inputpath, string out_path, string log_path, string config_path, string index_path, string videoname, string midname, int size);
	VideoAbstraction();
	void init();
	//~VideoAbstraction();
	int scaleSize;
	int framePerSecond;				//֡��/��
	int maxLength;					//��Ƶ�������
	int curMaxLength;					//��ǰ�������Ƶ�������
	int maxLengthToSpilt;				//��Ƶ����������ֵ������������Ƚ��ᱻ�ָ�
	int cacheCollision[33][33][100];	//��ͻ����Ļ���
	int cacheShift;		
	int frameWidth,frameHeight; 
	int sumLength;					//ѹ������Ƶ���ܳ���
	int objectarea;					//͹����С�����
	
	VideoCapture videoCapture;			//��Ƶ��д
	VideoWriter videoWriter;
	VideoWriter indexWriter;           //�洢��Ӧÿһ֡�ϵ����ض�Ӧ���¼������
	
	int backgroundSubtractionMethod;	//ǰ��������  1-��˹���ģ�� 2-ViBe
	int LEARNING_RATE;		
	//vector<ObjectCube> objectVector;	//�洢����˶����У����洢��Ŀ�ﵽmotionToCompound ��д�뱾���ļ�
	vector<ObjectCube> partToCompound;	//�洢һ���˶�������˶����� 
	Mat backgroundImage;				//�洢 ��ϸ�˹ģ����ȡ���� ������Ϣ------�ָ�������еĵڶ�����Ҫ�����Ĳ���
	Mat currentStartIndex,currentEndIndex;

	//path string inputpath, string out_path, string log_path, string config_path, string index_path, string videoname, string midname
	string Inputpath;					//�������Ƶ��·�������֡��м��ļ�������
	string Outpath;					//��������Ƶ���·��
	string Logpath;
	string Configpath;
	string Indexpath;
	//name
	string InputName;					//�������Ƶ�ļ�������
	string MidName;					//�����ļ�/͹�� �ļ���
	string OutputName;

	int ObjectCubeNumber;				//�����˶����е�֡��������
	vector<int> frame_start;			//��¼�����˶����еĿ�ʼ֡���/����֡���
	vector<int> frame_end;
	//vector<string> allObjectCube;		//�洢�����˶����е�͹����Ϣ 
	int loadIndex;					//load͹���˶���Ϣ��ƫ�Ʋ���

	string videoFormat;
	vector<Mat> compoundResult;
	vector<Mat> indexs,indexe;

	ObjectCube currentObject;			//�ⲿ�����ĵ�ǰ�˶�֡����Ϣ
	int detectedMotion;				//��⵽���˶����еĸ�����ÿ���� motionToCompound���˶����У�д��һ���ļ�
	int motionToCompound;				//д���ļ����ٵ��˶�������Ŀ+�ϳɵ��˶�����������Ŀ
	//Abstraction Model Part
	int sum,thres;					//ͳ�����˶����е�֡������  &  �˶���ȡ����ֵ
	int currentLength,tempLength;		//

	//***    GPU    ***//
	gpu::MOG2_GPU gpumog;				//���û�ϸ�˹ģ�͵���
	gpu::GpuMat gpuFrame;				//�洢��Ƶ֡
	gpu::GpuMat gpuForegroundMask;		//�洢��Ƶ��ǰ����Ϣ 
	gpu::GpuMat gpuBackgroundImg;		//�洢��Ƶ�ı�����Ϣ

	bool useGpu;
	bool useROI;
	bool useIndex;
	Rect rectROI;

	BackgroundSubtractorMOG2 mog;
	Mat gFrame;						//�洢��Ƶ֡
	Mat gForegroundMask;				//�洢��Ƶ��ǰ����Ϣ 
	Mat gBackgroundImg;				//�洢��Ƶ�ı�����Ϣ
	int noObjectCount;					//�洢���˶����е�����֡��֡����
	bool flag;						//flag Ϊtrue��ʾ��ǰ֡�������˶����壬��Ϊfalse, ��ǰ֡��û���˶����壬�Ǳ���֡
	Mat currentMask;	

	string int2string(int _Val);
	vector<vector<Point>> stringToContors(string ss);										//���ַ���-->͹����Ϣ
	string contorsToString(vector<vector<Point>> &contors);
	void setVideoFormat(string Format);
	void postProc(Mat& frame);															//�Եõ���֡ͼ����к���(ȥ�� ��ͨ������ ͹������ ���͸�ʴ)							
	void ConnectedComponents(int frameindex, Mat &mask,int thres);										//͹������---��ͨ��������㷨��ʵ��
	void stitch(Mat &input1,Mat &input2,Mat &output,Mat &back,Mat &mask,int start,int end, int frameno);	//ƴ��ͼƬ֡���в����
//	void stitch(Mat &input1,Mat &input2,Mat &output,Mat &back,Mat &mask,int start,int end,vector<vector<Point>>& re_contours, bool& flag);	//ƴ��ͼƬ֡���в����
//	void stitch(Mat &input1,Mat &input2,Mat &output,Mat &back,vector<bool> &mask,int start,int end);
	Mat join(Mat &input1,Mat &input2,int type);											// Returns: cv::Mat �ϲ�����ͼ��Parameter: int type 1��ʾ�Ҷ�ͼ��3��ʾ��ɫͼ  
																					//��ͼƬ2ˮƽ��ӵ�ͼƬ1���Ҳಿ�ֺϳ�ͬһ��ͼƬ
	Mat join(Mat &input1,Mat &input2,Mat &input3,int type);								//�ϲ�3��ͼ��
	double random(double start, double end);												//����start��end֮��������
	int computeMaskCollision(Mat &input1,Mat &input2);									// ��������ͼ���г�ͻ�����ظ���
	int computeMaskCollision(vector<bool> &input1,vector<bool> &input2);						//overloaded function

	int computeObjectCollision(ObjectCube &ob1,ObjectCube &ob2,int shift,string path="");		// Returns:  int �����ڵ�ǰʱ��ƫ���������¼��ĳ�ͻ���ص�ĸ���  
																					//obj1���¼�1 obj2:�¼�2 shift �¼�2����¼�1��ʱ��ƫ��  string path �¼��洢Ŀ¼
	int graphCut(vector<int> &shift,vector<ObjectCube> &ob,int step=5);						//�����˶����У�ʹ�� graph_cut �㷨������ʱ��ƫ��
	int ComponentLable(Mat& fg_mask, vector<Rect>& vComponents_out, int area_threshold);
	bool isSimilarRects(const Rect& r1, const Rect& r2, double eps);
	double rectsOverlapAreaRate(const Rect& r1, const Rect& r2);
	static void on_mouse(int event,int x,int y,int flags,void* param);
	
	void saveObjectCube(ObjectCube &ob);													//����˶����е�����
	int saveRemainObject();															//�����������˶�������Ϣ�����ذ������˶����е�ͼ��֡�ĸ��� ��ֵ�� ObjectCubeNumber
	void loadObjectCube(int index_start, int index_len);									//��ȡ���ش�ŵ��˶�����
	void saveConfigInfo();																//���������Ϣ
	void LoadConfigInfo();																//��ȡ���ش�ŵ�������Ϣ
	void LoadConfigInfo(int frameCountUsed);												//��ȡ���ش�ŵ�������Ϣ

	void Abstraction(Mat& currentFrame, int frameIndex);									//���б��������ĺ��� ����ǰ�����֡�� ��ǰ֡�ı�ţ�
	void compound(string savepath);														//��Ƶ�ϳɺ���

	void freeObject();																	//��ʽ���������ܺ���
	void setGpu(bool isgpu);
	void setROI(bool isroi);
	void setIndex(bool isindex);
	void writeMask(Mat& input, Mat& output, int index);
};