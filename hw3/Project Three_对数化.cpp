#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<windows.h>
using namespace std;

//宏定义，用于计算不小于位图数据所需字节数且为4的整数倍的数

#define WIDTHBYTES(i)    ((i+31)/32*4)

int funr[256] = { 0 };
int fung[256] = { 0 };
int funb[256] = { 0 };


//全局变量

int BmpWidth;//bmp图像的宽度（单位像素）
int BmpHeight;//bmp图像的高度（单位像素）
int BmpByte;//bmp图像的位数
unsigned char *BmpBitDate;//bmp图像位图数据的起始指针


//函数声明
bool readBmp(char *address);
bool writemap(char *address, int BmpByte, int BmpWidth, int BmpHeight, RGBQUAD *RGBpalette, unsigned char *BmpBitDate);
int*  rgbtoyuv(unsigned char *imgBuf);
unsigned char*  yuvtogray(int *imgBuf, int *histogram);
unsigned char*  logarithm(unsigned char *imgBuf);//灰度图普通对数化
unsigned char*  logarithm1(unsigned char *imgBuf,int thres);//灰度图改良对数化
int*  logarithm2(int *imgBuf);//彩色图普通对数化
int*  logarithm3(int *imgBuf,int thres);// 彩色图改良对数化
unsigned char*  yuvtorgb(int *imgBuf);

int histogram2[256] = { 0 };
//主函数
int main()
{
	//定义经过不同变换后的位图数据指针
	int *bBmpBitDate;
	unsigned char *cBmpBitDate;
	unsigned char *dBmpBitDate;
	int *eBmpBitDate;
	unsigned char *fBmpBitDate;
	int *gBmpBitDate;
	unsigned char *hBmpBitDate;
	


	//灰度分布直方图
	int histogram[256] = { 0 };
	
	//二值化的阈值
	int threshold;
	int equaenergy = 16;

	//原始位图数据指针
	char address[] = "pic.bmp";
	//读入address1路径的24位bmp图像
	readBmp(address);

	//测试输出原始图像
	char address_test[] = "test.bmp";
	writemap(address_test, BmpByte, BmpWidth, BmpHeight, NULL, BmpBitDate);


	//将读入的rgb全彩色图像转化为yuv图像
	bBmpBitDate = rgbtoyuv(BmpBitDate);


	//将yuv图像转化为灰度图并输出
	cBmpBitDate = yuvtogray(bBmpBitDate,histogram);
	char address1[] = "gray.bmp";
	writemap(address1, BmpByte, BmpWidth, BmpHeight, NULL, cBmpBitDate);


	//灰度图的普通版对数化
	dBmpBitDate = logarithm(cBmpBitDate);
	char address2[] = "loga灰度普通.bmp";
	writemap(address2, BmpByte, BmpWidth, BmpHeight, NULL, dBmpBitDate);

	//灰度图的改良版对数化
	dBmpBitDate = logarithm1(cBmpBitDate,15);
	char address3[] = "loga灰度改良.bmp";
	writemap(address3, BmpByte, BmpWidth, BmpHeight, NULL, dBmpBitDate);

	//彩色图的普通对数化
	eBmpBitDate = logarithm2(bBmpBitDate);
	fBmpBitDate = yuvtorgb(eBmpBitDate);
	char address4[] = "loga彩色普通.bmp";
	writemap(address4, BmpByte, BmpWidth, BmpHeight, NULL, fBmpBitDate);


	//彩色图的改良对数化
	gBmpBitDate = logarithm3(bBmpBitDate,50);
	hBmpBitDate = yuvtorgb(gBmpBitDate);
	char address5[] = "loga彩色改良.bmp";
	writemap(address5, BmpByte, BmpWidth, BmpHeight, NULL, hBmpBitDate);

	
	system("pause");

}



