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

//��¼��ֵ�������е�ӳ���ϵ
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
unsigned char*  rgbtoyuv(unsigned char *imgBuf);
unsigned char*  yuvtorgb(unsigned char *imgBuf);
//������ɫ�ʿռ�ķ�������ֵ������
unsigned char*  equalize(unsigned char * imgBuf);
//ֻ��Y��������ֵ������
unsigned char*  equalize2(unsigned char * imgBuf,int choice);
//�õ�ӳ���ϵ
void  getequalize(unsigned char * imgBuf,int equaenergy);


//������
int main()
{
	//���徭����ͬ�任���λͼ����ָ��
	unsigned char *bBmpBitDate;
	unsigned char *cBmpBitDate;
	unsigned char *dBmpBitDate;
	unsigned char *eBmpBitDate;
	unsigned char *fBmpBitDate;
	unsigned char *gBmpBitDate;

	//ӳ�������ֵĵȼ���
	int equaenergy = 256;

	//ԭʼλͼ����ָ��
	char address[] = "pic5.bmp";
	//����address1·����24λbmpͼ��
	readBmp(address);

	//�������ԭʼͼ��
	char address_test[] = "test.bmp";
	writemap(address_test, BmpByte, BmpWidth, BmpHeight, NULL, BmpBitDate);

	//RGBתYUV
	bBmpBitDate = rgbtoyuv(BmpBitDate);

	//��ɫͼ��RGB����ֱ�Ӿ��⻯
	getequalize(BmpBitDate,equaenergy);
	cBmpBitDate = equalize(BmpBitDate);
	char address1[] = "equaRGB.bmp";
	writemap(address1, BmpByte, BmpWidth, BmpHeight, NULL, cBmpBitDate);

	//��ɫͼ��Y�������⻯
	getequalize(bBmpBitDate, equaenergy);
	dBmpBitDate = equalize2(bBmpBitDate,1);
	eBmpBitDate = yuvtorgb(dBmpBitDate);
	char address2[] = "equaY.bmp";
	writemap(address2, BmpByte, BmpWidth, BmpHeight, NULL, eBmpBitDate);

	//��ɫͼ��YUV�������⻯
	getequalize(bBmpBitDate, equaenergy);
	fBmpBitDate = equalize(bBmpBitDate);
	gBmpBitDate = yuvtorgb(fBmpBitDate);
	char address3[] = "equaYUV.bmp";
	writemap(address3, BmpByte, BmpWidth, BmpHeight, NULL, gBmpBitDate);
	
	system("pause");

}

//������
//���ܣ�
//�õ�ӳ���ϵ
//������
//imgBuf����Ҫ�����λͼ��ʼָ��
//equaenergy��ӳ�����ּ����ȼ�
void  getequalize(unsigned char * imgBuf,int equaenergy)
{
	unsigned char* getequa;
	unsigned char* p = imgBuf;
	double histor[256] = { 0,0 };
	double histog[256] = { 0,0 };
	double histob[256] = { 0,0 };

	//rgb������ǰֵ�Ƿ��Ѿ������ӳ��ֵ�Ŀ��Ʊ���
	int flagr=0;
	int flagg = 0;
	int flagb = 0;

	int i;
	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);


	//��̬���ٿռ��Ŵ���������
	getequa = new unsigned char[LineByte*BmpHeight];

	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{
		//ͳ������ɫ�ʿռ������ֱ��ͼ
		histor[*(p+i)]++;
		histog[*(p+i+1)]++;
		histob[*(p+i+2)]++;
	}

	//��ֱ��ͼ�ۼӼ���Sk��ֵ
	for (i = 1; i < 256; i++)
	{
		histor[i] += histor[i - 1];
		histog[i] += histog[i - 1];
		histob[i] += histob[i - 1];
	}

	//�õ�����ֵ���ۼ�ֵ
	for (i = 0; i < 256; i++)
	{
		histor[i]/= histob[255];
		histog[i]/= histob[255];
		histob[i]/= histob[255];
	}

	//����ӳ�����ȼ����ó�ÿһ��ӳ��ȼ���������
	double equacolor = 256 / equaenergy;

	//��������ɫ�ʿռ�ֱ��ͼ��ӳ��ֵ
	for (i = 0; i < 256; i++)
	{
		flagr = flagg = flagb = 0;
		for (int j = 0; j < equaenergy; j++)
		{
			//�����һ��ɫ�ʿռ��ӳ��ֵ
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
			//����ڶ���ɫ�ʿռ��ӳ��ֵ
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
			//���������ɫ�ʿռ��ӳ��ֵ
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

//������
//���ܣ�
//����ӳ���ϵ��ֱ��ͼӳ�䵽���⻯���ֱ��ͼ
//������
//imgBuf����Ҫ�����λͼ��ʼָ��
//choice���任�ڼ���ɫ�ʿռ�ķ���
unsigned char*  equalize(unsigned char * imgBuf)
{
	int r, g, b;
	unsigned char* equa;
	unsigned char* p = imgBuf;
	int i;

	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);

	//��̬���ٿռ��Ŵ���������
	equa = new unsigned char[LineByte*BmpHeight];

	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{
		//�õ�ӳ��ֵ
		r = funr[*(p + i)];
		g = fung[*(p + i + 1)];
		b = funb[*(p + i + 2)];
		
		//����ӳ��
		equa[i] = r;
		equa[i+1] = g;
		equa[i+2] = b;
	}
	return equa;

}

//������
//���ܣ�
//����ӳ���ϵ��ֱ��ͼӳ�䵽���⻯���ֱ��ͼ
//��ֻ�ı�һ��������
//������
//imgBuf����Ҫ�����λͼ��ʼָ��
unsigned char*  equalize2(unsigned char * imgBuf,int choice)
{
	int fun;
	unsigned char* equa;
	unsigned char* p = imgBuf;
	int i;

	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);


	//��̬���ٿռ��Ŵ���������
	equa = new unsigned char[LineByte*BmpHeight];

	//��������
	for (i = 0; i < LineByte*BmpHeight; i = i + 3)
	{
		//����ѡ��任�ķ���������ӳ��ֵ������ӳ��
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
unsigned char*  yuvtorgb(unsigned char *imgBuf)
{
	//���崦����ָ��
	unsigned char*rgb;
	unsigned char*p = imgBuf;
	int i;
	int k;
	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);

	//��̬���ٿռ��Ŵ���������
	rgb = new unsigned char[LineByte*BmpHeight];

	//��������
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
	//���ش�����λͼ������ʼָ��
	return rgb;
}


//������
//���ܣ�
//rgbתyuv
//������
//imgBuf����Ҫ�����λͼ������ʼָ��
unsigned char*  rgbtoyuv(unsigned char *imgBuf)
{
	//���崦����ָ��
	unsigned char* yuv;
	unsigned char*p = imgBuf;
	int i;
	int k;
	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);

	//��̬���ٿռ��Ŵ���������
	yuv = new unsigned char[LineByte*BmpHeight];

	//��������
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
	//���ش�����λͼ������ʼָ��
	return yuv;
}



