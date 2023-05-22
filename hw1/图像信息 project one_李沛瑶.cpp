#define _CRT_SECURE_NO_WARNINGS
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
unsigned char *BmpBitDate;//bmpͼ��λͼ���ݵ���ʼָ��


//��������
bool readBmp(char *address);
bool writemap(char *address, int BmpByte, int BmpWidth, int BmpHeight, RGBQUAD *RGBpalette, unsigned char *BmpBitDate);
int*  rgbtoyuv(unsigned char *imgBuf);
unsigned char*  yuvtorgb(int *imgBuf);
unsigned char*  yuvtogray(int *imgBuf);
int*  changey(int *imgBuf, int Ychange);


//������
int main()
{
	//���徭����ͬ�任���λͼ����ָ��
	int *bBmpBitDate;
	unsigned char *cBmpBitDate;
	unsigned char *dBmpBitDate;
	int *eBmpBitDate;
	unsigned char *fBmpBitDate;

	//YUVͼ����Y�����ı��ֵ
	int Ychange = -50;


	//ԭʼλͼ����ָ��
	char address[] = "sy.bmp";
	//����address1·����24λbmpͼ��
	readBmp(address);

	//�������ԭʼͼ��
	char address_test[] = "sytest.bmp";
	writemap(address_test, BmpByte, BmpWidth, BmpHeight, NULL, BmpBitDate);



	//�������rgbȫ��ɫͼ��ת��Ϊyuvͼ�����
	bBmpBitDate = rgbtoyuv(BmpBitDate);
	//char address3[] = "syyuv.bmp";
	//writemap(address3, BmpByte, BmpWidth, BmpHeight, NULL, bBmpBitDate);


	//��yuvͼ��ת��λrgbͼ�����
	cBmpBitDate = yuvtorgb(bBmpBitDate);
	char address4[] = "syrgb.bmp";
	writemap(address4, BmpByte, BmpWidth, BmpHeight, NULL, cBmpBitDate);


	//��yuvͼ��ת��Ϊ�Ҷ�ͼ
	dBmpBitDate = yuvtogray(bBmpBitDate);
	char address5[] = "sygray.bmp";
	writemap(address5, BmpByte, BmpWidth, BmpHeight, NULL, dBmpBitDate);


	//�ı�yuvͼ���е�Y������ֵ	
	eBmpBitDate = changey(bBmpBitDate, Ychange);
	//char address6[] = "sychange.bmp";
	//writemap(address6, BmpByte, BmpWidth, BmpHeight, NULL, eBmpBitDate);


	//���ı�Y����ֵ֮���yuvͼ��ת��Ϊrgbͼ�����
	fBmpBitDate = yuvtorgb(eBmpBitDate);
	char address7[] = "sychangep.bmp";
	writemap(address7, BmpByte, BmpWidth, BmpHeight, NULL, fBmpBitDate);
	//printf("yes");
	//system("pause");

	//��ʾĳ��ͼ��
	//system("sygray.bmp");
}




//������
//���ܣ�
//����һ��24λbmpͼ��
//������
//address��Ҫ�����bmpͼ��ĵ�ַ
bool readBmp(char *address)
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
	BmpBitDate = new unsigned char[LineByte*BmpHeight];
	//����λͼ����
	for (int i = 0; i < LineByte * BmpHeight; i++) {
		fread(&BmpBitDate[i], 1, 1, fp);
	}

	//�ر��ļ�
	fclose(fp);
	return 1;

}

//������
//���ܣ�
//д��һ��bmp�ļ�
//������
//address���ļ���ַ
//BmpByte��bmpͼ��λ�� 
//BmpWidth��bmpͼ����
//BmpHeight��bmpͼ��߶�
//RGBpalette����ɫ����ʼָ�룬�˴�Ϊ��
//BmpBitDate��λͼ������ʼָ��
bool writemap(char *address, int BmpByte, int BmpWidth, int BmpHeight, RGBQUAD *RGBpalette, unsigned char *BmpBitDate)
{
	//���ղ����еĵ�ַ�ԡ�������д���ķ�ʽ��һ��bmp�ļ�
	FILE *fp;
	int LineByte;
	if ((fp = fopen(address, "wb")) == NULL)return 0;


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
	for (int i = 0; i < LineByte * BmpHeight; i++) {
		fwrite(&BmpBitDate[i], 1, 1, fp);
	}

	//�ر��ļ�
	fclose(fp);
	return 1;
}