//函数：
//功能：
//读入一张24位bmp图像
//参数：
//address：要读入的bmp图像的地址
bool readBmp(char *address)
{
	//按照函数参数的地址打开文件
	FILE *fp;
	if ((fp = fopen(address, "rb")) == NULL)
	{
		return 0;
	}
	//定义文件信息头
	//BITMAPFILEHEADER filehead;
	BITMAPINFOHEADER infohead;

	//移动指针到信息头起始位置
	fseek(fp, sizeof(BITMAPFILEHEADER), 0);
	//fread(&infohead, sizeof(BITMAPFILEHEADER), 1, fp);
	//读入文件的信息头
	fread(&infohead, sizeof(BITMAPINFOHEADER), 1, fp);

	//将信息头中的数据赋值给全局变量
	BmpByte = infohead.biBitCount;
	BmpHeight = infohead.biHeight;
	BmpWidth = infohead.biWidth;


	//计算存储位图每行数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);

	//动态开辟内存用来储存位图数据
	BmpBitDate = new unsigned char[LineByte*BmpHeight];
	//读入位图数据
	for (int i = 0; i < LineByte * BmpHeight; i++) {
		fread(&BmpBitDate[i], 1, 1, fp);
	}

	//关闭文件
	fclose(fp);
	return 1;

}

//函数：
//功能：
//写出一个bmp文件
//参数：
//address：文件地址
//BmpByte：bmp图像位数 
//BmpWidth：bmp图像宽度
//BmpHeight：bmp图像高度
//RGBpalette：调色盘起始指针，此处为空
//BmpBitDate：位图数据起始指针
bool writemap(char *address, int BmpByte, int BmpWidth, int BmpHeight, RGBQUAD *RGBpalette, unsigned char *BmpBitDate)
{
	//按照参数中的地址以“二进制写”的方式打开一个bmp文件
	FILE *fp;
	int LineByte;
	if ((fp = fopen(address, "wb")) == NULL)return 0;


	//定义文件头和信息头
	BITMAPFILEHEADER filehead;
	BITMAPINFOHEADER infohead;
	//计算位图数据每行需要的字节数
	LineByte = WIDTHBYTES(BmpByte * BmpWidth);


	//为文件头赋值
	filehead.bfType = 0x4D42;
	filehead.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + LineByte * BmpHeight;
	filehead.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	//写出文件头
	fwrite(&filehead, sizeof(BITMAPFILEHEADER), 1, fp);

	//为信息头赋值
	infohead.biSize = 40;
	infohead.biWidth = BmpWidth;
	infohead.biHeight = BmpHeight;
	infohead.biPlanes = 1;
	infohead.biBitCount = BmpByte;
	infohead.biCompression = BI_RGB;
	infohead.biSizeImage = LineByte * BmpHeight;
	infohead.biXPelsPerMeter = 0;
	infohead.biYPelsPerMeter = 0;
	infohead.biClrImportant = 0;
	infohead.biClrUsed = 0;

	//写出信息头
	fwrite(&infohead, sizeof(BITMAPINFOHEADER), 1, fp);

	//写出位图数据
	for (int i = 0; i < LineByte * BmpHeight; i++) {
		fwrite(&BmpBitDate[i], 1, 1, fp);
	}

	//关闭文件
	fclose(fp);
	return 1;
}

//函数：
//功能：
//yuv转rgb
//参数：
//imgBuf：需要处理的位图数据起始指针
unsigned char*  yuvtorgb(int *imgBuf)
{
	//定义处理后的指针
	unsigned char*rgb;
	int*p = imgBuf;
	int i;
	int k;
	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);

	//动态开辟空间存放处理后的数据
	rgb = new unsigned char[LineByte*BmpHeight];

	//处理数据
	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{
		k = ((double)*(p + i)) + 1.140*((double)*(p + i + 2));
		if (k > 255)k = 255;
		if (k < 0)k = 0;
		rgb[i] = k;

		k = ((double)*(p + i)) - 0.395*((double)*(p + i + 1)) - 0.581*((double)*(p + i + 2));
		if (k > 255)k = 255;
		if (k < 0)k = 0;
		rgb[i + 1] = k;

		k = ((double)*(p + i)) + 2.032*((double)*(p + i + 1));
		if (k > 255)k = 255;
		if (k < 0)k = 0;
		rgb[i + 2] = k;


	}
	//返回处理后的位图数据起始指针
	return rgb;
}


