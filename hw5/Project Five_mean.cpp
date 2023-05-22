#define _CRT_SECURE_NO_WARNINGS
#define PI 3.1415926535
#include<iostream>
#include<string>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<windows.h>
using namespace std;

//宏定义，用于计算不小于位图数据所需字节数且为4的整数倍的数
#define WIDTHBYTES(i)    ((i+31)/32*4)

//全局变量
int BmpWidth;//bmp图像的宽度（单位像素）
int BmpHeight;//bmp图像的高度（单位像素）
int BmpByte;//bmp图像的位数
unsigned char **BmpBitDate;//bmp图像位图数据的起始指针


//函数声明
//bool readBmp(char *address);
//读入一张图片并将其储存在二维数组中
bool readBmp2(char *address);
//bool writemap(char *address, int BmpByte, int BmpWidth, int BmpHeight, RGBQUAD *RGBpalette, unsigned char *BmpBitDate);
//将二维数组的数据读出在一张图片中
bool writemap2(char *address, int BmpByte, int BmpWidth, int BmpHeight, RGBQUAD *RGBpalette, unsigned char **BmpBitDate);

//开始改良时用的一维数组转二维函数
//unsigned char** dimension(unsigned char *imgBuf);
//得到长宽为size的均值滤波器
unsigned char **getmeanfilter(int size);

//int **getleplacefilter(int size);
//用长宽为size的滤波器对图像进行滤波
unsigned char **filtering(unsigned char **imgBuf, unsigned char **filter, int size, int total);
//计算每个点滤波后的值
int point_filtering(unsigned char **imgBuf, int loci, int locj, unsigned char **filter, int size, int total);


//主函数
int main()
{	
	//定义经过变换后的位图数据指针
	unsigned char **bBmpBitDate;

	//均值滤波器头指针
	unsigned char **meanfilter;
	//均值滤波器大小
	int meanfilter_size;


	//原始位图数据指针
	char address[] = "lena.bmp";
	//读入address1路径的24位bmp图像
	readBmp2(address);

	//测试输出原始图像（改良为二维矩阵存储后）
	char address_test2[] = "test2.bmp";
	writemap2(address_test2, BmpByte, BmpWidth, BmpHeight, NULL, BmpBitDate);

	//用大小为3的均值滤波器对图像进行滤波并输出
	meanfilter_size = 3;
	meanfilter=getmeanfilter(meanfilter_size);
	bBmpBitDate=filtering(BmpBitDate, meanfilter, meanfilter_size, meanfilter_size*meanfilter_size);
	char address1[] = "size3_mean.bmp";
	writemap2(address1, BmpByte, BmpWidth, BmpHeight, NULL, bBmpBitDate);

	//用大小为3的均值滤波器对图像进行滤波并输出
	meanfilter_size = 5;
	meanfilter = getmeanfilter(meanfilter_size);
	bBmpBitDate = filtering(BmpBitDate, meanfilter, meanfilter_size, meanfilter_size*meanfilter_size);
	char address2[] = "size5_mean.bmp";
	writemap2(address2, BmpByte, BmpWidth, BmpHeight, NULL, bBmpBitDate);

	//用大小为3的均值滤波器对图像进行滤波并输出
	meanfilter_size = 7;
	meanfilter = getmeanfilter(meanfilter_size);
	bBmpBitDate = filtering(BmpBitDate, meanfilter, meanfilter_size, meanfilter_size*meanfilter_size);
	char address3[] = "size7_mean.bmp";
	writemap2(address3, BmpByte, BmpWidth, BmpHeight, NULL, bBmpBitDate);
	

	system("pause");
}

//函数：
//功能：
//将储存在一维数组中的位图数据转为储存在二维数组中
//参数：
//imgBuf：位图数据头指针
/*
unsigned char** dimension(unsigned char *imgBuf)
{
	unsigned char** dim;
	unsigned char*p = imgBuf;
	int i, j;
	//int LinePixel;
	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);

	//动态开辟空间存放处理后的数据
	dim = new unsigned char*[BmpHeight];
	for (i = 0; i < BmpHeight; i++)
	{
		dim[i] = new unsigned char[LineByte];
	}

	//处理数据
	for (i = 0; i < BmpHeight; i++)
	{
		for (j = 0; j < LineByte; j++)
		{
			dim[i][j] = p[i * LineByte + j];
		}
	}
	return dim;
}
*/

