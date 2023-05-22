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
unsigned char *BmpBitDate;//bmp图像位图数据的起始指针


//函数声明
bool readBmp(char *address);
bool writemap(char *address, int BmpByte, int BmpWidth, int BmpHeight, RGBQUAD *RGBpalette, unsigned char *BmpBitDate);

unsigned char*  x_mirror(unsigned char *imgBuf);
unsigned char*  y_mirror(unsigned char *imgBuf);
unsigned char*  translation(unsigned char *imgBuf, int xtrans, int ytrans);//裁剪，保持画布大小不变
unsigned char*  translation2(unsigned char *imgBuf, int xtrans, int ytrans);//不裁剪，保证原图数据都被保留
unsigned char*  scale(unsigned char *imgBuf, double xsca, double ysca);//伸缩，不进行二维线性插值
unsigned char*  scale2(unsigned char *imgBuf, double xsca, double ysca);//伸缩，进行二维线性插值
unsigned char*  shear(unsigned char *imgBuf, double dx);
unsigned char*  rotation(unsigned char *imgBuf, double angle);
unsigned char*  interpolation(unsigned char *imgBuf, double angle);
unsigned char*  rotation2(unsigned char *imgBuf, double angle);

//主函数
int main()
{
	//图像变换参数
	//平移变换参数：
	//x向右为正方向，y向上为正方向
	int xtrans = -50;
	int ytrans = 80;
	//伸缩变换参数
	//图像横向变换为原来的xsca倍，纵向变换为原来的ysca倍
	double xsca = 5;
	double ysca = 5;
	//侧切（shear）变换参数
	double dx = 1.2;
	double angle = 30 * PI / 180;

	//定义经过不同变换后的位图数据指针
	unsigned char *bBmpBitDate;
	unsigned char *cBmpBitDate;
	unsigned char *dBmpBitDate;
	unsigned char *eBmpBitDate;
	unsigned char *fBmpBitDate;
	unsigned char *gBmpBitDate;
	unsigned char *hBmpBitDate;
	unsigned char *iBmpBitDate;
	unsigned char *jBmpBitDate;

	//原始位图数据指针
	char address[] = "lena.bmp";
	//读入address1路径的24位bmp图像
	readBmp(address);

	//测试输出原始图像
	char address_test[] = "test.bmp";
	writemap(address_test, BmpByte, BmpWidth, BmpHeight, NULL, BmpBitDate);

	
	//横向镜像变换并输出
	bBmpBitDate = x_mirror(BmpBitDate);
	char address1[] = "x_mirror.bmp";
	writemap(address1, BmpByte, BmpWidth, BmpHeight, NULL, bBmpBitDate);

	//纵向镜像变换并输出
	cBmpBitDate = y_mirror(BmpBitDate);
	char address2[] = "y_mirror.bmp";
	writemap(address2, BmpByte, BmpWidth, BmpHeight, NULL, cBmpBitDate);

	//平移变换，裁剪，画布大小不变
	dBmpBitDate = translation(BmpBitDate, xtrans, ytrans);
	char address3[] = "trans.bmp";
	writemap(address3, BmpByte, BmpWidth, BmpHeight, NULL, dBmpBitDate);

	//平移变换，不裁剪，画布大小根据平移量改变
	eBmpBitDate = translation2(BmpBitDate,xtrans,ytrans);
	char address4[] = "trans2.bmp";
	writemap(address4, BmpByte, BmpWidth+abs(xtrans), BmpHeight+abs(ytrans), NULL, eBmpBitDate);


	//伸缩，不进行二维线性插值
	fBmpBitDate = scale(BmpBitDate, xsca,ysca);
	char address5[] = "scale.bmp";
	writemap(address5, BmpByte, floor(BmpWidth*xsca), floor(BmpHeight*ysca), NULL, fBmpBitDate);
	

	//伸缩，进行二维线性插值
	gBmpBitDate = scale2(BmpBitDate, xsca, ysca);
	char address6[] = "scale2.bmp";
	writemap(address6, BmpByte, floor(BmpWidth*xsca), floor(BmpHeight*ysca), NULL, gBmpBitDate);

	//斜切图像并输出
	gBmpBitDate = shear(BmpBitDate, dx);
	char address7[] = "shear.bmp";
	writemap(address7, BmpByte, floor(BmpWidth), floor(BmpHeight + floor(dx*BmpWidth)), NULL, gBmpBitDate);

	//旋转图像并输出
	hBmpBitDate = rotation(BmpBitDate, angle);
	char address8[] = "rotation.bmp";
	writemap(address8, BmpByte, floor(BmpWidth*cos(angle) + BmpHeight * sin(angle)), floor(BmpWidth*sin(angle) + BmpHeight * cos(angle)), NULL, hBmpBitDate);

	//对旋转图像进行空处插值并输出
	iBmpBitDate = interpolation(hBmpBitDate, angle);
	char address9[] = "inter.bmp";
	writemap(address9, BmpByte, floor(BmpWidth*cos(angle) + BmpHeight * sin(angle)), floor(BmpWidth*sin(angle) + BmpHeight * cos(angle)), NULL, iBmpBitDate);

	//旋转图像进行二位线性插值并输出
	jBmpBitDate = rotation2(BmpBitDate, angle);
	char address10[] = "rotation2.bmp";
	writemap(address10, BmpByte, floor(BmpWidth*cos(angle) + BmpHeight * sin(angle)), floor(BmpWidth*sin(angle) + BmpHeight * cos(angle)), NULL, jBmpBitDate);


    system("pause");
}

