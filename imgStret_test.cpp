#include <stdio.h>
#include <hls_opencv.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>
typedef ap_axiu<8,2,5,6> uint8_sc;

#define IN_IMG "lena.bmp"
#define OUT_IMG "lena_out.bmp"
char imgOutArr[240][320];

void imgStret(hls::stream<uint8_sc> &inStream,
		hls::stream<uint8_sc> &outStream,
		unsigned char xMin,unsigned char xMax);

void saveImage(const std::string path, cv::Mat inArr)
{
	double min,max;
	cv::minMaxIdx(inArr,&min,&max);
	cv::Mat adjMap;
	cv::convertScaleAbs(inArr,adjMap,255/max);
	IplImage iplImage = IplImage(adjMap);
	cvSaveImage(path.c_str(),&iplImage);
}
int main()
{
	printf("LOAD IMAGE %s\n",IN_IMG);
	IplImage *src= cvLoadImage(IN_IMG);
	IplImage *img = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_8U, 1);
	cvCvtColor(src, img, CV_BGR2GRAY);
	cv::Mat* matImage = new cv::Mat(img);
	cv::Mat imgSrc = *matImage;
	hls::stream<uint8_sc> inStream;
	hls::stream<uint8_sc> outStream;
	cv::Mat imgOut(cv::Size(imgSrc.cols,imgSrc.rows),CV_8UC1,imgOutArr,cv::Mat::AUTO_STEP);
	printf("ROWS %d COLS %d\n",imgSrc.rows,imgSrc.cols);
	for(int i=0;i<imgSrc.rows;i++)
	{
		for(int j=0;j<imgSrc.cols;j++)
		{
			uint8_sc valIn;
			valIn.data = imgSrc.at<unsigned char>(i,j);
			valIn.keep=1; valIn.strb=1; valIn.user=1;
			valIn.user=0; valIn.dest=0;
			inStream<<valIn;
		}
	}
	imgStret(inStream,outStream,0,205);
	for(int i=0;i<imgSrc.rows;i++)
	{
		for(int j=0;j<imgSrc.cols;j++)
		{
			uint8_sc valOut;
			outStream.read(valOut);
			imgOutArr[i][j]=valOut.data;
		}
	}
	//saveImage(OUT_IMG,imgOutArr);
//	IplImage* imgOutIp;
//	imgOutIp = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_8U, 1);
//	memcpy(imgOutIp->imageData,imgOut.data,sizeof(unsigned char)*(src->width* src->height));

	//IplImage iplImage = IplImage(imgOut);
	//cvSaveImage(OUT_IMG,&iplImage, 0);
	saveImage(std::string(OUT_IMG),imgOut);
	return 0;
}
