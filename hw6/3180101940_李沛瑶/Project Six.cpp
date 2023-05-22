#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<cmath>
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

//变换色彩空间及转灰度图
unsigned char**  rgbtoyuv(unsigned char **imgBuf);
unsigned char**  yuvtorgb(unsigned char **imgBuf);

unsigned char point_filtering(unsigned char **imgBuf, int loci, int locj,  int size);
unsigned char **one_filtering(unsigned char **imgBuf, int size);
unsigned char **three_filtering(unsigned char **imgBuf, int size);
unsigned char **enhance(unsigned char **imgBuf, int **laplace);

//主函数
int main()
{	
	//定义经过不同变换后的位图数据指针
	unsigned char **bBmpBitDate;
	unsigned char **dBmpBitDate;
	unsigned char **eBmpBitDate;
	unsigned char **fBmpBitDate;
	unsigned char **gBmpBitDate;
	unsigned char **hBmpBitDate;

	int filter_size = 11;
	//原始位图数据指针
	char address[] = "castle.bmp";
	//读入address1路径的24位bmp图像
	readBmp2(address);
	
	//对Y通道进行双线插值
	bBmpBitDate = rgbtoyuv(BmpBitDate);
	dBmpBitDate = one_filtering(bBmpBitDate,filter_size);
	eBmpBitDate = yuvtorgb(dBmpBitDate);

	//对RGB三个通道进行双线插值
	fBmpBitDate = three_filtering(BmpBitDate,filter_size);

	//对YUV三个通道进行双线插值
	gBmpBitDate = three_filtering(bBmpBitDate, filter_size);
	hBmpBitDate = yuvtorgb(gBmpBitDate);

	char address1[] = "Y_Binary.bmp";
	writemap2(address1, BmpByte, BmpWidth, BmpHeight, NULL, eBmpBitDate);

	char address2[] = "RGB_Binary.bmp";
	writemap2(address2, BmpByte, BmpWidth, BmpHeight, NULL, fBmpBitDate);

	char address3[] = "YUV_Binary.bmp";
	writemap2(address3, BmpByte, BmpWidth, BmpHeight, NULL, hBmpBitDate);

	system("pause");
}

//函数：
//功能：
//对图像进行滤波
//参数：
//imgBuf：位图数据头指针
//size：滤波器大小（长宽）
unsigned char **one_filtering(unsigned char **imgBuf, int size)
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
	for (i = 0; i < BmpHeight; i++)
	{
		for (j = 0; j < LinePixel; j++)
		{
			if ((i < mid) || (i >= BmpHeight - mid) || (j < mid) || (j >= LinePixel - mid))
			{
				res[i][j * 3] = p[i][j * 3];
				res[i][j * 3 + 1] = p[i][j * 3 + 1];
				res[i][j * 3 + 2] = p[i][j * 3 + 2];
			}
			else
			{
				res[i][j * 3] = point_filtering(p, i, j * 3, size);
				res[i][j * 3 + 1] = p[i][j * 3 + 1];
				res[i][j * 3 + 2] = p[i][j * 3 + 2];
			}
		}
	}
	return res;
}