//函数：
//功能：
//对图像进行旋转(进行二维线性插值)
//参数：
//imgBuf：原始图像的位图数据起始指针
//angle：旋转角度参数
unsigned char*  rotation2(unsigned char *imgBuf, double angle)
{
	unsigned char* rot;
	unsigned char*p = imgBuf;
	int i, j;

	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);
	int RotWidth = floor(BmpWidth*cos(angle) + BmpHeight * sin(angle));
	int RotHeight = floor(BmpWidth*sin(angle) + BmpHeight * cos(angle));
	int RotLine = WIDTHBYTES(BmpByte * RotWidth);
	int RotPixel = RotLine / 3;
	int LinePixel = LineByte / 3;
	int orix = 0;
	int oriy = BmpHeight - 1;
	int a, b, c, d;
	double aa, cc;
	int result[3];

	//计算旋转中心的变换
	double coss = cos(angle);
	double sinn = sin(angle);
	int rotorix = -orix * coss + oriy * sinn + orix;
	int rotoriy = -orix * sinn - oriy * coss + oriy;
	rotoriy = 0;

	rot = new unsigned char[RotLine*RotHeight];

	//将初始值全部置为0
	for (i = 0; i < RotHeight; i++)
	{
		for (j = 0; j < RotLine; j++)
		{
			rot[i*RotLine + j] = 0;
		}
	}
	//return rot;

	//处理数据
	for (i = 0; i < RotHeight; i++)
	{
		for (j = 0; j < RotPixel; j++)
		{
			//计算变换前后对应的坐标
			int rotloc = i * RotLine + j * 3;
			double locx = ((j-rotorix) * coss + (i-rotoriy) * sinn);
			double locy = (-(j - rotorix)* sinn + (i - rotoriy) * coss);
			int ii = floor(locy);
			int jj = floor(locx);

			//根据不同的值（边界值，图像内部值，不在图像内的值），对数据进行处理
			if ((ii < 0) || (ii >= BmpHeight) || (jj < 0) || (jj >= LinePixel))
			{
				result[0]=result[1]=result[2]=0;
			}
			else if ((ii == BmpHeight - 1) || (jj == LinePixel-1))
			{
				for (int k = 0; k < 3; k++)
				{
					a = p[ii * LineByte + jj * 3 + k];
					result[k] = a;
				}
			}
			else
			{
				for (int k = 0; k < 3; k++)
				{


					a = p[ii * LineByte + jj * 3 + k];
					b = p[ii * LineByte + (jj + 1) * 3 + k];
					c = p[(ii + 1) * LineByte + jj * 3 + k];
					d = p[(ii + 1) * LineByte + (jj + 1) * 3 + k];

					aa = a * (jj + 1 - locx) + b * (locx - jj);
					cc = c * (jj + 1 - locx) + d * (locx - jj);

					result[k] = aa * (ii + 1 - locy) + cc * (locy - ii);
				}
			}
			//对结果图像中该像素点的三个分量分别赋值
			rot[rotloc] = result[0];
			rot[rotloc + 1] = result[1];
			rot[rotloc + 2] = result[2];
		}
		//printf("%d#%d\n", i, j);
	}
	
	return rot;
}