//函数：
//功能：
//rgb转yuv
//参数：
//imgBuf：需要处理的位图数据起始指针
int*  rgbtoyuv(unsigned char *imgBuf)
{
	//定义处理后的指针
	int* yuv;
	unsigned char*p = imgBuf;
	int i;
	int k;
	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);

	//动态开辟空间存放处理后的数据
	yuv = new int[LineByte*BmpHeight];

	//处理数据
	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{
		k = 0.299*((double)*(p + i)) + 0.587 * ((double)*(p + i + 1)) + 0.114*((double)*(p + i + 2));
		if (k > 255)k = 255;
		if (k < 0)k = 0;
		yuv[i] = k;

		k = -0.148 * ((double)*(p + i)) - 0.289*((double)*(p + i + 1)) + 0.437*((double)*(p + i + 2));
		if (k > 122)k = 122;
		if (k < -122)k = -122;
		yuv[i + 1] = k;

		k = 0.615 * ((double)*(p + i)) - 0.515*((double)*(p + i + 1)) - 0.100 * ((double)*(p + i + 2));
		if (k > 157)k = 157;
		if (k < -157)k = -157;
		yuv[i + 2] = k;

	}
	//返回处理后的位图数据起始指针
	return yuv;
}



//函数：
//功能：
//yuv转gray并统计亮度分布直方图
//参数：
//imgBuf：需要处理的位图数据起始指针
//histogram：储存灰度直方图的数组头指针
unsigned char*  yuvtogray(int *imgBuf,int * histogram)
{
	//定义处理后的指针
	unsigned char*gray;
	int*p = imgBuf;
	int i;
	int k;
	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);


	//动态开辟空间存放处理后的数据
	gray = new unsigned char[LineByte*BmpHeight];

	//处理数据
	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{

		k = *(p + i);
		if (k > 255)k = 255;
		if (k < 0)k = 0;
		gray[i] = k;
		//储存灰度直方图数据
		histogram[k]++;

		gray[i + 1] = gray[i];
		gray[i + 2] = gray[i];

		//此处不需要判断数据是否越界
		//因为传入的YUV图像在处理时已经判断
	}
	//返回处理后的位图数据起始指针
	return gray;
}

//函数：
//功能：
//灰度图对数化改良版
//参数：
//imgBuf：需要处理的位图数据起始指针
//thres：舍去数量过小的过低亮度像素点的阈值
unsigned char*  logarithm1(unsigned char *imgBuf,int thres)
{
	double* loga;
	unsigned char* result;
	unsigned char* p = imgBuf;

	//控制变量
	int i;
	//最大值
	double max=0;
	//最小值
	double min = 255;
	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);

	double kk;
	int maxlocii;
	int minlocii;
	int maxii=0;
	//int maxlocii;
	double cc;
	//int Maxvalue;
	int Minvalue;
	double cc2;
	double kk2;
	int histogram2[256] = { 0,0 };



	//动态开辟空间存放处理后的数据
	loga = new double[LineByte*BmpHeight];
	result = new unsigned char[LineByte*BmpHeight];

	//进行对数化
	for (i = 0; i < LineByte*BmpHeight; i=i+3)
	{
		loga[i] = log(1+(*(p + i)));	
		if (loga[i] >= max)
		{
			max = loga[i];
		}
		if ((loga[i] <= min))
		{
			min = loga[i];
		}

	}

	//得到映射比例
	cc = 255 / (max-min);
	kk = min * cc;

	//将对数结果映射到0-255
	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{	
		result[i] = ((cc*loga[i]) - kk);
		histogram2[result[i]]++;
		result[i + 1] = result[i];
		result[i + 2] = result[i];		
	}
	//得到当前的最小灰度值
	Minvalue = 0;
	for (i = 1; histogram2[i] <=thres; i++)
	{
        Minvalue = i;
	}
	
	//得到第二次映射比例
	cc2 = 255/(255 - double(Minvalue));
	kk2 = Minvalue * cc2;

	//将得到的最小灰度到最大灰度映射到0-255
		for (i = 0; i < LineByte*BmpHeight; i = i + 3)
		{
			if (result[i] <= Minvalue) {
				result[i] = 0;
			}
			else {
				result[i] = result[i] * cc2 - kk2;
				
			}
			result[i + 1] = result[i];
			result[i + 2] = result[i];
		}

	return result;
}