//函数：
//功能：
//得到一个均值滤波器
//参数：
//size：要得到的均值滤波器的大小（长宽）
unsigned char **getmeanfilter(int size)
{
	//得到滤波器首行首列相对于中心点的偏移度
	int mid = (size - 1) / 2;
	//定义滤波器数据头指针
	unsigned char **filter;

	//开辟储存空间
	filter = new unsigned char*[size];
	for (int i = 0; i < size; i++)
	{
		filter[i] = new unsigned char[size];
	}

	//对均值滤波器进行赋值
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			filter[i][j] = 1;
		}
	}
	return filter;
}

//函数：
//功能：
//对图像进行滤波
//参数：
//imgBuf：位图数据头指针
//filter：滤波器数据头指针
//size：滤波器大小（长宽）
//total：位图数据与滤波器数据对应相乘取和后所乘系数
unsigned char **filtering(unsigned char **imgBuf, unsigned char **filter, int size, int total)
{
	unsigned char** res;
	unsigned char**p = imgBuf;
	int i, j;
	//得到滤波器首行首列相对于中心点的偏移度
	int mid = (size - 1) / 2;

	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);
	int LinePixel = LineByte / 3;

	//动态开辟空间存放处理后的数据
	res = new unsigned char*[BmpHeight];
	for (i = 0; i < BmpHeight; i++)
	{
		res[i] = new unsigned char[LineByte];
	}

	//处理数据
	for (i = 0+mid; i < BmpHeight-mid; i++)
	{
		for (j = 0+mid; j < LinePixel-mid; j++)
		{
			for (int k = 0; k < 3; k++)
			{			
				res[i][j*3+k]=point_filtering(p, i, j*3+k, filter, size, total);
			}
		}
	}
	return res;
}

//函数：
//功能：
//对图像中的单个像素点进行计算
//参数：
//imgBuf：位图数据头指针
//filter：滤波器数据头指针
//size：滤波器大小（长宽）
//total：位图数据与滤波器数据对应相乘取和后所乘系数
//loci、locj：当前像素在位图数组中的坐标
int point_filtering(unsigned char **imgBuf, int loci, int locj, unsigned char **filter, int size, int total)
{
	//结果储存变量
	int sum = 0;
	int result;
	//得到滤波器首行首列相对于中心点的偏移度
	int mid = (size - 1) / 2;
	//位图数据头指针
	unsigned char **p = imgBuf;

	//处理数据
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			sum += p[loci - mid + i][locj - mid * 3 + j * 3] * filter[i][j];

		}
	}
	result= fabs(sum/total);
	return result;
}


//函数：
//功能：
//读入一张24位bmp图像（并储存在一个二维数组中）
//参数：
//address：要读入的bmp图像的地址
bool readBmp2(char *address)
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
	//读入文件的信息头
	fread(&infohead, sizeof(BITMAPINFOHEADER), 1, fp);

	//将信息头中的数据赋值给全局变量
	BmpByte = infohead.biBitCount;
	BmpHeight = infohead.biHeight;
	BmpWidth = infohead.biWidth;


	//计算存储位图每行数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);

	//动态开辟内存用来储存位图数据

	BmpBitDate = new unsigned char*[BmpHeight];
	for (int i = 0; i < BmpHeight; i++)
	{
		BmpBitDate[i] = new unsigned char[LineByte];
	}
	//BmpBitDate = new unsigned char[LineByte*BmpHeight];
	//读入位图数据
	for (int i = 0; i < BmpHeight; i++)
	{
		for (int j = 0; j < LineByte; j++)
		{
			fread(&BmpBitDate[i][j], 1, 1, fp);
		}
	}

	//关闭文件
	fclose(fp);
	return 1;
}

//函数：
//功能：
//写出一个bmp文件（根据一个二维数组中的数据）
//参数：
//address：文件地址
//BmpByte：bmp图像位数 
//BmpWidth：bmp图像宽度
//BmpHeight：bmp图像高度
//RGBpalette：调色盘起始指针，此处为空
//BmpBitDate：位图数据起始指针
bool writemap2(char *address, int BmpByte, int BmpWidth, int BmpHeight, RGBQUAD *RGBpalette, unsigned char **BmpBitDate)
{
	//按照参数中的地址以“二进制写”的方式打开一个bmp文件
	FILE *fp;
	int LineByte;
	if ((fp = fopen(address, "wb")) == NULL)return 0;

	int i, j;

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
	for (i = 0; i < BmpHeight; i++)
	{
		for (j = 0; j < LineByte; j++)
		{
			fwrite(&BmpBitDate[i][j], 1, 1, fp);
		}
	}
	
	//关闭文件
	fclose(fp);
	return 1;
}

