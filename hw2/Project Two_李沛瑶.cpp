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
unsigned char*  yuvtogray(int *imgBuf, int *histogram);
int OTSU(int * histogram);
unsigned char*  graytobinary(unsigned char *imgBuf,int threshold);
unsigned char*  erosion(unsigned char *imgBuf);
unsigned char*  dilation(unsigned char *imgBuf);
unsigned char*  opening(unsigned char *imgBuf);
unsigned char*  closing(unsigned char *imgBuf);


//主函数
int main()
{
	//定义经过不同变换后的位图数据指针
	int *bBmpBitDate;
	unsigned char *cBmpBitDate;
	unsigned char *dBmpBitDate;
	//int *eBmpBitDate;
	unsigned char *fBmpBitDate;
	unsigned char *gBmpBitDate;
	unsigned char *hBmpBitDate;
	unsigned char *iBmpBitDate;
	//unsigned char *jBmpBitDate;
	//unsigned char *kBmpBitDate;

	//灰度分布直方图
	int histogram[256] = { 0 };
	//二值化的阈值
	int threshold;


	//原始位图数据指针
	char address[] = "lenacolor.bmp";
	//读入address1路径的24位bmp图像
	readBmp(address);

	//测试输出原始图像
	char address_test[] = "test.bmp";
	writemap(address_test, BmpByte, BmpWidth, BmpHeight, NULL, BmpBitDate);


	//将读入的rgb全彩色图像转化为yuv图像
	bBmpBitDate = rgbtoyuv(BmpBitDate);


	//将yuv图像转化为灰度图并输出
	cBmpBitDate = yuvtogray(bBmpBitDate,histogram);
	char address2[] = "gray.bmp";
	writemap(address2, BmpByte, BmpWidth, BmpHeight, NULL, cBmpBitDate);

	//使用大津算法计算阈值
	threshold = OTSU(histogram);

	//将灰度图转为二值图并输出
	dBmpBitDate = graytobinary(cBmpBitDate,threshold);
	char address3[] = "binary.bmp";
	writemap(address3, BmpByte, BmpWidth, BmpHeight, NULL, dBmpBitDate);

	//erosion操作
	fBmpBitDate = erosion(dBmpBitDate);
	char address4[] = "erosion.bmp";
	writemap(address4, BmpByte, BmpWidth, BmpHeight, NULL, fBmpBitDate);

	//dilation操作
	gBmpBitDate = dilation(dBmpBitDate);
	char address5[] = "dilation.bmp";
	writemap(address5, BmpByte, BmpWidth, BmpHeight, NULL, gBmpBitDate);

	//opening操作
	hBmpBitDate = opening(dBmpBitDate);
	char address6[] = "opening.bmp";
	writemap(address6, BmpByte, BmpWidth, BmpHeight, NULL, hBmpBitDate);

	//closing操作
	iBmpBitDate = closing(dBmpBitDate);
	char address7[] = "closing.bmp";
	writemap(address7, BmpByte, BmpWidth, BmpHeight, NULL, iBmpBitDate);

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
//yuv转gray
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
//功能：使用大津算法计算阈值
//参数：
//threshold：灰度分布直方图起始指针
int OTSU(int *histogram)
{
	int Maxvalue=0;
	int Minvalue=0;
	int i;
	int total = 0;

	int ForePixel=0;
	int BackPixel=0;
	int ForeGray=0;
	int BackGray=0;
	int pixel=0;

	int threshold;

	double ForeAver;
	double BackAver;
	double ForeProb;
	double BackProb;
	//double totalaver;

	double variance=0;
	double newvariance;
	//double anovariance;

	//得到最小灰度值
	for (i = 0; histogram[i] == 0; i++);
	Minvalue = i;
	//得到最大灰度值
	for (i = 255; histogram[i] == 0; i--);
	Maxvalue = i;

	for (i = Minvalue; i <= Maxvalue; i++)
	{
		pixel += histogram[i];
		total += histogram[i]*i;
	}
	BackPixel = pixel;
	BackGray = total;

	//totalaver = total / pixel;
	threshold = Minvalue;

	for (i = Minvalue; i <= Maxvalue; i++)
	{
		//前景图像像素点
		ForePixel += histogram[i];
		//后景图像像素点
		BackPixel -= histogram[i];
		//前景图像灰度总值
		ForeGray += histogram[i] * i;
		//后景图像灰度总值
		BackGray -= histogram[i] * i;

		//前景图像像素点百分比
		ForeProb = (double)ForePixel / pixel;
		//后景图像像素点百分比
		BackProb = (double)BackPixel / pixel;
		//前景图像灰度总值所占百分比
		ForeAver = (double)ForeGray / ForePixel;
		//后景图像灰度总值所占百分比
		BackAver = (double)BackGray / BackPixel;

		//计算
		newvariance = ForeProb * BackProb*(ForeAver - BackAver)*(ForeAver - BackAver);
		//另一种算法（做验证用）
		//anovariance = ForeProb * (totalaver - ForeAver)*(totalaver - ForeAver) + BackProb * (totalaver - BackAver)*(totalaver - BackAver);
		if (newvariance > variance)
		{
			variance = newvariance;
			threshold = i;
		}

	}
	return threshold;
}

//函数：
//功能：将灰度图二值化
//参数：
//imgBuf：需要处理的位图数据起始指针
//threshold：灰度分布直方图起始指针
unsigned char*  graytobinary(unsigned char *imgBuf, int threshold)
{
	unsigned char* binary;
	unsigned char* p = imgBuf;
	int i;
	int k;
	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);


	//动态开辟空间存放处理后的数据
	binary = new unsigned char[LineByte*BmpHeight];

	//处理数据
	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{

		k = *(p + i);
		if (k >= threshold)k = 255;
		else if (k < threshold)k = 0;
		binary[i] = k;


		binary[i + 1] = binary[i];
		binary[i + 2] = binary[i];

	}
	return binary;
}


