#define _CRT_SECURE_NO_WARNINGS
#define PI 3.1415926535
#include<iostream>
#include<string>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<windows.h>
using namespace std;

//�궨�壬���ڼ��㲻С��λͼ���������ֽ�����Ϊ4������������
#define WIDTHBYTES(i)    ((i+31)/32*4)

//ȫ�ֱ���

int BmpWidth;//bmpͼ��Ŀ�ȣ���λ���أ�
int BmpHeight;//bmpͼ��ĸ߶ȣ���λ���أ�
int BmpByte;//bmpͼ���λ��
unsigned char **BmpBitDate;//bmpͼ��λͼ���ݵ���ʼָ��


//��������
//bool readBmp(char *address);
//����һ��ͼƬ�����䴢���ڶ�ά������
bool readBmp2(char *address);
//bool writemap(char *address, int BmpByte, int BmpWidth, int BmpHeight, RGBQUAD *RGBpalette, unsigned char *BmpBitDate);
//����ά��������ݶ�����һ��ͼƬ��
bool writemap2(char *address, int BmpByte, int BmpWidth, int BmpHeight, RGBQUAD *RGBpalette, unsigned char **BmpBitDate);

//�任ɫ�ʿռ估ת�Ҷ�ͼ
unsigned char**  rgbtoyuv(unsigned char **imgBuf);
unsigned char**  yuvtorgb(unsigned char **imgBuf);
unsigned char**  yuvtogray(unsigned char **imgBuf);

int **getlaplacefilter(void);
int point_filtering(unsigned char **imgBuf, int loci, int locj, int **filter, int size, int total);
int **filtering(unsigned char **imgBuf, int **filter, int size, int total);
unsigned char **enhance(unsigned char **imgBuf, int **laplace);



//������
int main()
{	
	//���徭����ͬ�任���λͼ����ָ��
	unsigned char **bBmpBitDate;
	int **dBmpBitDate;
	unsigned char **eBmpBitDate;
	int **fBmpBitDate;
	unsigned char **hBmpBitDate;
	int **lBmpBitDate;
	unsigned char **mBmpBitDate;
	unsigned char **nBmpBitDate;

	unsigned char **tBmpBitDate;
	unsigned char **rBmpBitDate;
	unsigned char **gBmpBitDate;

	int **filter2;
	int filter2_size = 3;

	filter2=getlaplacefilter();

	//ԭʼλͼ����ָ��
	char address[] = "pic4.bmp";
	//����address1·����24λbmpͼ��
	readBmp2(address);

	//�������ԭʼͼ��
	char address_test2[] = "test2.bmp";
	writemap2(address_test2, BmpByte, BmpWidth, BmpHeight, NULL, BmpBitDate);

	//�������rgbתyuv�ٷ�ת��ͼ��
	tBmpBitDate = rgbtoyuv(BmpBitDate);
	rBmpBitDate = yuvtorgb(tBmpBitDate);
	char address_rgb[] = "rgbte.bmp";
	writemap2(address_rgb, BmpByte, BmpWidth, BmpHeight, NULL, rBmpBitDate);

	//yuvת�Ҷ�ͼ�����
	gBmpBitDate = yuvtogray(tBmpBitDate);
	char address_gray[] = "grayte.bmp";
	writemap2(address_gray, BmpByte, BmpWidth, BmpHeight, NULL, gBmpBitDate);

	//�ԻҶ�ͼ���л���������˹���Ӽ���ͼ���Ե�ĵ�ͼ����ǿ
	dBmpBitDate = filtering(gBmpBitDate, filter2, filter2_size, 1);
	eBmpBitDate = enhance(gBmpBitDate, dBmpBitDate);
	char address1[] = "Gray_Enhance.bmp";
	writemap2(address1, BmpByte, BmpWidth, BmpHeight, NULL, eBmpBitDate);

	//�Բ�ɫͼRGBɫ�ʿռ���л���������˹���Ӽ���ͼ���Ե�ĵ�ͼ����ǿ
	fBmpBitDate = filtering(BmpBitDate, filter2, filter2_size, 1);
	hBmpBitDate = enhance(BmpBitDate, fBmpBitDate);
	char address2[] = "RGB_Enhance.bmp";
	writemap2(address2, BmpByte, BmpWidth, BmpHeight, NULL, hBmpBitDate);

	//�Բ�ɫͼYUVɫ�ʿռ���л���������˹���Ӽ���ͼ���Ե�ĵ�ͼ����ǿ
	lBmpBitDate = filtering(tBmpBitDate, filter2, filter2_size, 1);
	mBmpBitDate = enhance(tBmpBitDate, lBmpBitDate);
	nBmpBitDate = yuvtorgb(mBmpBitDate);
	char address3[] = "YUV_Enhance.bmp";
	writemap2(address3, BmpByte, BmpWidth, BmpHeight, NULL, nBmpBitDate);

	system("pause");
}

