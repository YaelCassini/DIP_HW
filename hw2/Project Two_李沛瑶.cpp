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
unsigned char*  yuvtogray(int *imgBuf, int *histogram);
int OTSU(int * histogram);
unsigned char*  graytobinary(unsigned char *imgBuf,int threshold);
unsigned char*  erosion(unsigned char *imgBuf);
unsigned char*  dilation(unsigned char *imgBuf);
unsigned char*  opening(unsigned char *imgBuf);
unsigned char*  closing(unsigned char *imgBuf);


//������
int main()
{
	//���徭����ͬ�任���λͼ����ָ��
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

	//�Ҷȷֲ�ֱ��ͼ
	int histogram[256] = { 0 };
	//��ֵ������ֵ
	int threshold;


	//ԭʼλͼ����ָ��
	char address[] = "lenacolor.bmp";
	//����address1·����24λbmpͼ��
	readBmp(address);

	//�������ԭʼͼ��
	char address_test[] = "test.bmp";
	writemap(address_test, BmpByte, BmpWidth, BmpHeight, NULL, BmpBitDate);


	//�������rgbȫ��ɫͼ��ת��Ϊyuvͼ��
	bBmpBitDate = rgbtoyuv(BmpBitDate);


	//��yuvͼ��ת��Ϊ�Ҷ�ͼ�����
	cBmpBitDate = yuvtogray(bBmpBitDate,histogram);
	char address2[] = "gray.bmp";
	writemap(address2, BmpByte, BmpWidth, BmpHeight, NULL, cBmpBitDate);

	//ʹ�ô���㷨������ֵ
	threshold = OTSU(histogram);

	//���Ҷ�ͼתΪ��ֵͼ�����
	dBmpBitDate = graytobinary(cBmpBitDate,threshold);
	char address3[] = "binary.bmp";
	writemap(address3, BmpByte, BmpWidth, BmpHeight, NULL, dBmpBitDate);

	//erosion����
	fBmpBitDate = erosion(dBmpBitDate);
	char address4[] = "erosion.bmp";
	writemap(address4, BmpByte, BmpWidth, BmpHeight, NULL, fBmpBitDate);

	//dilation����
	gBmpBitDate = dilation(dBmpBitDate);
	char address5[] = "dilation.bmp";
	writemap(address5, BmpByte, BmpWidth, BmpHeight, NULL, gBmpBitDate);

	//opening����
	hBmpBitDate = opening(dBmpBitDate);
	char address6[] = "opening.bmp";
	writemap(address6, BmpByte, BmpWidth, BmpHeight, NULL, hBmpBitDate);

	//closing����
	iBmpBitDate = closing(dBmpBitDate);
	char address7[] = "closing.bmp";
	writemap(address7, BmpByte, BmpWidth, BmpHeight, NULL, iBmpBitDate);

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
//yuvתgray
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
//���ܣ�ʹ�ô���㷨������ֵ
//������
//threshold���Ҷȷֲ�ֱ��ͼ��ʼָ��
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

	//�õ���С�Ҷ�ֵ
	for (i = 0; histogram[i] == 0; i++);
	Minvalue = i;
	//�õ����Ҷ�ֵ
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
		//ǰ��ͼ�����ص�
		ForePixel += histogram[i];
		//��ͼ�����ص�
		BackPixel -= histogram[i];
		//ǰ��ͼ��Ҷ���ֵ
		ForeGray += histogram[i] * i;
		//��ͼ��Ҷ���ֵ
		BackGray -= histogram[i] * i;

		//ǰ��ͼ�����ص�ٷֱ�
		ForeProb = (double)ForePixel / pixel;
		//��ͼ�����ص�ٷֱ�
		BackProb = (double)BackPixel / pixel;
		//ǰ��ͼ��Ҷ���ֵ��ռ�ٷֱ�
		ForeAver = (double)ForeGray / ForePixel;
		//��ͼ��Ҷ���ֵ��ռ�ٷֱ�
		BackAver = (double)BackGray / BackPixel;

		//����
		newvariance = ForeProb * BackProb*(ForeAver - BackAver)*(ForeAver - BackAver);
		//��һ���㷨������֤�ã�
		//anovariance = ForeProb * (totalaver - ForeAver)*(totalaver - ForeAver) + BackProb * (totalaver - BackAver)*(totalaver - BackAver);
		if (newvariance > variance)
		{
			variance = newvariance;
			threshold = i;
		}

	}
	return threshold;
}

