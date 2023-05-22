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

//记录均值化过程中的映射关系
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
unsigned char*  rgbtoyuv(unsigned char *imgBuf);
unsigned char*  yuvtorgb(unsigned char *imgBuf);
//对三个色彩空间的分量做均值化处理
unsigned char*  equalize(unsigned char * imgBuf);
//只对Y分量做均值化处理
unsigned char*  equalize2(unsigned char * imgBuf,int choice);
//得到映射关系
void  getequalize(unsigned char * imgBuf,int equaenergy);


//主函数
int main()
{
	//定义经过不同变换后的位图数据指针
	unsigned char *bBmpBitDate;
	unsigned char *cBmpBitDate;
	unsigned char *dBmpBitDate;
	unsigned char *eBmpBitDate;
	unsigned char *fBmpBitDate;
	unsigned char *gBmpBitDate;

	//映射结果所分的等级数
	int equaenergy = 256;

	//原始位图数据指针
	char address[] = "pic5.bmp";
	//读入address1路径的24位bmp图像
	readBmp(address);

	//测试输出原始图像
	char address_test[] = "test.bmp";
	writemap(address_test, BmpByte, BmpWidth, BmpHeight, NULL, BmpBitDate);

	//RGB转YUV
	bBmpBitDate = rgbtoyuv(BmpBitDate);

	//彩色图的RGB分量直接均衡化
	getequalize(BmpBitDate,equaenergy);
	cBmpBitDate = equalize(BmpBitDate);
	char address1[] = "equaRGB.bmp";
	writemap(address1, BmpByte, BmpWidth, BmpHeight, NULL, cBmpBitDate);

	//彩色图的Y分量均衡化
	getequalize(bBmpBitDate, equaenergy);
	dBmpBitDate = equalize2(bBmpBitDate,1);
	eBmpBitDate = yuvtorgb(dBmpBitDate);
	char address2[] = "equaY.bmp";
	writemap(address2, BmpByte, BmpWidth, BmpHeight, NULL, eBmpBitDate);

	//彩色图的YUV分量均衡化
	getequalize(bBmpBitDate, equaenergy);
	fBmpBitDate = equalize(bBmpBitDate);
	gBmpBitDate = yuvtorgb(fBmpBitDate);
	char address3[] = "equaYUV.bmp";
	writemap(address3, BmpByte, BmpWidth, BmpHeight, NULL, gBmpBitDate);
	
	system("pause");

}

//函数：
//功能：
//得到映射关系
//参数：
//imgBuf：需要处理的位图起始指针
//equaenergy：映射结果分几个等级
void  getequalize(unsigned char * imgBuf,int equaenergy)
{
	unsigned char* getequa;
	unsigned char* p = imgBuf;
	double histor[256] = { 0,0 };
	double histog[256] = { 0,0 };
	double histob[256] = { 0,0 };

	//rgb分量当前值是否已经计算出映射值的控制变量
	int flagr=0;
	int flagg = 0;
	int flagb = 0;

	int i;
	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);


	//动态开辟空间存放处理后的数据
	getequa = new unsigned char[LineByte*BmpHeight];

	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{
		//统计三个色彩空间分量的直方图
		histor[*(p+i)]++;
		histog[*(p+i+1)]++;
		histob[*(p+i+2)]++;
	}

	//对直方图累加计算Sk的值
	for (i = 1; i < 256; i++)
	{
		histor[i] += histor[i - 1];
		histog[i] += histog[i - 1];
		histob[i] += histob[i - 1];
	}

	//得到概率值的累加值
	for (i = 0; i < 256; i++)
	{
		histor[i]/= histob[255];
		histog[i]/= histob[255];
		histob[i]/= histob[255];
	}

	//根据映射结果等级数得出每一个映射等级的区间宽度
	double equacolor = 256 / equaenergy;

	//计算三个色彩空间直方图的映射值
	for (i = 0; i < 256; i++)
	{
		flagr = flagg = flagb = 0;
		for (int j = 0; j < equaenergy; j++)
		{
			//计算第一个色彩空间的映射值
			if (flagr == 0) {
				if ((histor[i] >= j / double(equaenergy - 1)) && (histor[i] <= (j + 1) / double(equaenergy - 1)))
				{
					if ((histor[i] - j / double(equaenergy - 1)) > (double(equaenergy - 1) / 255.0 - histor[i]))
					{
						funr[i] = (j + 1)*equacolor;
					}
					else {
						funr[i] = j * equacolor;
					}
					flagr = 1;
				}
			}
			//计算第二个色彩空间的映射值
			if (flagg == 0) {
				if ((histog[i] >= j / double(equaenergy - 1)) && (histog[i] <= (j + 1) / double(equaenergy - 1)))
				{
					if ((histog[i] - j / double(equaenergy - 1)) > ((j + 1) / double(equaenergy - 1) - histog[i]))
					{
						fung[i] = (j + 1)*equacolor;
					}
					else {
						fung[i] = j * equacolor;
					}
					flagg = 1;
				}
			}
			//计算第三个色彩空间的映射值
			if (flagb == 0) {
				if ((histob[i] >= j / double(equaenergy - 1)) && (histob[i] <= (j + 1) / double(equaenergy - 1)))
				{
					if ((histob[i] - j / double(equaenergy - 1)) > ((j + 1) / double(equaenergy - 1)-histob[i]))
					{
						funb[i] = (j + 1)*equacolor;
					}
					else {
						funb[i] = j * equacolor;
					}
					flagb = 1;
				}
			}
			if (flagr&&flagg&&flagb)
			{
				break;
			}
		}
	}

}