//函数：
//功能：
//对图像进行旋转
//参数：
//imgBuf：原始图像的位图数据起始指针
//angle：旋转角度参数
unsigned char*  rotation(unsigned char *imgBuf, double angle)
{
	unsigned char* rot;
	unsigned char*p = imgBuf;
	int i, j;

	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);
	int RotWidth = floor(BmpWidth*cos(angle) + BmpHeight * sin(angle));
	int RotHeight = floor(BmpWidth*sin(angle) + BmpHeight * cos(angle));
	int RotLine = WIDTHBYTES(BmpByte * RotWidth);
	int RotPixel = RotLine/3;
	int LinePixel = LineByte / 3;
	int orix = 0;
	int oriy = BmpHeight-1;
	
	//计算旋转中心的变换
	double coss = cos(angle);
	double sinn = sin(angle);
	int rotorix = -orix * coss + oriy * sinn + orix;
	int rotoriy = -orix * sinn - oriy * coss + oriy;
	rotoriy = 0;

	rot = new unsigned char[RotLine*RotHeight];

	//将初始值全部置为0
	for (i = 0; i < RotHeight; i++)
	{
		for (j = 0; j < RotLine; j++)
		{
			rot[i*RotLine + j] = 0;
		}
	}

	//处理数据
	for (i = 0; i < BmpHeight; i++)
	{
		for (j = 0; j < LinePixel; j++)
		{
			//计算变换前后对应的坐标
			int loc = i * LineByte + j * 3;
			int rotx = i * coss - j * sinn + rotorix;
			int roty = i * sinn + j * coss + rotoriy;
			int rotloc = rotx * RotLine + roty * 3;

				rot[rotloc] = p[loc];
				rot[rotloc + 1] = p[loc + 1];
				rot[rotloc + 2] = p[loc + 2];
		}
	}

	return rot;
}


//函数：
//功能：
//对旋转后的图像进行插值
//参数：
//imgBuf：原始图像的位图数据起始指针
//angle：旋转角度参数
unsigned char*  interpolation(unsigned char *imgBuf, double angle)
{
	unsigned char* inter;
	unsigned char*p = imgBuf;
	int i, j;

	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);
	int RotWidth = floor(BmpWidth*cos(angle) + BmpHeight * sin(angle));
	int RotHeight = floor(BmpWidth*sin(angle) + BmpHeight * cos(angle));
	int RotLine = WIDTHBYTES(BmpByte * RotWidth);
	int RotPixel = RotLine / 3;
	int LinePixel = LineByte / 3;
	int orix = 0;
	int oriy = BmpHeight - 1;
	int a, b, c, d, e, f, g, h;
	int result[3];

	double coss = cos(angle);
	double sinn = sin(angle);
	int rotorix = -orix * coss + oriy * sinn + orix;
	int rotoriy = -orix * sinn - oriy * coss + oriy;
	rotoriy = 0;

	//将初始值全部置为0
	inter = new unsigned char[RotLine*RotHeight];
	for (i = 0; i < RotHeight; i++)
	{
		for (j = 0; j < RotLine; j++)
		{
			inter[i*RotLine + j] = p[i*RotLine + j];
		}
	}
	//处理数据
	for (i = 1; i < RotHeight - 1; i++)
	{
		for (j = 1; j < RotPixel - 1; j++)
		{
			if (inter[i*RotLine + j * 3] == 0)
			{
				for (int k = 0; k < 3; k++)
				{
					//计算前后左右八个像素点的数值
					a = inter[(i - 1)*RotLine + (j - 1) * 3 + k];
					b = inter[(i - 1)*RotLine + j * 3 + k];
					c = inter[(i - 1)*RotLine + (j + 1) * 3 + k];
					d = inter[i*RotLine + (j - 1) * 3 + k];
					e = inter[i*RotLine + (j + 1) * 3 + k];
					f = inter[(i + 1)*RotLine + (j - 1) * 3 + k];
					g = inter[(i + 1)*RotLine + j * 3 + k];
					h = inter[(i + 1)*RotLine + (j + 1) * 3 + k];
					if (!(a == 0 || b == 0 || c == 0 || d == 0 || e == 0 || f == 0 || g == 0 || h == 0))
					{
						//取平均对结果数组赋值
						result[k] = (a + b + c + d + e + f + g + h) / 8;
						inter[i*RotLine + j * 3] = result[0];
						inter[i*RotLine + j * 3 + 1] = result[1];
						inter[i*RotLine + j * 3 + 2] = result[2];
						//result[k] = inter[i*RotLine + j * 3 + k];

					}
					else {
						continue;
						//result[k] = (a + b + c + d + e + f + g + h) / 8;
						//result[k]= p[i*RotLine + j * 3 + k];
					}

				}
			}
		}
	}
	return inter;
}