//������
//���ܣ����Ҷ�ͼ��ֵ��
//������
//imgBuf����Ҫ�����λͼ������ʼָ��
//threshold���Ҷȷֲ�ֱ��ͼ��ʼָ��
unsigned char*  graytobinary(unsigned char *imgBuf, int threshold)
{
	unsigned char* binary;
	unsigned char* p = imgBuf;
	int i;
	int k;
	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);


	//��̬���ٿռ��Ŵ���������
	binary = new unsigned char[LineByte*BmpHeight];

	//��������
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


//������
//���ܣ��ñ�׼ʮ�ֿ�Զ�ֵ��ͼ�����dilation�����ͣ�����
//������
//imgBuf����Ҫ�����λͼ������ʼָ��
unsigned char*  dilation(unsigned char *imgBuf)
{
	unsigned char* dila;
	unsigned char* p = imgBuf;
	int i, j;
	int k;
	int m, n;
	int u;
	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);


	//��̬���ٿռ��Ŵ���������
	dila = new unsigned char[LineByte*BmpHeight];
	

	//��������
	for (i = 0; i < BmpHeight; i = i + 1)
	{
		for (j = 0; j  < LineByte; j = j + 3)
		{
            //���������±�
			u = i * LineByte + j;
			//�����ϱ�Ե���±�Ե
			if ((i == 0) || (i == BmpHeight-1))
			{
				dila[u] = *(p + u);
			}
			//�������Ե���ұ�Ե
			else if((j==0)||(j>= (BmpByte * BmpWidth)/8 - 3)){
				dila[u] = *(p + u);
			}
			//�����м䲿������
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
			//��UV������ֵ
			dila[u + 1] = dila[u];
			dila[u + 2] = dila[u];

		}
	}

	return dila;

}

//������
//���ܣ��ñ�׼ʮ�ֿ�Զ�ֵ��ͼ�����erosion����ʴ������
//������
//imgBuf����Ҫ�����λͼ������ʼָ��
unsigned char*  erosion(unsigned char *imgBuf)
{
	unsigned char* eros;
	unsigned char* p = imgBuf;
	int i, j;
	int k;
	int m, n;
	int u;
	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);


	//��̬���ٿռ��Ŵ���������
	eros = new unsigned char[LineByte*BmpHeight];

	

	//��������
	for (i = 0; i < BmpHeight ; i = i + 1)
	{
		for (j = 0; j  < LineByte; j = j + 3)
		{
			//���������±�
			u = i * LineByte + j;

			//�����ϱ�Ե���±�Ե
			if ((i == 0) || (i == BmpHeight-1))
			{
				eros[u] = *(p + u);
			}
			//�������Ե���ұ�Ե
			else if ((j == 0) || (j >= (BmpByte * BmpWidth)/8 - 3)) {
				eros[u] = *(p + u);
			}
			//�����м䲿������
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
			//��UV������ֵ
			eros[u + 1] = eros[u];
			eros[u + 2] = eros[u];
		}
	}

	return eros;

}

//������
//���ܣ��ñ�׼ʮ�ֿ�Զ�ֵ��ͼ�����opening����������
//������
//imgBuf����Ҫ�����λͼ������ʼָ��
unsigned char*  opening(unsigned char *imgBuf)
{
	unsigned char * p;
	p = erosion(imgBuf);
	p = dilation(p);
	return p;
}

//������
//���ܣ��ñ�׼ʮ�ֿ�Զ�ֵ��ͼ�����closing���գ�����
//������
//imgBuf����Ҫ�����λͼ������ʼָ��
unsigned char*  closing(unsigned char *imgBuf)
{
	unsigned char * p;
	p = dilation(imgBuf);
	p = erosion(p);
	return p;
}