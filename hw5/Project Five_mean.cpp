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

//��ʼ����ʱ�õ�һά����ת��ά����
//unsigned char** dimension(unsigned char *imgBuf);
//�õ�����Ϊsize�ľ�ֵ�˲���
unsigned char **getmeanfilter(int size);

//int **getleplacefilter(int size);
//�ó���Ϊsize���˲�����ͼ������˲�
unsigned char **filtering(unsigned char **imgBuf, unsigned char **filter, int size, int total);
//����ÿ�����˲����ֵ
int point_filtering(unsigned char **imgBuf, int loci, int locj, unsigned char **filter, int size, int total);


//������
int main()
{	
	//���徭���任���λͼ����ָ��
	unsigned char **bBmpBitDate;

	//��ֵ�˲���ͷָ��
	unsigned char **meanfilter;
	//��ֵ�˲�����С
	int meanfilter_size;


	//ԭʼλͼ����ָ��
	char address[] = "lena.bmp";
	//����address1·����24λbmpͼ��
	readBmp2(address);

	//�������ԭʼͼ�񣨸���Ϊ��ά����洢��
	char address_test2[] = "test2.bmp";
	writemap2(address_test2, BmpByte, BmpWidth, BmpHeight, NULL, BmpBitDate);

	//�ô�СΪ3�ľ�ֵ�˲�����ͼ������˲������
	meanfilter_size = 3;
	meanfilter=getmeanfilter(meanfilter_size);
	bBmpBitDate=filtering(BmpBitDate, meanfilter, meanfilter_size, meanfilter_size*meanfilter_size);
	char address1[] = "size3_mean.bmp";
	writemap2(address1, BmpByte, BmpWidth, BmpHeight, NULL, bBmpBitDate);

	//�ô�СΪ3�ľ�ֵ�˲�����ͼ������˲������
	meanfilter_size = 5;
	meanfilter = getmeanfilter(meanfilter_size);
	bBmpBitDate = filtering(BmpBitDate, meanfilter, meanfilter_size, meanfilter_size*meanfilter_size);
	char address2[] = "size5_mean.bmp";
	writemap2(address2, BmpByte, BmpWidth, BmpHeight, NULL, bBmpBitDate);

	//�ô�СΪ3�ľ�ֵ�˲�����ͼ������˲������
	meanfilter_size = 7;
	meanfilter = getmeanfilter(meanfilter_size);
	bBmpBitDate = filtering(BmpBitDate, meanfilter, meanfilter_size, meanfilter_size*meanfilter_size);
	char address3[] = "size7_mean.bmp";
	writemap2(address3, BmpByte, BmpWidth, BmpHeight, NULL, bBmpBitDate);
	

	system("pause");
}

//������
//���ܣ�
//��������һά�����е�λͼ����תΪ�����ڶ�ά������
//������
//imgBuf��λͼ����ͷָ��
/*
unsigned char** dimension(unsigned char *imgBuf)
{
	unsigned char** dim;
	unsigned char*p = imgBuf;
	int i, j;
	//int LinePixel;
	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);

	//��̬���ٿռ��Ŵ���������
	dim = new unsigned char*[BmpHeight];
	for (i = 0; i < BmpHeight; i++)
	{
		dim[i] = new unsigned char[LineByte];
	}

	//��������
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

//������
//���ܣ�
//�õ�һ����ֵ�˲���
//������
//size��Ҫ�õ��ľ�ֵ�˲����Ĵ�С������
unsigned char **getmeanfilter(int size)
{
	//�õ��˲�������������������ĵ��ƫ�ƶ�
	int mid = (size - 1) / 2;
	//�����˲�������ͷָ��
	unsigned char **filter;

	//���ٴ���ռ�
	filter = new unsigned char*[size];
	for (int i = 0; i < size; i++)
	{
		filter[i] = new unsigned char[size];
	}

	//�Ծ�ֵ�˲������и�ֵ
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			filter[i][j] = 1;
		}
	}
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
unsigned char **filtering(unsigned char **imgBuf, unsigned char **filter, int size, int total)
{
	unsigned char** res;
	unsigned char**p = imgBuf;
	int i, j;
	//�õ��˲�������������������ĵ��ƫ�ƶ�
	int mid = (size - 1) / 2;

	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);
	int LinePixel = LineByte / 3;

	//��̬���ٿռ��Ŵ���������
	res = new unsigned char*[BmpHeight];
	for (i = 0; i < BmpHeight; i++)
	{
		res[i] = new unsigned char[LineByte];
	}

	//��������
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

//������
//���ܣ�
//��ͼ���еĵ������ص���м���
//������
//imgBuf��λͼ����ͷָ��
//filter���˲�������ͷָ��
//size���˲�����С������
//total��λͼ�������˲������ݶ�Ӧ���ȡ�ͺ�����ϵ��
//loci��locj����ǰ������λͼ�����е�����
int point_filtering(unsigned char **imgBuf, int loci, int locj, unsigned char **filter, int size, int total)
{
	//����������
	int sum = 0;
	int result;
	//�õ��˲�������������������ĵ��ƫ�ƶ�
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
	result= fabs(sum/total);
	return result;
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

