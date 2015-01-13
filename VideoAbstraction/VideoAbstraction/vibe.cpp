
#include "vibe.h"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int cx_off[9] = {-1,  0,  1, -1, 1, -1, 0, 1, 0};  //x的邻居点
int cy_off[9] = {-1,  0,  1, -1, 1, -1, 0, 1, 0};  //y的邻居点

ViBe::ViBe(void)
{

}
ViBe::~ViBe(void)
{

}

/**************** Assign space and init ***************************/
void ViBe::init(const Mat _image)
{
	for(int i = 0; i < NUM_SAMPLES; i++)
	{
		samples[i] = Mat::zeros(_image.size(), CV_8UC1);
	}
	mask = Mat::zeros(_image.size(),CV_8UC1);
	foregroundMatchCount = Mat::zeros(_image.size(),CV_8UC1);
	
	thresholdMap=Mat::zeros(_image.size(),CV_32FC1);
	//thresholdMap+=(Mat::ones(thresholdMap.rows,thresholdMap.cols,CV_32FC1)*2);
	frameCount=1;
}


void ViBe::init(const Mat first,const Mat second){
	Mat difference=second-first;

}

/**************** Init model from first frame ********************/
void ViBe::processFirstFrame(const Mat _image)
{
	RNG rng;
	int row, col;
	formerFrame=_image.clone();
	for(int i = 0; i < _image.rows; i++)
	{
		for(int j = 0; j < _image.cols; j++)
		{
			for(int k = 0 ; k < NUM_SAMPLES; k++)
			{
				// Random pick up NUM_SAMPLES pixel in neighbourhood to construct the model
				int random = rng.uniform(0, 9);

				row = i + cy_off[random];
				if (row < 0) 
					row = 0;
				if (row >= _image.rows)
					row = _image.rows - 1;

				col = j + cx_off[random];
				if (col < 0) 
					col = 0;
				if (col >= _image.cols)
					col = _image.cols - 1;

				samples[k].at<uchar>(i, j) = _image.at<uchar>(row, col);
			}
		}
	}
}

