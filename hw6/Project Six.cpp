#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<cmath>
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

unsigned char point_filtering(unsigned char **imgBuf, int loci, int locj,  int size);
unsigned char **one_filtering(unsigned char **imgBuf, int size);
unsigned char **three_filtering(unsigned char **imgBuf, int size);
unsigned char **enhance(unsigned char **imgBuf, int **laplace);

//������
int main()
{	
	//���徭����ͬ�任���λͼ����ָ��
	unsigned char **bBmpBitDate;
	unsigned char **dBmpBitDate;
	unsigned char **eBmpBitDate;
	unsigned char **fBmpBitDate;
	unsigned char **gBmpBitDate;
	unsigned char **hBmpBitDate;

	int filter_size = 11;
	//ԭʼλͼ����ָ��
	char address[] = "castle.bmp";
	//����address1·����24λbmpͼ��
	readBmp2(address);
	
	//��Yͨ������˫�߲�ֵ
	bBmpBitDate = rgbtoyuv(BmpBitDate);
	dBmpBitDate = one_filtering(bBmpBitDate,filter_size);
	eBmpBitDate = yuvtorgb(dBmpBitDate);

	//��RGB����ͨ������˫�߲�ֵ
	fBmpBitDate = three_filtering(BmpBitDate,filter_size);

	//��YUV����ͨ������˫�߲�ֵ
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

//������
//���ܣ�
//��ͼ������˲�
//������
//imgBuf��λͼ����ͷָ��
//size���˲�����С������
unsigned char **one_filtering(unsigned char **imgBuf, int size)
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

//������
//���ܣ�
//��ͼ������˲�
//������
//imgBuf��λͼ����ͷָ��
//size���˲�����С������
unsigned char **three_filtering(unsigned char **imgBuf, int size)
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


//������
//���ܣ�
//��ͼ���еĵ������ص���м���
//������
//imgBuf��λͼ����ͷָ��
//size���˲�����С������
//loci��locj����ǰ������λͼ�����е�����
unsigned char point_filtering(unsigned char **imgBuf, int loci, int locj, int size)
{
	//����������
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
	//�õ��˲�������������������ĵ��ƫ����
	int mid = (size - 1) / 2;
	//λͼ����ͷָ��
	unsigned char **p = imgBuf;
	int i, j;

	//��������
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