//函数：
//功能：
//对图像进行斜切
//参数：
//imgBuf：原始图像的位图数据起始指针
//dx：斜切变换参数
unsigned char*  shear(unsigned char *imgBuf, double dx)
{
	unsigned char* she;
	unsigned char*p = imgBuf;
	int i, j;

	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);
	//计算变换后每行像素数
	int ShearWidth = BmpWidth;
	//计算变换后每列像素数
	int ShearHeight = floor(BmpHeight + dx * BmpWidth);
	int LinePixel = LineByte / 3;


	//动态开辟空间存放处理后的数据
	she = new unsigned char[LineByte*ShearHeight];

	//将初始值全部置为0
	for (i = 0; i < ShearHeight; i++)
	{
		for (j = 0; j < LineByte; j++)
		{
			she[i * LineByte + j] = 0;
		}
	}


	//处理数据
	for (i = 0; i < ShearHeight; i++)
	{
		for (j = 0; j < LinePixel; j++)
		{
			//分别计算变换前后的像素坐标以及其对应的数组下标
			int sheloc = i * LineByte + j * 3;
			int ii = i - floor(dx*j);
			int loc = ii * LineByte + j * 3;
			//对三个分量分别进行赋值
			if ((ii >= 0) && (ii < BmpHeight))
			{
				she[sheloc] = p[loc];
				she[sheloc+1] = p[loc+1];
				she[sheloc+2] = p[loc+2];
			}
		}
	}
	return she;
}




//函数：
//功能：
//对图像进行水平方向的镜像变换
//参数：
//imgBuf：bmp图像的位图数据地址
unsigned char*  x_mirror(unsigned char *imgBuf)
{
	unsigned char* mir;
	unsigned char*p = imgBuf;
	int i, j;
	int LinePixel;
	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);

	//动态开辟空间存放处理后的数据
	mir = new unsigned char[LineByte*BmpHeight];
	//计算填充后每行的像素数
	LinePixel = LineByte / 3;

	//处理数据
	for (i = 0; i < BmpHeight; i++)
	{
		for (j = 0; j < LinePixel; j++)
		{
			//计算变换前后对应的坐标
			int loc = i * LineByte + j * 3;
			int mirloc = i * LineByte + (LinePixel - 1 - j) * 3;
			//对三个色彩空间分量分别赋值
			mir[mirloc] = p[loc];
			mir[mirloc + 1] = p[loc + 1];
			mir[mirloc + 2] = p[loc + 2];
		}
	}
	//返回处理后的位图数据起始指针
	return mir;
}

//函数：
//功能：
//对图像进行竖直方向的镜像变换
//参数：
//imgBuf：bmp图像的位图数据地址
unsigned char*  y_mirror(unsigned char *imgBuf)
{
	unsigned char* mir;
	unsigned char*p = imgBuf;
	int i, j;
	int k;
	int LinePixel;
	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);

	//动态开辟空间存放处理后的数据
	mir = new unsigned char[LineByte*BmpHeight];
	//计算填补后每行的像素数
	LinePixel = LineByte / 3;

	//处理数据
	for (i = 0; i < BmpHeight; i++)
	{
		for (j = 0; j < LinePixel; j++)
		{
			//计算变换前后对应的坐标值
			int loc = i * LineByte + j * 3;
			int mirloc = (BmpHeight - 1 - i) * LineByte + j * 3;
			//对三个色彩空间分量分别进行赋值
			mir[mirloc] = p[loc];
			mir[mirloc + 1] = p[loc + 1];
			mir[mirloc + 2] = p[loc + 2];
		}
	}
	//返回处理后的位图数据起始指针
	return mir;
}

