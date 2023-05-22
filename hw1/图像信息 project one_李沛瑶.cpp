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




//全局变量

int BmpWidth;//bmp图像的宽度（单位像素）
int BmpHeight;//bmp图像的高度（单位像素）
int BmpByte;//bmp图像的位数
unsigned char *BmpBitDate;//bmp图像位图数据的起始指针


//函数声明
bool readBmp(char *address);
bool writemap(char *address, int BmpByte, int BmpWidth, int BmpHeight, RGBQUAD *RGBpalette, unsigned char *BmpBitDate);
int*  rgbtoyuv(unsigned char *imgBuf);
unsigned char*  yuvtorgb(int *imgBuf);
unsigned char*  yuvtogray(int *imgBuf);
int*  changey(int *imgBuf, int Ychange);


//主函数
int main()
{
	//定义经过不同变换后的位图数据指针
	int *bBmpBitDate;
	unsigned char *cBmpBitDate;
	unsigned char *dBmpBitDate;
	int *eBmpBitDate;
	unsigned char *fBmpBitDate;

	//YUV图像中Y分量改变的值
	int Ychange = -50;


	//原始位图数据指针
	char address[] = "sy.bmp";
	//读入address1路径的24位bmp图像
	readBmp(address);

	//测试输出原始图像
	char address_test[] = "sytest.bmp";
	writemap(address_test, BmpByte, BmpWidth, BmpHeight, NULL, BmpBitDate);



	//将读入的rgb全彩色图像转化为yuv图像并输出
	bBmpBitDate = rgbtoyuv(BmpBitDate);
	//char address3[] = "syyuv.bmp";
	//writemap(address3, BmpByte, BmpWidth, BmpHeight, NULL, bBmpBitDate);


	//将yuv图像反转化位rgb图像并输出
	cBmpBitDate = yuvtorgb(bBmpBitDate);
	char address4[] = "syrgb.bmp";
	writemap(address4, BmpByte, BmpWidth, BmpHeight, NULL, cBmpBitDate);


	//将yuv图像转化为灰度图
	dBmpBitDate = yuvtogray(bBmpBitDate);
	char address5[] = "sygray.bmp";
	writemap(address5, BmpByte, BmpWidth, BmpHeight, NULL, dBmpBitDate);


	//改变yuv图像中的Y分量的值	
	eBmpBitDate = changey(bBmpBitDate, Ychange);
	//char address6[] = "sychange.bmp";
	//writemap(address6, BmpByte, BmpWidth, BmpHeight, NULL, eBmpBitDate);


	//将改变Y分量值之后的yuv图像反转化为rgb图像并输出
	fBmpBitDate = yuvtorgb(eBmpBitDate);
	char address7[] = "sychangep.bmp";
	writemap(address7, BmpByte, BmpWidth, BmpHeight, NULL, fBmpBitDate);
	//printf("yes");
	//system("pause");

	//显示某张图像
	//system("sygray.bmp");
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
		 k= 0.299*((double)*(p + i)) + 0.587 * ((double)*(p + i + 1)) + 0.114*((double)*(p + i + 2));
		 if (k > 255)k = 255;
		 if (k < 0)k = 0;
		 yuv[i] = k;

		k = -0.148 * ((double)*(p + i)) - 0.289*((double)*(p + i + 1)) + 0.437*((double)*(p + i + 2));
		if (k > 122)k = 122;
		if (k < -122)k = -122;
		yuv[i+1] = k;

		k = 0.615 * ((double)*(p + i)) - 0.515*((double)*(p + i + 1)) - 0.100 * ((double)*(p + i + 2));
		if (k > 157)k = 157;
		if (k < -157)k = -157;
		yuv[i + 2] = k;


		/*
		//判断YUV值是否越界
		//YUV的取值范围：Y = 0~255, U = -122~+ 122, V = -157~+ 157
		if (yuv[i] > 255)yuv[i] = 255;
		if (yuv[i] < 0)yuv[i] = 0;
		if (yuv[i + 1] > 122)yuv[i + 1] = 122;
		if (yuv[i + 1] < -122)yuv[i + 1] = -122;
		if (yuv[i + 2] > 157)yuv[i + 2] = 157;
		if (yuv[i + 2] < -157)yuv[i + 2] = -157;
		*/
	}
	//返回处理后的位图数据起始指针
	return yuv;
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
		k= ((double)*(p + i)) + 1.140*((double)*(p + i + 2));
		if (k > 255)k= 255;
		if (k< 0)k = 0;
		rgb[i] = k;

		k = ((double)*(p + i)) - 0.395*((double)*(p + i + 1)) - 0.581*((double)*(p + i + 2));
		if (k > 255)k = 255;
		if (k < 0)k = 0;
		rgb[i+1] = k;

		k = ((double)*(p + i)) + 2.032*((double)*(p + i + 1));
		if (k > 255)k = 255;
		if (k < 0)k = 0;
		rgb[i + 2] = k;


		/*
		//判断RGB值是否越界
		//RGB的取值范围：R = 0~255, G = 0~255, B = 0~255,
		if (rgb[i] > 255)rgb[i] = 255;
		if (rgb[i] < 0)rgb[i] = 0;
		if (rgb[i + 1] > 255)rgb[i + 1] = 255;
		if (rgb[i + 1] < 0)rgb[i + 1] = 0;
		if (rgb[i + 2] > 255)rgb[i + 2] = 255;
		if (rgb[i + 2] < 0)rgb[i + 2] = 0;
		*/
	}
	//返回处理后的位图数据起始指针
	return rgb;
}

//函数：
//功能：
//yuv转gray
//参数：
//imgBuf：需要处理的位图数据起始指针
unsigned char*  yuvtogray(int *imgBuf)
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

		k= *(p + i);
		if (k > 255)k = 255;
		if (k < 0)k = 0;
	    gray[i] = k;

		gray[i+1]= gray[i];
		/*
		if (k > 255)k = 255;
		if (k < 0)k = 0;
		gray[i+1] = k;
		*/

		gray[i+2]= gray[i];
		/*
		if (k > 255)k = 255;
		if (k < 0)k = 0;
		gray[i+2] = k;
		*/

		//此处不需要判断数据是否越界
		//因为传入的YUV图像在处理时已经判断
	}
	//返回处理后的位图数据起始指针
	return gray;
}

//函数：
//功能：
//改变yuv中的Y分量
//参数：
//imgBuf：需要处理的位图数据起始指针
int*  changey(int *imgBuf,int Ychange)
{
	//定义处理后的指针
	int*change;
	int*p = imgBuf;
	int i;
	int k;
	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);


	//动态开辟空间存放处理后的数据
	change = new int[LineByte*BmpHeight];

	//处理数据
	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{
		 k= p[i] + Ychange;
		if (k > 255)k = 255;
		if (k < 0)k = 0;
		change[i] = k;
		change[i + 1] = p[i + 1];
		change[i + 2] = p[i + 2];

		//判断处理后的数据是否越界
		if (change[i] > 255)change[i] = 255;
		if (change[i] < 0)change[i] = 0;
	}
	//返回处理后的位图数据起始指针
	return change;
}

