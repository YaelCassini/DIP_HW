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

int funr[256] = { 0 };
int fung[256] = { 0 };
int funb[256] = { 0 };


//ȫ�ֱ���

int BmpWidth;//bmpͼ��Ŀ�ȣ���λ���أ�
int BmpHeight;//bmpͼ��ĸ߶ȣ���λ���أ�
int BmpByte;//bmpͼ���λ��
unsigned char *BmpBitDate;//bmpͼ��λͼ���ݵ���ʼָ��


//��������
bool readBmp(char *address);
bool writemap(char *address, int BmpByte, int BmpWidth, int BmpHeight, RGBQUAD *RGBpalette, unsigned char *BmpBitDate);
int*  rgbtoyuv(unsigned char *imgBuf);
unsigned char*  yuvtogray(int *imgBuf, int *histogram);
unsigned char*  logarithm(unsigned char *imgBuf);//�Ҷ�ͼ��ͨ������
unsigned char*  logarithm1(unsigned char *imgBuf,int thres);//�Ҷ�ͼ����������
int*  logarithm2(int *imgBuf);//��ɫͼ��ͨ������
int*  logarithm3(int *imgBuf,int thres);// ��ɫͼ����������
unsigned char*  yuvtorgb(int *imgBuf);

int histogram2[256] = { 0 };
//������
int main()
{
	//���徭����ͬ�任���λͼ����ָ��
	int *bBmpBitDate;
	unsigned char *cBmpBitDate;
	unsigned char *dBmpBitDate;
	int *eBmpBitDate;
	unsigned char *fBmpBitDate;
	int *gBmpBitDate;
	unsigned char *hBmpBitDate;
	


	//�Ҷȷֲ�ֱ��ͼ
	int histogram[256] = { 0 };
	
	//��ֵ������ֵ
	int threshold;
	int equaenergy = 16;

	//ԭʼλͼ����ָ��
	char address[] = "pic.bmp";
	//����address1·����24λbmpͼ��
	readBmp(address);

	//�������ԭʼͼ��
	char address_test[] = "test.bmp";
	writemap(address_test, BmpByte, BmpWidth, BmpHeight, NULL, BmpBitDate);


	//�������rgbȫ��ɫͼ��ת��Ϊyuvͼ��
	bBmpBitDate = rgbtoyuv(BmpBitDate);


	//��yuvͼ��ת��Ϊ�Ҷ�ͼ�����
	cBmpBitDate = yuvtogray(bBmpBitDate,histogram);
	char address1[] = "gray.bmp";
	writemap(address1, BmpByte, BmpWidth, BmpHeight, NULL, cBmpBitDate);


	//�Ҷ�ͼ����ͨ�������
	dBmpBitDate = logarithm(cBmpBitDate);
	char address2[] = "loga�Ҷ���ͨ.bmp";
	writemap(address2, BmpByte, BmpWidth, BmpHeight, NULL, dBmpBitDate);

	//�Ҷ�ͼ�ĸ����������
	dBmpBitDate = logarithm1(cBmpBitDate,15);
	char address3[] = "loga�Ҷȸ���.bmp";
	writemap(address3, BmpByte, BmpWidth, BmpHeight, NULL, dBmpBitDate);

	//��ɫͼ����ͨ������
	eBmpBitDate = logarithm2(bBmpBitDate);
	fBmpBitDate = yuvtorgb(eBmpBitDate);
	char address4[] = "loga��ɫ��ͨ.bmp";
	writemap(address4, BmpByte, BmpWidth, BmpHeight, NULL, fBmpBitDate);


	//��ɫͼ�ĸ���������
	gBmpBitDate = logarithm3(bBmpBitDate,50);
	hBmpBitDate = yuvtorgb(gBmpBitDate);
	char address5[] = "loga��ɫ����.bmp";
	writemap(address5, BmpByte, BmpWidth, BmpHeight, NULL, hBmpBitDate);

	
	system("pause");

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
		k = ((double)*(p + i)) + 1.140*((double)*(p + i + 2));
		if (k > 255)k = 255;
		if (k < 0)k = 0;
		rgb[i] = k;

		k = ((double)*(p + i)) - 0.395*((double)*(p + i + 1)) - 0.581*((double)*(p + i + 2));
		if (k > 255)k = 255;
		if (k < 0)k = 0;
		rgb[i + 1] = k;

		k = ((double)*(p + i)) + 2.032*((double)*(p + i + 1));
		if (k > 255)k = 255;
		if (k < 0)k = 0;
		rgb[i + 2] = k;


	}
	//���ش�����λͼ������ʼָ��
	return rgb;
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
	//���ش�����λͼ������ʼָ��
	return yuv;
}