//函数：
//功能：
//对图像进行平移变换（画布大小不变，超出部分剪切）
//参数：
//imgBuf：bmp图像的位图数据地址
unsigned char*  translation(unsigned char *imgBuf, int xtrans, int ytrans)
{
	unsigned char* trans;
	unsigned char*p = imgBuf;
	int i, j;
	int LinePixel;
	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);

	//动态开辟空间存放处理后的数据
	trans = new unsigned char[LineByte*BmpHeight];
	//计算填补后每行的像素数
	LinePixel = LineByte / 3;

	//全部进行初始赋值为零
	for (i = 0; i < LineByte*BmpHeight; i++)
	{
		trans[i] = 0;
	}

	//处理数据
	for (i = 0; i < BmpHeight; i++)
	{
		for (j = 0; j < LinePixel; j++)
		{
			//保证计算出的坐标在原图范围内
			if (((i + ytrans) < BmpHeight) && ((j + xtrans) < LinePixel) && ((i + ytrans) >= 0) && ((j + xtrans) >= 0))
			{
				//计算变换前后的坐标值及其所对应的数字下标
				int loc = i * LineByte + j * 3;
				int transloc = (i + ytrans)* LineByte + (j + xtrans) * 3;

				//对三个色彩空间分量分别赋值
				trans[transloc] = p[loc];
				trans[transloc + 1] = p[loc + 1];
				trans[transloc + 2] = p[loc + 2];
			}
		}
	}
	//返回处理后的位图数据起始指针
	return trans;
}

//函数：
//功能：
//对图像进行平移变换（画布大小根据平移改变，保证原图部分都被保留）
//参数：
//imgBuf：bmp图像的位图数据地址
unsigned char*  translation2(unsigned char *imgBuf, int xtrans, int ytrans)
{
	unsigned char* trans;
	unsigned char*p = imgBuf;
	int i, j;
	int LinePixel;
	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);
	int tranLine = WIDTHBYTES(BmpByte * (BmpWidth + abs(xtrans)));
	int tranHeight = BmpHeight + abs(ytrans);
	//动态开辟空间存放处理后的数据
	trans = new unsigned char[tranLine*tranHeight];
	//计算填补后的每行像素数
	LinePixel = LineByte / 3;

	//对位图数据数组全部初始赋值为零
	for (i = 0; i < tranLine*tranHeight; i++)
	{
		trans[i] = 0;
	}

	//处理数据
	for (i = 0; i < BmpHeight; i++)
	{
		for (j = 0; j < LinePixel; j++)
		{
			//计算变换前后的坐标值及其所对应的数组下标
			int loc = i * LineByte + j * 3;
			int transloc;
			//根据平移参数的正负的不同，坐标值及其对应的数组下标计算方法不同
			if ((ytrans >= 0) && (xtrans >= 0))
			{
				transloc = (i + ytrans)* (tranLine)+(j + xtrans) * 3;
			}
			else if (ytrans >= 0 && (xtrans < 0))
			{
				transloc = (i + ytrans)* (tranLine)+(j) * 3;
			}
			else if (ytrans < 0 && (xtrans >= 0))
			{
				transloc = (i)* (tranLine)+(j + xtrans) * 3;
			}
			else if (ytrans < 0 && (xtrans < 0))
			{
				transloc = (i)* (tranLine)+(j) * 3;
			}
			else { printf("Error"); }

			//对三个色彩空间的分量分别进行赋值
			trans[transloc] = p[loc];
			trans[transloc + 1] = p[loc + 1];
			trans[transloc + 2] = p[loc + 2];
		}
	}
	//返回处理后的位图数据起始指针
	return trans;
}