//������
//���ܣ�
//yuvתrgb
//������
//imgBuf����Ҫ�����λͼ������ʼָ��
unsigned char**  yuvtorgb(unsigned char **imgBuf)
{
	//���崦����ָ��
	unsigned char**rgb;
	unsigned char**p = imgBuf;

	int k;

	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);
	int LinePixel = LineByte / 3;

	//��̬���ٿռ��Ŵ���������
	rgb = new unsigned char*[BmpHeight];
	for (int i = 0; i < BmpHeight; i++)
	{
		rgb[i] = new unsigned char[LineByte];
	}

	//��������
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

	//���ش�����λͼ������ʼָ��
	return rgb;
}


//������
//���ܣ�
//rgbתyuv
//������
//imgBuf����Ҫ�����λͼ������ʼָ��
unsigned char** rgbtoyuv(unsigned char** imgBuf)
{
	//���崦����ָ��
	unsigned char** yuv;
	unsigned char** p = imgBuf;

	int k;

	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);
	int LinePixel = LineByte / 3;

	//��̬���ٿռ��Ŵ���������
	yuv = new unsigned char*[BmpHeight];
	for (int i = 0; i < BmpHeight; i++)
	{
		yuv[i] = new unsigned char[LineByte];
	}
	//��������
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
	//���ش�����λͼ������ʼָ��
	return yuv;
}



//������
//���ܣ�
//yuvtogray
//������
//imgBuf����Ҫ�����λͼ������ʼָ��
unsigned char** yuvtogray(unsigned char** imgBuf)
{
	//���崦����ָ��
	unsigned char** gray;
	unsigned char** p = imgBuf;

	int k;
	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);

	//��̬���ٿռ��Ŵ���������

	int LinePixel = LineByte / 3;
	//��̬���ٿռ��Ŵ���������
	gray = new unsigned char*[BmpHeight];
	for (int i = 0; i < BmpHeight; i++)
	{
		gray[i] = new unsigned char[LineByte];
	}
	//��������
	for (int i = 0; i < BmpHeight; i++)
	{
		for (int j = 0; j < LinePixel; j++)
		{
			gray[i][j * 3] = p[i][j * 3];
			gray[i][j * 3 + 1] = gray[i][j * 3];
			gray[i][j * 3 + 2] = gray[i][j * 3];
		}
	}

	return gray;
}

//������
//���ܣ�
//�õ�һ��������˹�˲�������
//������
int **getlaplacefilter(void)
{
	int size = 3;
	//�˲���ͷָ��
	int **filter;

	//���ٴ���ռ�
	filter = new int*[size];
	for (int i = 0; i < size; i++)
	{
		filter[i] = new int[size];
	}

	//���˲������и�ֵ
	filter[0][0] = 0;
	filter[0][1] = -1;
	filter[0][2] = 0;
	filter[1][0] = -1;
	filter[1][1] = 4;
	filter[1][2] = -1;
	filter[2][0] = 0;
	filter[2][1] = -1;
	filter[2][2] = 0;

	return filter;
}



//������
//���ܣ�
//��ͼ������˲�
//������
//imgBuf��λͼ����ͷָ��
//filter���˲�������ͷָ��
//size���˲�����С������
//total��λͼ�������˲������ݶ�Ӧ���ȡ�ͺ�����ϵ��
int **filtering(unsigned char **imgBuf, int **filter, int size, int total)
{
	int** res;
	unsigned char**p = imgBuf;
	int i, j;
	//�õ��˲�������������������ĵ��ƫ�ƶ�
	int mid = (size - 1) / 2;

	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);
	int LinePixel = LineByte / 3;
	//��̬���ٿռ��Ŵ���������
	res = new int*[BmpHeight];
	for (i = 0; i < BmpHeight; i++)
	{
		res[i] = new int[LineByte];
	}
	//��������
	for (i = 0+mid; i < BmpHeight-mid; i++)
	{
		for (j = 0+mid; j < LinePixel-mid; j++)
		{
			for (int k = 0; k <3 ; k++)
			{			
				res[i][j*3+k]=point_filtering(p, i, j*3+k, filter, size, total);
				//res[i][j * 3 +1] = res[i][j * 3 + k];
				//res[i][j * 3 +2] = res[i][j * 3 + k];
			}
		}
	}
	return res;
}

//������
//���ܣ�
//��ͼ���еĵ������ص���м���
//������
//imgBuf��λͼ����ͷָ��
//filter���˲�������ͷָ��
//size���˲�����С������
//total��λͼ�������˲������ݶ�Ӧ���ȡ�ͺ�����ϵ��
//loci��locj����ǰ������λͼ�����е�����
int point_filtering(unsigned char **imgBuf, int loci, int locj, int **filter, int size, int total)
{
	//����������
	int sum = 0;
	int result;
	//�õ��˲�������������������ĵ��ƫ����
	int mid = (size - 1) / 2;
	//λͼ����ͷָ��
	unsigned char **p = imgBuf;

	//��������
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			sum += p[loci - mid + i][locj - mid * 3 + j * 3] * filter[i][j];
		}
	}
	result = sum / total;
	return result;
}