//函数：
//功能：用标准十字块对二值化图像进行dilation（膨胀）操作
//参数：
//imgBuf：需要处理的位图数据起始指针
unsigned char*  dilation(unsigned char *imgBuf)
{
	unsigned char* dila;
	unsigned char* p = imgBuf;
	int i, j;
	int k;
	int m, n;
	int u;
	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);


	//动态开辟空间存放处理后的数据
	dila = new unsigned char[LineByte*BmpHeight];
	

	//处理数据
	for (i = 0; i < BmpHeight; i = i + 1)
	{
		for (j = 0; j  < LineByte; j = j + 3)
		{
            //计算数组下标
			u = i * LineByte + j;
			//处理上边缘和下边缘
			if ((i == 0) || (i == BmpHeight-1))
			{
				dila[u] = *(p + u);
			}
			//处理左边缘和右边缘
			else if((j==0)||(j>= (BmpByte * BmpWidth)/8 - 3)){
				dila[u] = *(p + u);
			}
			//处理中间部分数据
			else {
				
				k = *(p + u);
				if (k == 0) { dila[u] = 0; }
				else
				{
					m = *(p + u - 3);
					n = *(p + u + 3);
					if ((m == 0) || (n == 0))
					{
						dila[u] = 0;
					}
					else {
						m = *(p + u - LineByte);
						n = *(p + u + LineByte);
						if ((m == 0) || (n == 0))
						{
							dila[u] = 0;
						}
						else {
							dila[u] = 255;
						}
					}
				}

			}
			//对UV分量赋值
			dila[u + 1] = dila[u];
			dila[u + 2] = dila[u];

		}
	}

	return dila;

}

//函数：
//功能：用标准十字块对二值化图像进行erosion（腐蚀）操作
//参数：
//imgBuf：需要处理的位图数据起始指针
unsigned char*  erosion(unsigned char *imgBuf)
{
	unsigned char* eros;
	unsigned char* p = imgBuf;
	int i, j;
	int k;
	int m, n;
	int u;
	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);


	//动态开辟空间存放处理后的数据
	eros = new unsigned char[LineByte*BmpHeight];

	

	//处理数据
	for (i = 0; i < BmpHeight ; i = i + 1)
	{
		for (j = 0; j  < LineByte; j = j + 3)
		{
			//计算数组下标
			u = i * LineByte + j;

			//处理上边缘和下边缘
			if ((i == 0) || (i == BmpHeight-1))
			{
				eros[u] = *(p + u);
			}
			//处理左边缘和右边缘
			else if ((j == 0) || (j >= (BmpByte * BmpWidth)/8 - 3)) {
				eros[u] = *(p + u);
			}
			//处理中间部分数据
			else {
				k = *(p + u);
				if (k == 255) { eros[u] = 255; }
				else
				{
					m = *(p + u - 3);
					n = *(p + u + 3);
					if ((m == 255) || (n == 255))
					{
						eros[u] = 255;
					}
					else {
						m = *(p + u - LineByte);
						n = *(p + u + LineByte);
						if ((m == 255) || (n == 255))
						{
							eros[u] = 255;
						}
						else {
							eros[u] = 0;
						}
					}
				}
			}
			//对UV分量赋值
			eros[u + 1] = eros[u];
			eros[u + 2] = eros[u];
		}
	}

	return eros;

}

//函数：
//功能：用标准十字块对二值化图像进行opening（开）操作
//参数：
//imgBuf：需要处理的位图数据起始指针
unsigned char*  opening(unsigned char *imgBuf)
{
	unsigned char * p;
	p = erosion(imgBuf);
	p = dilation(p);
	return p;
}

//函数：
//功能：用标准十字块对二值化图像进行closing（闭）操作
//参数：
//imgBuf：需要处理的位图数据起始指针
unsigned char*  closing(unsigned char *imgBuf)
{
	unsigned char * p;
	p = dilation(imgBuf);
	p = erosion(p);
	return p;
}