//函数：
//功能：
//对图像进行伸缩变换（不进行二维线性插值）
//参数：
//imgBuf：原始图像的位图数据起始指针
//xsca：横向伸缩比例参数
//ysca：纵向伸缩比例参数
unsigned char*  scale(unsigned char *imgBuf, double xsca, double ysca)
{
	unsigned char* sca;
	unsigned char*p = imgBuf;
	int i, j;

	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);
	//计算伸缩后每行像素数
	int ScaleWidth = floor(BmpWidth*xsca);
	//计算伸缩后存储每行位图数据所需的字节数
	int ScaleLine = WIDTHBYTES(BmpByte * ScaleWidth);
	//计算伸缩后每列像素数
	int ScaleHeight = floor(BmpHeight*ysca);
	//计算补齐后每行像素数
	int ScalePixel = ScaleLine / 3;

	//动态开辟空间存放处理后的数据
	sca = new unsigned char[ScaleLine*ScaleHeight];

	//处理数据
	for (i = 0; i < ScaleHeight; i++)
	{
		for (j = 0; j < ScalePixel; j++)
		{
			//计算伸缩前后的对应坐标以及其相对应的数字下标
			int scaloc = i * ScaleLine + j * 3;
			int loc = ((int)(i / ysca)) * LineByte + ((int)(j / xsca)) * 3;
			//对三个色彩空间的分量分别进行赋值
			sca[scaloc] = p[loc];
			sca[scaloc + 1] = p[loc + 1];
			sca[scaloc + 2] = p[loc + 2];
		}
	}
	//返回处理后的位图数据起始指针
	return sca;
}

//函数：
//功能：
//对图像进行伸缩变换（进行二维线性插值）
//参数：
//imgBuf：原始图像的位图数据起始指针
//xsca：横向伸缩比例参数
//ysca：纵向伸缩比例参数
unsigned char*  scale2(unsigned char *imgBuf, double xsca, double ysca)
{
	unsigned char* sca;
	unsigned char*p = imgBuf;
	int i, j;
	//原图中每行像素数
	int LinePixel;
	int a, b, c, d;
	double aa, cc;
	int result[3];

	//计算存储每行位图数据所需的字节数
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);
	//计算伸缩后每行像素数
	int ScaleWidth = floor(BmpWidth*xsca);
	//计算伸缩后存储每行位图数据所需的字节数
	int ScaleLine = WIDTHBYTES(BmpByte * ScaleWidth);
	//计算伸缩后每列像素数
	int ScaleHeight = floor(BmpHeight*ysca);
	//计算补齐后每行像素数
	int ScalePixel = ScaleLine / 3;

	int scaloc;
	double xloc;
	double yloc;
	int ii;
	int jj;

	//计算元素中每行像素数
	LinePixel = LineByte / 3;
	//动态开辟空间存放处理后的数据
	sca = new unsigned char[ScaleLine*ScaleHeight];


	//处理数据
	for (i = 0; i < ScaleHeight; i++)
	{
		for (j = 0; j < ScalePixel; j++)
		{
			//计算结果途中坐标对应的数组下标
			scaloc = i * ScaleLine + j * 3;
			//计算由果索引的原图中坐标（为浮点类型）
			xloc = i / ysca;
			yloc = j / xsca;
			ii = floor(i / ysca);
			jj = floor(j / xsca);


			int loc1 = ii * LineByte + jj * 3;
			int loc2 = ii * LineByte + (jj + 1) * 3;
			int loc3 = (ii + 1) * LineByte + jj * 3;
			int loc4 = (ii + 1) * LineByte + (jj + 1) * 3;
			//对每个像素点进行二位线性插值
			//边界值取临近值
			for (int k = 0; k < 3; k++)
			{
				a = p[loc1 + k];
				b = p[loc2 + k];
				c = p[loc3 + k];
				d = p[loc4 + k];
				aa = a * (jj + 1 - yloc) + b * (yloc - jj);
				cc = c * (jj + 1 - yloc) + d * (yloc - jj);
				//aa = a * (yloc - jj) + b * (jj + 1 - yloc);
				//cc = c * (yloc - jj) + d * (jj + 1 - yloc);

				result[k] = aa * (ii + 1 - xloc) + cc * (xloc - ii);
				if (ii >= BmpHeight - 1)result[k] = a;
			}
			//对结果图像中的该像素点三个色彩分量分别赋值
			sca[scaloc] = result[0];
			sca[scaloc + 1] = result[1];
			sca[scaloc + 2] = result[2];
		}
	}
	//返回处理后的位图数据起始指针
	return sca;
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
	for (int i = 0; i < LineByte * BmpHeight; i++) 
	{
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




