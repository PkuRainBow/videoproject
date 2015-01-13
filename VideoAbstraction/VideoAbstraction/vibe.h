#pragma once
#include <iostream>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

#define NUM_SAMPLES 20		//ÿ�����ص����������
#define MIN_MATCHES 2		//#minָ��
#define RADIUS 20		//Sqthere�뾶
#define SUBSAMPLE_FACTOR 16	//�Ӳ�������


class ViBe
{
public:
	ViBe(void);
	~ViBe(void);

	void init(const Mat _image);   //��ʼ��
	void processFirstFrame(const Mat _image);
	void init(const Mat first,const Mat second);
	void testAndUpdate(const Mat _image);  //����
	Mat getMask(void){return mask;};
	Mat getMatchCount(){return foregroundMatchCount;};
	Mat mask;
//private:
	int frameCount;
	Mat samples[NUM_SAMPLES];
	Mat foregroundMatchCount;
	Mat thresholdMap;
	
	Mat formerFrame,currentFrame;
};

//#include <windows.h>
//#include <cv.h>
//#include <opencv2/core/core.hpp>
//
//using namespace cv;
//class vibe{
//public:
//	vibe(int NumOfSample,int RadiusOfSphere,int NumOfCloseSamples,int RadomSampling,int dWidth,int dHeight,InputArray FirstFrame);
//	void update(InputArray input);
//	void getNeighbours(Mat input,int corX,int coreY,byte *result);
//	int getRandomNeighborX(int x);
//	int getRandomNeighborY(int y);
//	double random(double start, double end);
//	int NumOfSamples;
//	int RadiusOfSphere;
//	int NumOfCloseSamples;//number of close samples for being part of the background (bg)
//	int RadomSampling;
//	int dWidth,dHeight;
//	byte *Neighbours;
//	Mat image;
//	Mat *bgModel;
//	Mat segMap;
//
//};