//函数：
//功能：
//对图像进行滤波
//参数：
//imgBuf：位图数据头指针
//size：滤波器大小（长宽）
unsigned char **three_filtering(unsigned char **imgBuf, int size)
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
	for (i = 0; i < BmpHeight; i++)
	{
		for (j = 0; j < LinePixel; j++)
		{
			if ((i < mid) || (i >= BmpHeight - mid) || (j < mid) || (j >= LinePixel - mid))
			{
				res[i][j * 3] = p[i][j * 3];
				res[i][j * 3 + 1] = p[i][j * 3 + 1];
				res[i][j * 3 + 2] = p[i][j * 3 + 2];
			}
			else
			{
				res[i][j * 3] = point_filtering(p, i, j * 3, size);
				res[i][j * 3+1] = point_filtering(p, i, j * 3+1, size);
				res[i][j * 3+2] = point_filtering(p, i, j * 3+2, size);
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
//size：滤波器大小（长宽）
//loci、locj：当前像素在位图数组中的坐标
unsigned char point_filtering(unsigned char **imgBuf, int loci, int locj, int size)
{
	//结果储存变量
	double totalw = 0;
	double total = 0;
	double Gs;
	double Gr;
	double Gsindex;
	double Grindex;
	double omigs = 18;
	double omigr = 1;

	int sum = 0;
	unsigned char result;
	//得到滤波器首行首列相对于中心点的偏移量
	int mid = (size - 1) / 2;
	//位图数据头指针
	unsigned char **p = imgBuf;
	int i, j;

	//处理数据
	for (i = 0; i < size; i++)
	{
		for (j = 0; j < size; j++)
		{
			Grindex = pow((p[loci - mid + i][locj - mid * 3 + j * 3] - p[loci][locj]), 2) / 2 / pow(omigr, 2);
			Gsindex = (pow(i - mid, 2) + pow(j - mid, 2)) / 2 / pow(omigs, 2);

			Gr = exp(-Grindex);
			Gs = exp(-Gsindex);

			totalw += Gr * Gs;
			total += Gr * Gs*p[loci - mid + i][locj - mid * 3 + j * 3];
		}
	}
	result = total / totalw;

	return result;
}

//函数：
//功能：
//yuv转rgb
//参数：
//imgBuf：需要处理的位图数据起始指针
unsigned char**  yuvtorgb(unsigned char **imgBuf)
{
	//定义处理后的指针
	unsigned char**rgb;
	unsigned char**p = imgBuf;

	int k;

	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);
	int LinePixel = LineByte / 3;

	//动态开辟空间存放处理后的数据
	rgb = new unsigned char*[BmpHeight];
	for (int i = 0; i < BmpHeight; i++)
	{
		rgb[i] = new unsigned char[LineByte];
	}

	//处理数据
	for (int i = 0; i < BmpHeight; i++)
	{
		for (int j = 0; j < LinePixel; j++)
		{
			k = ((double)p[i][j * 3]) + 1.140*((double)p[i][j * 3 + 2] - 128);
			if (k > 255)k = 255;
			if (k < 0)k = 0;
			rgb[i][j * 3] = k;

			k = ((double)p[i][j * 3]) - 0.395*((double)p[i][j * 3 + 1] - 128) - 0.581*((double)p[i][j * 3 + 2] - 128);
			if (k > 255)k = 255;
			if (k < 0)k = 0;
			rgb[i][j * 3 + 1] = k;

			k = ((double)p[i][j * 3]) + 2.032*((double)p[i][j * 3 + 1] - 128);
			if (k > 255)k = 255;
			if (k < 0)k = 0;
			rgb[i][j * 3 + 2] = k;

		}
	}

	//返回处理后的位图数据起始指针
	return rgb;
}


//函数：
//功能：
//rgb转yuv
//参数：
//imgBuf：需要处理的位图数据起始指针
unsigned char** rgbtoyuv(unsigned char** imgBuf)
{
	//定义处理后的指针
	unsigned char** yuv;
	unsigned char** p = imgBuf;

	int k;

	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);
	int LinePixel = LineByte / 3;

	//动态开辟空间存放处理后的数据
	yuv = new unsigned char*[BmpHeight];
	for (int i = 0; i < BmpHeight; i++)
	{
		yuv[i] = new unsigned char[LineByte];
	}
	//处理数据
	for (int i = 0; i < BmpHeight; i++)
	{
		for (int j = 0; j < LinePixel; j++)
		{
			k = 0.299*((double)p[i][j * 3]) + 0.587 * ((double)p[i][j * 3 + 1]) + 0.114*((double)p[i][j * 3 + 2]);
			if (k > 255)k = 255;
			if (k < 0)k = 0;
			yuv[i][j * 3] = k;

			k = 128 - 0.148 * ((double)p[i][j * 3]) - 0.289*((double)p[i][j * 3 + 1]) + 0.437*((double)p[i][j * 3 + 2]);
			if (k > 255)k = 255;
			if (k < 0)k = 0;
			//if (k > 255)k = 122;
			//if (k < 0)k = -122;
			yuv[i][j * 3 + 1] = k;

			k = 128 + 0.615 * ((double)p[i][j * 3]) - 0.515*((double)p[i][j * 3 + 1]) - 0.100 * ((double)p[i][j * 3 + 2]);
			if (k > 255)k = 255;
			if (k < 0)k = 0;
			//if (k > 157)k = 157;
			//if (k < -157)k = -157;
			yuv[i][j * 3 + 2] = k;
		}

	}
	//返回处理后的位图数据起始指针
	return yuv;
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