//������
//���ܣ�
//yuvתgray��ͳ�����ȷֲ�ֱ��ͼ
//������
//imgBuf����Ҫ�����λͼ������ʼָ��
//histogram������Ҷ�ֱ��ͼ������ͷָ��
unsigned char*  yuvtogray(int *imgBuf,int * histogram)
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

		k = *(p + i);
		if (k > 255)k = 255;
		if (k < 0)k = 0;
		gray[i] = k;
		//����Ҷ�ֱ��ͼ����
		histogram[k]++;

		gray[i + 1] = gray[i];
		gray[i + 2] = gray[i];

		//�˴�����Ҫ�ж������Ƿ�Խ��
		//��Ϊ�����YUVͼ���ڴ���ʱ�Ѿ��ж�
	}
	//���ش�����λͼ������ʼָ��
	return gray;
}

//������
//���ܣ�
//�Ҷ�ͼ������������
//������
//imgBuf����Ҫ�����λͼ������ʼָ��
//thres����ȥ������С�Ĺ����������ص����ֵ
unsigned char*  logarithm1(unsigned char *imgBuf,int thres)
{
	double* loga;
	unsigned char* result;
	unsigned char* p = imgBuf;

	//���Ʊ���
	int i;
	//���ֵ
	double max=0;
	//��Сֵ
	double min = 255;
	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);

	double kk;
	int maxlocii;
	int minlocii;
	int maxii=0;
	//int maxlocii;
	double cc;
	//int Maxvalue;
	int Minvalue;
	double cc2;
	double kk2;
	int histogram2[256] = { 0,0 };



	//��̬���ٿռ��Ŵ���������
	loga = new double[LineByte*BmpHeight];
	result = new unsigned char[LineByte*BmpHeight];

	//���ж�����
	for (i = 0; i < LineByte*BmpHeight; i=i+3)
	{
		loga[i] = log(1+(*(p + i)));	
		if (loga[i] >= max)
		{
			max = loga[i];
		}
		if ((loga[i] <= min))
		{
			min = loga[i];
		}

	}

	//�õ�ӳ�����
	cc = 255 / (max-min);
	kk = min * cc;

	//���������ӳ�䵽0-255
	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{	
		result[i] = ((cc*loga[i]) - kk);
		histogram2[result[i]]++;
		result[i + 1] = result[i];
		result[i + 2] = result[i];		
	}
	//�õ���ǰ����С�Ҷ�ֵ
	Minvalue = 0;
	for (i = 1; histogram2[i] <=thres; i++)
	{
        Minvalue = i;
	}
	
	//�õ��ڶ���ӳ�����
	cc2 = 255/(255 - double(Minvalue));
	kk2 = Minvalue * cc2;

	//���õ�����С�Ҷȵ����Ҷ�ӳ�䵽0-255
		for (i = 0; i < LineByte*BmpHeight; i = i + 3)
		{
			if (result[i] <= Minvalue) {
				result[i] = 0;
			}
			else {
				result[i] = result[i] * cc2 - kk2;
				
			}
			result[i + 1] = result[i];
			result[i + 2] = result[i];
		}

	return result;
}