//函数：
//功能：
//根据映射关系将直方图映射到均衡化后的直方图
//参数：
//imgBuf：需要处理的位图起始指针
//choice：变换第几个色彩空间的分量
unsigned char*  equalize(unsigned char * imgBuf)
{
	int r, g, b;
	unsigned char* equa;
	unsigned char* p = imgBuf;
	int i;

	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);

	//动态开辟空间存放处理后的数据
	equa = new unsigned char[LineByte*BmpHeight];

	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{
		//得到映射值
		r = funr[*(p + i)];
		g = fung[*(p + i + 1)];
		b = funb[*(p + i + 2)];
		
		//进行映射
		equa[i] = r;
		equa[i+1] = g;
		equa[i+2] = b;
	}
	return equa;

}

//函数：
//功能：
//根据映射关系将直方图映射到均衡化后的直方图
//（只改变一个分量）
//参数：
//imgBuf：需要处理的位图起始指针
unsigned char*  equalize2(unsigned char * imgBuf,int choice)
{
	int fun;
	unsigned char* equa;
	unsigned char* p = imgBuf;
	int i;

	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);


	//动态开辟空间存放处理后的数据
	equa = new unsigned char[LineByte*BmpHeight];

	//处理数据
	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{
		//根据选择变换的分量，计算映射值并进行映射
		if (choice == 1) {
            fun = funr[*(p + i)];
            equa[i] = fun;
			equa[i +1] = *(p + i + 1);
			equa[i + 2] = *(p + i + 2);
		}
		else if (choice == 2) {
			fun = fung[*(p + i)];
			equa[i+1] = fun;
			equa[i] = *(p + i);
			equa[i + 2] = *(p + i + 2);
		}
		else if (choice == 3) {
			fun = funb[*(p + i)];
			equa[i+2] = fun;
			equa[i + 1] = *(p + i + 1);
			equa[i] = *(p + i);
		}
	}
	return equa;

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
unsigned char*  yuvtorgb(unsigned char *imgBuf)
{
	//定义处理后的指针
	unsigned char*rgb;
	unsigned char*p = imgBuf;
	int i;
	int k;
	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);

	//动态开辟空间存放处理后的数据
	rgb = new unsigned char[LineByte*BmpHeight];

	//处理数据
	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{
		k = ((double)*(p + i)) + 1.140*((double)*(p + i + 2)-128);
		if (k > 255)k = 255;
		if (k < 0)k = 0;
		rgb[i] = k;

		k = ((double)*(p + i)) - 0.395*((double)*(p + i + 1)-128) - 0.581*((double)*(p + i + 2)-128);
		if (k > 255)k = 255;
		if (k < 0)k = 0;
		rgb[i + 1] = k;

		k = ((double)*(p + i)) + 2.032*((double)*(p + i + 1)-128);
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
unsigned char*  rgbtoyuv(unsigned char *imgBuf)
{
	//定义处理后的指针
	unsigned char* yuv;
	unsigned char*p = imgBuf;
	int i;
	int k;
	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);

	//动态开辟空间存放处理后的数据
	yuv = new unsigned char[LineByte*BmpHeight];

	//处理数据
	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{
		k = 0.299*((double)*(p + i)) + 0.587 * ((double)*(p + i + 1)) + 0.114*((double)*(p + i + 2));
		if (k > 255)k = 255;
		if (k < 0)k = 0;
		yuv[i] = k;

		k =128 -0.148 * ((double)*(p + i)) - 0.289*((double)*(p + i + 1)) + 0.437*((double)*(p + i + 2));
		if (k > 255)k = 255;
		if (k < 0)k = 0;
		//if (k > 255)k = 122;
		//if (k < 0)k = -122;
		yuv[i + 1] = k;

		k = 128+0.615 * ((double)*(p + i)) - 0.515*((double)*(p + i + 1)) - 0.100 * ((double)*(p + i + 2));
		if (k > 255)k = 255;
		if (k < 0)k = 0;
		//if (k > 157)k = 157;
		//if (k < -157)k = -157;
		yuv[i + 2] = k;

	}
	//返回处理后的位图数据起始指针
	return yuv;
}