//������
//���ܣ�
//rgbתyuv
//������
//imgBuf����Ҫ�����λͼ������ʼָ��
int*  rgbtoyuv(unsigned char *imgBuf)
{
	//���崦����ָ��
	int* yuv;
	unsigned char*p = imgBuf;
	int i;
	int k;
	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);

	//��̬���ٿռ��Ŵ���������
	yuv = new int[LineByte*BmpHeight];

	//��������
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
		//�ж�YUVֵ�Ƿ�Խ��
		//YUV��ȡֵ��Χ��Y = 0~255, U = -122~+ 122, V = -157~+ 157
		if (yuv[i] > 255)yuv[i] = 255;
		if (yuv[i] < 0)yuv[i] = 0;
		if (yuv[i + 1] > 122)yuv[i + 1] = 122;
		if (yuv[i + 1] < -122)yuv[i + 1] = -122;
		if (yuv[i + 2] > 157)yuv[i + 2] = 157;
		if (yuv[i + 2] < -157)yuv[i + 2] = -157;
		*/
	}
	//���ش�����λͼ������ʼָ��
	return yuv;
}

//������
//���ܣ�
//yuvתrgb
//������
//imgBuf����Ҫ�����λͼ������ʼָ��
unsigned char*  yuvtorgb(int *imgBuf)
{
	//���崦����ָ��
	unsigned char*rgb;
	int*p = imgBuf;
	int i;
	int k;
	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);

	//��̬���ٿռ��Ŵ���������
	rgb = new unsigned char[LineByte*BmpHeight];

	//��������
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
		//�ж�RGBֵ�Ƿ�Խ��
		//RGB��ȡֵ��Χ��R = 0~255, G = 0~255, B = 0~255,
		if (rgb[i] > 255)rgb[i] = 255;
		if (rgb[i] < 0)rgb[i] = 0;
		if (rgb[i + 1] > 255)rgb[i + 1] = 255;
		if (rgb[i + 1] < 0)rgb[i + 1] = 0;
		if (rgb[i + 2] > 255)rgb[i + 2] = 255;
		if (rgb[i + 2] < 0)rgb[i + 2] = 0;
		*/
	}
	//���ش�����λͼ������ʼָ��
	return rgb;
}

//������
//���ܣ�
//yuvתgray
//������
//imgBuf����Ҫ�����λͼ������ʼָ��
unsigned char*  yuvtogray(int *imgBuf)
{
	//���崦����ָ��
	unsigned char*gray;
	int*p = imgBuf;
	int i;
	int k;
	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);


	//��̬���ٿռ��Ŵ���������
	gray = new unsigned char[LineByte*BmpHeight];

	//��������
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

		//�˴�����Ҫ�ж������Ƿ�Խ��
		//��Ϊ�����YUVͼ���ڴ���ʱ�Ѿ��ж�
	}
	//���ش�����λͼ������ʼָ��
	return gray;
}

//������
//���ܣ�
//�ı�yuv�е�Y����
//������
//imgBuf����Ҫ�����λͼ������ʼָ��
int*  changey(int *imgBuf,int Ychange)
{
	//���崦����ָ��
	int*change;
	int*p = imgBuf;
	int i;
	int k;
	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);


	//��̬���ٿռ��Ŵ���������
	change = new int[LineByte*BmpHeight];

	//��������
	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{
		 k= p[i] + Ychange;
		if (k > 255)k = 255;
		if (k < 0)k = 0;
		change[i] = k;
		change[i + 1] = p[i + 1];
		change[i + 2] = p[i + 2];

		//�жϴ����������Ƿ�Խ��
		if (change[i] > 255)change[i] = 255;
		if (change[i] < 0)change[i] = 0;
	}
	//���ش�����λͼ������ʼָ��
	return change;
}