//������
//���ܣ�
//�Ҷ�ͼ��������ͨ��
//������
//imgBuf����Ҫ�����λͼ������ʼָ��
unsigned char*  logarithm(unsigned char *imgBuf)
{
	double* loga;
	unsigned char* result;
	unsigned char* p = imgBuf;

	//���Ʊ���
	int i;
	//���ֵ
	double max = 0;
	//��Сֵ
	double min = 255;
	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);

	double kk;
	//int maxlocii;
	//int minlocii;
	//int maxii = 0;
	double cc;



	//��̬���ٿռ��Ŵ���������
	loga = new double[LineByte*BmpHeight];
	result = new unsigned char[LineByte*BmpHeight];

	//���ж�����
	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{
		loga[i] = log(1 + (*(p + i)));

		if (loga[i] >= max)
		{
			max = loga[i];
		}
		if ((loga[i] <= min))
		{
			min = loga[i];
		}

	}

	//�õ�ӳ�����
	cc = 255 / (max-min);
	kk = min * cc;
	//���������ӳ�䵽0-255
	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{
		result[i] = ((cc*loga[i])-kk);
		result[i + 1] = result[i];
		result[i + 2] = result[i];
	}

	return result;
}

//������
//���ܣ�
//��ɫͼ��������ͨ��
//������
//imgBuf����Ҫ�����λͼ������ʼָ��
int *  logarithm2(int *imgBuf)
{
	double* loga;
	int* result;
	int* p = imgBuf;
	int i;
	double max = 0;
	double min = 255;
	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);
	
	double cc;
	double kk;



	//��̬���ٿռ��Ŵ���������
	loga = new double[LineByte*BmpHeight];
	result = new int[LineByte*BmpHeight];


	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{
		loga[i] = log(1 + (*(p + i)));

		if (loga[i] >= max)
		{
			max = loga[i];
		}
		if (loga[i] <= min)
		{
			min = loga[i];
		}

	}

	cc = 255 / max;
	kk = min * cc;


	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{
		result[i] = (cc*loga[i]);
		result[i + 1] = *(p + i + 1);
		result[i + 2] = *(p + i + 2);
	}

	return result;
}

//������
//���ܣ�
//��ɫͼ������������
//������
//imgBuf����Ҫ�����λͼ������ʼָ��
//thres����ȥ������С�Ĺ����������ص����ֵ
int *  logarithm3(int *imgBuf,int thres)
{
	double* loga;
	int* result;
	int* p = imgBuf;
	int i;

	double max = 0;
	double min = 255;
	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);

	double cc;
	double kk;
	int Minvalue;
	double cc2;
	double kk2;
	int histogram2[256] = { 0,0 };


	//��̬���ٿռ��Ŵ���������
	loga = new double[LineByte*BmpHeight];
	result = new int[LineByte*BmpHeight];


	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{
		loga[i] = log(1 + (*(p + i)));
		if (loga[i] >= max)
		{
			max = loga[i];
		}
		if (loga[i] <= min)
		{
			min = loga[i];
		}

	}

	//cc = 255 / max;
	//result[i] = cc * loga[i];

	//�õ���һ��ӳ��ı���
	cc = 255 / (max - min);
	kk = min * cc;

    max = 255;
	min = 0;
	//���е�һ��ӳ��
	//ͳ��ӳ���ĻҶ�ͼ
	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{

		result[i] = ((cc*loga[i]) - kk);
		histogram2[result[i]]++;
		if (result[i] >= max)
		{
			max = result[i];
		}
		if (result[i] <= min)
		{
			min = result[i];
		}
		result[i + 1] = *(p + i + 1);
		result[i + 2] = *(p + i + 2);
	}

	//�õ�����������ȥһЩ�������ٵĻҶȺ����Сֵ
	Minvalue = 0;
	for (i = 1; histogram2[i] <= thres; i++)
	{
		Minvalue = i;
	}


	//�õ��ڶ���ӳ�����
	cc2 = 255 / double(255-Minvalue);
	kk2 = Minvalue * cc2;

	//���еڶ���ӳ��
	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{
		if (result[i] <= Minvalue) {
			result[i] = 0;
		}
		else {
			result[i] = result[i] * cc2 - kk2;
		}

	}

	return result;
}