//������
//���ܣ�
//��������˹���Ӽ�⵽��ͼ���Ե��ԭͼ����е���
//������
//imgBuf��λͼ����ͷָ��
//laplace������ͼ���Ե��Ϣ������ͷָ��
unsigned char **enhance(unsigned char **imgBuf, int **laplace)
{
	//��������ͷָ��
	unsigned char** enh;
	unsigned char**p = imgBuf;
	int i, j;


	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);
	int LinePixel = LineByte / 3;
	//��̬���ٿռ��Ŵ���������
	enh = new unsigned char*[BmpHeight];
	for (i = 0; i < BmpHeight; i++)
	{
		enh[i] = new unsigned char[LineByte];
	}

	//��������
	for (i = 0; i < BmpHeight; i++)
	{
		for (j = 0; j < LinePixel; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				//����Խ�����ֵ
				int result = p[i][j * 3 + k] + laplace[i][j * 3 + k];
				if (result < 0)result = 0;
				if (result > 255)result = 255;
				enh[i][j * 3 + k] = result;
			}
		}
	}
	return enh;
}

//������
//���ܣ�
//����һ��24λbmpͼ�񣨲�������һ����ά�����У�
//������
//address��Ҫ�����bmpͼ��ĵ�ַ
bool readBmp2(char *address)
{
	//���պ��������ĵ�ַ���ļ�
	FILE *fp;
	if ((fp = fopen(address, "rb")) == NULL)
	{
		return 0;
	}
	//�����ļ���Ϣͷ
	//BITMAPFILEHEADER filehead;
	BITMAPINFOHEADER infohead;

	//�ƶ�ָ�뵽��Ϣͷ��ʼλ��
	fseek(fp, sizeof(BITMAPFILEHEADER), 0);
	//fread(&infohead, sizeof(BITMAPFILEHEADER), 1, fp);
	//�����ļ�����Ϣͷ
	fread(&infohead, sizeof(BITMAPINFOHEADER), 1, fp);

	//����Ϣͷ�е����ݸ�ֵ��ȫ�ֱ���
	BmpByte = infohead.biBitCount;
	BmpHeight = infohead.biHeight;
	BmpWidth = infohead.biWidth;


	//����洢λͼÿ������������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);

	//��̬�����ڴ���������λͼ����

	BmpBitDate = new unsigned char*[BmpHeight];
	for (int i = 0; i < BmpHeight; i++)
	{
		BmpBitDate[i] = new unsigned char[LineByte];
	}
	//BmpBitDate = new unsigned char[LineByte*BmpHeight];
	//����λͼ����
	for (int i = 0; i < BmpHeight; i++)
	{
		for (int j = 0; j < LineByte; j++)
		{
			fread(&BmpBitDate[i][j], 1, 1, fp);
		}

	}

	//�ر��ļ�
	fclose(fp);
	return 1;
}

//������
//���ܣ�
//д��һ��bmp�ļ�������һ����ά�����е����ݣ�
//������
//address���ļ���ַ
//BmpByte��bmpͼ��λ�� 
//BmpWidth��bmpͼ����
//BmpHeight��bmpͼ��߶�
//RGBpalette����ɫ����ʼָ�룬�˴�Ϊ��
//BmpBitDate��λͼ������ʼָ��
bool writemap2(char *address, int BmpByte, int BmpWidth, int BmpHeight, RGBQUAD *RGBpalette, unsigned char **BmpBitDate)
{
	//���ղ����еĵ�ַ�ԡ�������д���ķ�ʽ��һ��bmp�ļ�
	FILE *fp;
	int LineByte;
	if ((fp = fopen(address, "wb")) == NULL)return 0;

	int i, j;

	//�����ļ�ͷ����Ϣͷ
	BITMAPFILEHEADER filehead;
	BITMAPINFOHEADER infohead;
	//����λͼ����ÿ����Ҫ���ֽ���
	LineByte = WIDTHBYTES(BmpByte * BmpWidth);


	//Ϊ�ļ�ͷ��ֵ
	filehead.bfType = 0x4D42;
	filehead.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + LineByte * BmpHeight;
	filehead.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	//д���ļ�ͷ
	fwrite(&filehead, sizeof(BITMAPFILEHEADER), 1, fp);

	//Ϊ��Ϣͷ��ֵ
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

	//д����Ϣͷ
	fwrite(&infohead, sizeof(BITMAPINFOHEADER), 1, fp);

	//д��λͼ����
	for (i = 0; i < BmpHeight; i++)
	{
		for (j = 0; j < LineByte; j++)
		{
			fwrite(&BmpBitDate[i][j], 1, 1, fp);
		}
	}
	

	//�ر��ļ�
	fclose(fp);
	return 1;
}