//函数：
//功能：
//灰度图对数化普通版
//参数：
//imgBuf：需要处理的位图数据起始指针
unsigned char*  logarithm(unsigned char *imgBuf)
{
	double* loga;
	unsigned char* result;
	unsigned char* p = imgBuf;

	//控制变量
	int i;
	//最大值
	double max = 0;
	//最小值
	double min = 255;
	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);

	double kk;
	//int maxlocii;
	//int minlocii;
	//int maxii = 0;
	double cc;



	//动态开辟空间存放处理后的数据
	loga = new double[LineByte*BmpHeight];
	result = new unsigned char[LineByte*BmpHeight];

	//进行对数化
	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{
		loga[i] = log(1 + (*(p + i)));

		if (loga[i] >= max)
		{
			max = loga[i];
		}
		if ((loga[i] <= min))
		{
			min = loga[i];
		}

	}

	//得到映射比例
	cc = 255 / (max-min);
	kk = min * cc;
	//将对数结果映射到0-255
	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{
		result[i] = ((cc*loga[i])-kk);
		result[i + 1] = result[i];
		result[i + 2] = result[i];
	}

	return result;
}

//函数：
//功能：
//彩色图对数化普通版
//参数：
//imgBuf：需要处理的位图数据起始指针
int *  logarithm2(int *imgBuf)
{
	double* loga;
	int* result;
	int* p = imgBuf;
	int i;
	double max = 0;
	double min = 255;
	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);
	
	double cc;
	double kk;



	//动态开辟空间存放处理后的数据
	loga = new double[LineByte*BmpHeight];
	result = new int[LineByte*BmpHeight];


	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{
		loga[i] = log(1 + (*(p + i)));

		if (loga[i] >= max)
		{
			max = loga[i];
		}
		if (loga[i] <= min)
		{
			min = loga[i];
		}

	}

	cc = 255 / max;
	kk = min * cc;


	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{
		result[i] = (cc*loga[i]);
		result[i + 1] = *(p + i + 1);
		result[i + 2] = *(p + i + 2);
	}

	return result;
}

//函数：
//功能：
//彩色图对数化改良版
//参数：
//imgBuf：需要处理的位图数据起始指针
//thres：舍去数量过小的过低亮度像素点的阈值
int *  logarithm3(int *imgBuf,int thres)
{
	double* loga;
	int* result;
	int* p = imgBuf;
	int i;

	double max = 0;
	double min = 255;
	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);

	double cc;
	double kk;
	int Minvalue;
	double cc2;
	double kk2;
	int histogram2[256] = { 0,0 };


	//动态开辟空间存放处理后的数据
	loga = new double[LineByte*BmpHeight];
	result = new int[LineByte*BmpHeight];


	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{
		loga[i] = log(1 + (*(p + i)));
		if (loga[i] >= max)
		{
			max = loga[i];
		}
		if (loga[i] <= min)
		{
			min = loga[i];
		}

	}

	//cc = 255 / max;
	//result[i] = cc * loga[i];

	//得到第一次映射的比例
	cc = 255 / (max - min);
	kk = min * cc;

    max = 255;
	min = 0;
	//进行第一次映射
	//统计映射后的灰度图
	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{

		result[i] = ((cc*loga[i]) - kk);
		histogram2[result[i]]++;
		if (result[i] >= max)
		{
			max = result[i];
		}
		if (result[i] <= min)
		{
			min = result[i];
		}
		result[i + 1] = *(p + i + 1);
		result[i + 2] = *(p + i + 2);
	}

	//得到对数化都舍去一些数量较少的灰度后的最小值
	Minvalue = 0;
	for (i = 1; histogram2[i] <= thres; i++)
	{
		Minvalue = i;
	}


	//得到第二次映射比例
	cc2 = 255 / double(255-Minvalue);
	kk2 = Minvalue * cc2;

	//进行第二次映射
	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{
		if (result[i] <= Minvalue) {
			result[i] = 0;
		}
		else {
			result[i] = result[i] * cc2 - kk2;
		}

	}

	return result;
}