/**************** Test a new frame and update model ********************/
void ViBe::testAndUpdate(const Mat _image)
{
	RNG rng;
	
	for(int i = 0; i < _image.rows; i++)
	{
		for(int j = 0; j < _image.cols; j++)
		{
			int matches(0), count(0);
			float dist;
			float currentMinMatch=1.5+3/(1+exp(-1*(100*thresholdMap.at<float>(i,j)/frameCount-3)));
			//cout<<currentMinMatch<<endl;
			while(matches < currentMinMatch && count < NUM_SAMPLES)
			{
				dist = abs(samples[count].at<uchar>(i, j) - _image.at<uchar>(i, j));
				if (dist < RADIUS)
					matches++;
				count++;
			}

			if (matches >= currentMinMatch)
			{
				// It is a background pixel
				foregroundMatchCount.at<uchar>(i, j) = 0;

				// Set background pixel to 0
				mask.at<uchar>(i, j) = 0;

				// 如果一个像素是背景点，那么它有 1 / defaultSubsamplingFactor 的概率去更新自己的模型样本值
				int random = rng.uniform(0, SUBSAMPLE_FACTOR);
				if (random == 0) 
				{
					random = rng.uniform(0, NUM_SAMPLES);
					samples[random].at<uchar>(i, j) = _image.at<uchar>(i, j);
				}

				// 同时也有 1 / defaultSubsamplingFactor 的概率去更新它的邻居点的模型样本值
				random = rng.uniform(0, SUBSAMPLE_FACTOR);
				if (random == 0)
				{
					int row, col;
					random = rng.uniform(0, 9);
					row = i + cy_off[random];
					if (row < 0) 
						row = 0;
					if (row >= _image.rows)
						row = _image.rows - 1;

					random = rng.uniform(0, 9);
					col = j + cx_off[random];
					if (col < 0) 
						col = 0;
					if (col >= _image.cols)
						col = _image.cols - 1;

					random = rng.uniform(0, NUM_SAMPLES);
					samples[random].at<uchar>(row, col) = _image.at<uchar>(i, j);
				}
			}
			else
			{
				// It is a foreground pixel
				foregroundMatchCount.at<uchar>(i, j)++;
				thresholdMap.at<float>(i,j)++;
				// Set background pixel to 255
				mask.at<uchar>(i, j) = 255;

				//如果某个像素点连续N次被检测为前景，则认为一块静止区域被误判为运动，将其更新为背景点
				if (foregroundMatchCount.at<uchar>(i, j) > 50)
				{
					int random = rng.uniform(0, NUM_SAMPLES);
					if (random == 0)
					{
						random = rng.uniform(0, NUM_SAMPLES);
						samples[random].at<uchar>(i, j) = _image.at<uchar>(i, j);
					}
				}
			}
		}
	}
	//thresholdMap*=0.9;
	if(frameCount<200){
		currentFrame=_image.clone();
		Mat diff=currentFrame-formerFrame;
		threshold(diff,diff,1,255,CV_THRESH_BINARY);
		mask=mask&diff;
		formerFrame=currentFrame;
	}
	frameCount++;
}
//vibe::vibe(int _NumOfSample,int _RadiusOfSphere,int _NumOfCloseSamples,int _RadomSampling,int _dWidth,int _dHeight,InputArray _FirstFrame){
//	dHeight=_dHeight;
//	dWidth=_dWidth;
//	NumOfSamples=_NumOfSample;
//	RadiusOfSphere=_RadiusOfSphere;
//	NumOfCloseSamples=_NumOfCloseSamples;
//	RadomSampling=_RadomSampling;
//	image=_FirstFrame.getMat();
//	bgModel=new Mat[NumOfSamples];
//	for(int k=0;k<NumOfSamples;k++){
//		bgModel[k].create(dHeight,dWidth,CV_8U);
//	}
//	Neighbours=new byte[NumOfSamples];
//	segMap.create(dHeight,dWidth,CV_8U);
//	for(int i=0;i<dHeight;i++){
//		for(int j=0;j<dWidth;j++){
//			getNeighbours(image,i,j,Neighbours);
//			for(int k=0;k<NumOfSamples;k++){
//				*(bgModel[k].data+i*bgModel[k].step[0]+j*bgModel[k].step[1])=Neighbours[k];
//			}
//		}
//	}
//}
//void vibe::getNeighbours(Mat input,int corX,int corY,byte *result){
//	int r,x,y,i=0;
//	byte* data = input.data;
//	//result[i++]=*(data + corX * step + corY * elemSize);
//	int step=input.step[0],elemSize=input.step[1];
//	//for(r=1;r<=1;r++){
//	//	for(x=corX-r;x<corX+r;x++){
//	//		for(y=corY-r;y<corY+r;y++){
//	//			if(((x-corX)^2+(y-corY)^2>=r*r)&&(x>=0)&&(x<dHeight)&&(y>=0)&&(y<dWidth)){
//	//				/*result[i++]=input.at<uchar>(x,y);*/
//	//				result[i++]=*(data + x * step + y * elemSize);
//	//				if(i==NumOfSamples)return;
//	//			}
//	//		}
//	//	}
//	//}
//	int j=i;
//	if(i<NumOfSamples){
//		for(;i<NumOfSamples;i++)
//			result[i]=*(data + corX * step + corY * elemSize);//result[i%j];
//	}
//}
//
//void vibe::update(InputArray input){
//	int x,y;
//	byte* data;
//	Mat inputmat=input.getMat();
//	for(x=0;x<dHeight;x++){
//		for(y=0;y<dWidth;y++){
//			int count=0,index=0,dist=0;
//			data=inputmat.data;
//			int step=inputmat.step[0],elemSize=inputmat.step[1];
//			while((count<NumOfCloseSamples)&&(index<NumOfSamples)){
//				byte a=*(data + x * step + y * elemSize);
//				byte b=*(bgModel[index].data+x*bgModel[index].step[0]+y*bgModel[index].step[1]);
//				dist=a-b;
//				//if(dist<RadiusOfSphere)count++;
//				if(dist<RadiusOfSphere&&dist>-RadiusOfSphere)count++;
//				index++;
//			}
//			if(count>=NumOfCloseSamples){//为背景
//				*(segMap.data+x*segMap.step[0]+y*segMap.step[1])=0;
//				//segMap.at<uchar>(x,y)=0;
//				int rand;
//				rand= int(random(0,RadomSampling));
//				if(rand==0||true){
//					rand=int(random(0,NumOfSamples));
//					*(bgModel[rand].data+x*bgModel[rand].step[0]+y*bgModel[rand].step[1])=*(inputmat.data+x*inputmat.step[0]+y*inputmat.step[1]);
//					//bgModel[rand].at<uchar>(x,y)=inputmat.at<uchar>(x,y);
//				}
//				rand= int(random(0,RadomSampling));
//				if(rand==0){
//					int NeighbourX=getRandomNeighborX(x);
//					int NeighbourY=getRandomNeighborY(y);
//					rand=int(random(0,NumOfSamples));
//					*(bgModel[rand].data+NeighbourX*bgModel[rand].step[0]+NeighbourY*bgModel[rand].step[1])=*(inputmat.data+x*inputmat.step[0]+y*inputmat.step[1]);
//					//bgModel[rand].at<uchar>(NeighbourX,NeighbourY)=inputmat.at<uchar>(x,y);
//				}
//			}
//			else{
//				*(segMap.data+x*segMap.step[0]+y*segMap.step[1])=255;
//				//segMap.at<uchar>(x,y)=255;
//			}
//		}
//	}
//}
//
//int vibe::getRandomNeighborX(int x){
//	int r=x+int(random(-1,2));
//	while(r==x||r<0||r>=dHeight)r=x+int(random(-1,2));
//	return r;
//}
//int vibe::getRandomNeighborY(int y){
//	int r=y+int(random(-1,2));
//	while(r==y||r<0||r>=dWidth)r=y+int(random(-1,2));
//	return r;
//}
//double vibe::random(double start, double end)
//{
//    return start+(end-start)*rand()/(RAND_MAX + 1.0);
//}

//void vibe::ConnectedComponents(Mat &mask)
//{
//	byte* data=mask.data;
//	int step=mask.step,elemSize=mask.elemSize();
//	for(int i=1;i<mask.rows-1;i++){
//		for(int j=1;j<mask.cols-1;j++){
//			if(*(data+(i-1)*step+j*elemSize)&&*(data+i*step+(j-1)*elemSize)&&*(data+(i-1)*step+(j-1)*elemSize)){
//				*(data+i*step+j*elemSize)=255;
//			}
//		}
//	}
//}
