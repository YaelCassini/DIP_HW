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
unsigned char *BmpBitDate;//bmpͼ��λͼ���ݵ���ʼָ��


//��������
bool readBmp(char *address);
bool writemap(char *address, int BmpByte, int BmpWidth, int BmpHeight, RGBQUAD *RGBpalette, unsigned char *BmpBitDate);

unsigned char*  x_mirror(unsigned char *imgBuf);
unsigned char*  y_mirror(unsigned char *imgBuf);
unsigned char*  translation(unsigned char *imgBuf, int xtrans, int ytrans);//�ü������ֻ�����С����
unsigned char*  translation2(unsigned char *imgBuf, int xtrans, int ytrans);//���ü�����֤ԭͼ���ݶ�������
unsigned char*  scale(unsigned char *imgBuf, double xsca, double ysca);//�����������ж�ά���Բ�ֵ
unsigned char*  scale2(unsigned char *imgBuf, double xsca, double ysca);//���������ж�ά���Բ�ֵ
unsigned char*  shear(unsigned char *imgBuf, double dx);
unsigned char*  rotation(unsigned char *imgBuf, double angle);
unsigned char*  interpolation(unsigned char *imgBuf, double angle);
unsigned char*  rotation2(unsigned char *imgBuf, double angle);

//������
int main()
{
	//ͼ��任����
	//ƽ�Ʊ任������
	//x����Ϊ������y����Ϊ������
	int xtrans = -50;
	int ytrans = 80;
	//�����任����
	//ͼ�����任Ϊԭ����xsca��������任Ϊԭ����ysca��
	double xsca = 5;
	double ysca = 5;
	//���У�shear���任����
	double dx = 1.2;
	double angle = 30 * PI / 180;

	//���徭����ͬ�任���λͼ����ָ��
	unsigned char *bBmpBitDate;
	unsigned char *cBmpBitDate;
	unsigned char *dBmpBitDate;
	unsigned char *eBmpBitDate;
	unsigned char *fBmpBitDate;
	unsigned char *gBmpBitDate;
	unsigned char *hBmpBitDate;
	unsigned char *iBmpBitDate;
	unsigned char *jBmpBitDate;

	//ԭʼλͼ����ָ��
	char address[] = "lena.bmp";
	//����address1·����24λbmpͼ��
	readBmp(address);

	//�������ԭʼͼ��
	char address_test[] = "test.bmp";
	writemap(address_test, BmpByte, BmpWidth, BmpHeight, NULL, BmpBitDate);

	
	//������任�����
	bBmpBitDate = x_mirror(BmpBitDate);
	char address1[] = "x_mirror.bmp";
	writemap(address1, BmpByte, BmpWidth, BmpHeight, NULL, bBmpBitDate);

	//������任�����
	cBmpBitDate = y_mirror(BmpBitDate);
	char address2[] = "y_mirror.bmp";
	writemap(address2, BmpByte, BmpWidth, BmpHeight, NULL, cBmpBitDate);

	//ƽ�Ʊ任���ü���������С����
	dBmpBitDate = translation(BmpBitDate, xtrans, ytrans);
	char address3[] = "trans.bmp";
	writemap(address3, BmpByte, BmpWidth, BmpHeight, NULL, dBmpBitDate);

	//ƽ�Ʊ任�����ü���������С����ƽ�����ı�
	eBmpBitDate = translation2(BmpBitDate,xtrans,ytrans);
	char address4[] = "trans2.bmp";
	writemap(address4, BmpByte, BmpWidth+abs(xtrans), BmpHeight+abs(ytrans), NULL, eBmpBitDate);


	//�����������ж�ά���Բ�ֵ
	fBmpBitDate = scale(BmpBitDate, xsca,ysca);
	char address5[] = "scale.bmp";
	writemap(address5, BmpByte, floor(BmpWidth*xsca), floor(BmpHeight*ysca), NULL, fBmpBitDate);
	

	//���������ж�ά���Բ�ֵ
	gBmpBitDate = scale2(BmpBitDate, xsca, ysca);
	char address6[] = "scale2.bmp";
	writemap(address6, BmpByte, floor(BmpWidth*xsca), floor(BmpHeight*ysca), NULL, gBmpBitDate);

	//б��ͼ�����
	gBmpBitDate = shear(BmpBitDate, dx);
	char address7[] = "shear.bmp";
	writemap(address7, BmpByte, floor(BmpWidth), floor(BmpHeight + floor(dx*BmpWidth)), NULL, gBmpBitDate);

	//��תͼ�����
	hBmpBitDate = rotation(BmpBitDate, angle);
	char address8[] = "rotation.bmp";
	writemap(address8, BmpByte, floor(BmpWidth*cos(angle) + BmpHeight * sin(angle)), floor(BmpWidth*sin(angle) + BmpHeight * cos(angle)), NULL, hBmpBitDate);

	//����תͼ����пմ���ֵ�����
	iBmpBitDate = interpolation(hBmpBitDate, angle);
	char address9[] = "inter.bmp";
	writemap(address9, BmpByte, floor(BmpWidth*cos(angle) + BmpHeight * sin(angle)), floor(BmpWidth*sin(angle) + BmpHeight * cos(angle)), NULL, iBmpBitDate);

	//��תͼ����ж�λ���Բ�ֵ�����
	jBmpBitDate = rotation2(BmpBitDate, angle);
	char address10[] = "rotation2.bmp";
	writemap(address10, BmpByte, floor(BmpWidth*cos(angle) + BmpHeight * sin(angle)), floor(BmpWidth*sin(angle) + BmpHeight * cos(angle)), NULL, jBmpBitDate);


    system("pause");
}

//������
//���ܣ�
//��ͼ�������ת(���ж�ά���Բ�ֵ)
//������
//imgBuf��ԭʼͼ���λͼ������ʼָ��
//angle����ת�ǶȲ���
unsigned char*  rotation2(unsigned char *imgBuf, double angle)
{
	unsigned char* rot;
	unsigned char*p = imgBuf;
	int i, j;

	//����洢ÿ��λͼ����������ֽ���
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

	//������ת���ĵı任
	double coss = cos(angle);
	double sinn = sin(angle);
	int rotorix = -orix * coss + oriy * sinn + orix;
	int rotoriy = -orix * sinn - oriy * coss + oriy;
	rotoriy = 0;

	rot = new unsigned char[RotLine*RotHeight];

	//����ʼֵȫ����Ϊ0
	for (i = 0; i < RotHeight; i++)
	{
		for (j = 0; j < RotLine; j++)
		{
			rot[i*RotLine + j] = 0;
		}
	}
	//return rot;

	//��������
	for (i = 0; i < RotHeight; i++)
	{
		for (j = 0; j < RotPixel; j++)
		{
			//����任ǰ���Ӧ������
			int rotloc = i * RotLine + j * 3;
			double locx = ((j-rotorix) * coss + (i-rotoriy) * sinn);
			double locy = (-(j - rotorix)* sinn + (i - rotoriy) * coss);
			int ii = floor(locy);
			int jj = floor(locx);

			//���ݲ�ͬ��ֵ���߽�ֵ��ͼ���ڲ�ֵ������ͼ���ڵ�ֵ���������ݽ��д���
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
			//�Խ��ͼ���и����ص�����������ֱ�ֵ
			rot[rotloc] = result[0];
			rot[rotloc + 1] = result[1];
			rot[rotloc + 2] = result[2];
		}
		//printf("%d#%d\n", i, j);
	}
	
	return rot;
}

//������
//���ܣ�
//��ͼ�������ת
//������
//imgBuf��ԭʼͼ���λͼ������ʼָ��
//angle����ת�ǶȲ���
unsigned char*  rotation(unsigned char *imgBuf, double angle)
{
	unsigned char* rot;
	unsigned char*p = imgBuf;
	int i, j;

	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);
	int RotWidth = floor(BmpWidth*cos(angle) + BmpHeight * sin(angle));
	int RotHeight = floor(BmpWidth*sin(angle) + BmpHeight * cos(angle));
	int RotLine = WIDTHBYTES(BmpByte * RotWidth);
	int RotPixel = RotLine/3;
	int LinePixel = LineByte / 3;
	int orix = 0;
	int oriy = BmpHeight-1;
	
	//������ת���ĵı任
	double coss = cos(angle);
	double sinn = sin(angle);
	int rotorix = -orix * coss + oriy * sinn + orix;
	int rotoriy = -orix * sinn - oriy * coss + oriy;
	rotoriy = 0;

	rot = new unsigned char[RotLine*RotHeight];

	//����ʼֵȫ����Ϊ0
	for (i = 0; i < RotHeight; i++)
	{
		for (j = 0; j < RotLine; j++)
		{
			rot[i*RotLine + j] = 0;
		}
	}

	//��������
	for (i = 0; i < BmpHeight; i++)
	{
		for (j = 0; j < LinePixel; j++)
		{
			//����任ǰ���Ӧ������
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


//������
//���ܣ�
//����ת���ͼ����в�ֵ
//������
//imgBuf��ԭʼͼ���λͼ������ʼָ��
//angle����ת�ǶȲ���
unsigned char*  interpolation(unsigned char *imgBuf, double angle)
{
	unsigned char* inter;
	unsigned char*p = imgBuf;
	int i, j;

	//����洢ÿ��λͼ����������ֽ���
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

	//����ʼֵȫ����Ϊ0
	inter = new unsigned char[RotLine*RotHeight];
	for (i = 0; i < RotHeight; i++)
	{
		for (j = 0; j < RotLine; j++)
		{
			inter[i*RotLine + j] = p[i*RotLine + j];
		}
	}
	//��������
	for (i = 1; i < RotHeight - 1; i++)
	{
		for (j = 1; j < RotPixel - 1; j++)
		{
			if (inter[i*RotLine + j * 3] == 0)
			{
				for (int k = 0; k < 3; k++)
				{
					//����ǰ�����Ұ˸����ص����ֵ
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
						//ȡƽ���Խ�����鸳ֵ
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

//������
//���ܣ�
//��ͼ�����б��
//������
//imgBuf��ԭʼͼ���λͼ������ʼָ��
//dx��б�б任����
unsigned char*  shear(unsigned char *imgBuf, double dx)
{
	unsigned char* she;
	unsigned char*p = imgBuf;
	int i, j;

	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);
	//����任��ÿ��������
	int ShearWidth = BmpWidth;
	//����任��ÿ��������
	int ShearHeight = floor(BmpHeight + dx * BmpWidth);
	int LinePixel = LineByte / 3;


	//��̬���ٿռ��Ŵ���������
	she = new unsigned char[LineByte*ShearHeight];

	//����ʼֵȫ����Ϊ0
	for (i = 0; i < ShearHeight; i++)
	{
		for (j = 0; j < LineByte; j++)
		{
			she[i * LineByte + j] = 0;
		}
	}


	//��������
	for (i = 0; i < ShearHeight; i++)
	{
		for (j = 0; j < LinePixel; j++)
		{
			//�ֱ����任ǰ������������Լ����Ӧ�������±�
			int sheloc = i * LineByte + j * 3;
			int ii = i - floor(dx*j);
			int loc = ii * LineByte + j * 3;
			//�����������ֱ���и�ֵ
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




//������
//���ܣ�
//��ͼ�����ˮƽ����ľ���任
//������
//imgBuf��bmpͼ���λͼ���ݵ�ַ
unsigned char*  x_mirror(unsigned char *imgBuf)
{
	unsigned char* mir;
	unsigned char*p = imgBuf;
	int i, j;
	int LinePixel;
	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);

	//��̬���ٿռ��Ŵ���������
	mir = new unsigned char[LineByte*BmpHeight];
	//��������ÿ�е�������
	LinePixel = LineByte / 3;

	//��������
	for (i = 0; i < BmpHeight; i++)
	{
		for (j = 0; j < LinePixel; j++)
		{
			//����任ǰ���Ӧ������
			int loc = i * LineByte + j * 3;
			int mirloc = i * LineByte + (LinePixel - 1 - j) * 3;
			//������ɫ�ʿռ�����ֱ�ֵ
			mir[mirloc] = p[loc];
			mir[mirloc + 1] = p[loc + 1];
			mir[mirloc + 2] = p[loc + 2];
		}
	}
	//���ش�����λͼ������ʼָ��
	return mir;
}

//������
//���ܣ�
//��ͼ�������ֱ����ľ���任
//������
//imgBuf��bmpͼ���λͼ���ݵ�ַ
unsigned char*  y_mirror(unsigned char *imgBuf)
{
	unsigned char* mir;
	unsigned char*p = imgBuf;
	int i, j;
	int k;
	int LinePixel;
	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);

	//��̬���ٿռ��Ŵ���������
	mir = new unsigned char[LineByte*BmpHeight];
	//�������ÿ�е�������
	LinePixel = LineByte / 3;

	//��������
	for (i = 0; i < BmpHeight; i++)
	{
		for (j = 0; j < LinePixel; j++)
		{
			//����任ǰ���Ӧ������ֵ
			int loc = i * LineByte + j * 3;
			int mirloc = (BmpHeight - 1 - i) * LineByte + j * 3;
			//������ɫ�ʿռ�����ֱ���и�ֵ
			mir[mirloc] = p[loc];
			mir[mirloc + 1] = p[loc + 1];
			mir[mirloc + 2] = p[loc + 2];
		}
	}
	//���ش�����λͼ������ʼָ��
	return mir;
}

//������
//���ܣ�
//��ͼ�����ƽ�Ʊ任��������С���䣬�������ּ��У�
//������
//imgBuf��bmpͼ���λͼ���ݵ�ַ
unsigned char*  translation(unsigned char *imgBuf, int xtrans, int ytrans)
{
	unsigned char* trans;
	unsigned char*p = imgBuf;
	int i, j;
	int LinePixel;
	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);

	//��̬���ٿռ��Ŵ���������
	trans = new unsigned char[LineByte*BmpHeight];
	//�������ÿ�е�������
	LinePixel = LineByte / 3;

	//ȫ�����г�ʼ��ֵΪ��
	for (i = 0; i < LineByte*BmpHeight; i++)
	{
		trans[i] = 0;
	}

	//��������
	for (i = 0; i < BmpHeight; i++)
	{
		for (j = 0; j < LinePixel; j++)
		{
			//��֤�������������ԭͼ��Χ��
			if (((i + ytrans) < BmpHeight) && ((j + xtrans) < LinePixel) && ((i + ytrans) >= 0) && ((j + xtrans) >= 0))
			{
				//����任ǰ�������ֵ��������Ӧ�������±�
				int loc = i * LineByte + j * 3;
				int transloc = (i + ytrans)* LineByte + (j + xtrans) * 3;

				//������ɫ�ʿռ�����ֱ�ֵ
				trans[transloc] = p[loc];
				trans[transloc + 1] = p[loc + 1];
				trans[transloc + 2] = p[loc + 2];
			}
		}
	}
	//���ش�����λͼ������ʼָ��
	return trans;
}

//������
//���ܣ�
//��ͼ�����ƽ�Ʊ任��������С����ƽ�Ƹı䣬��֤ԭͼ���ֶ���������
//������
//imgBuf��bmpͼ���λͼ���ݵ�ַ
unsigned char*  translation2(unsigned char *imgBuf, int xtrans, int ytrans)
{
	unsigned char* trans;
	unsigned char*p = imgBuf;
	int i, j;
	int LinePixel;
	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);
	int tranLine = WIDTHBYTES(BmpByte * (BmpWidth + abs(xtrans)));
	int tranHeight = BmpHeight + abs(ytrans);
	//��̬���ٿռ��Ŵ���������
	trans = new unsigned char[tranLine*tranHeight];
	//��������ÿ��������
	LinePixel = LineByte / 3;

	//��λͼ��������ȫ����ʼ��ֵΪ��
	for (i = 0; i < tranLine*tranHeight; i++)
	{
		trans[i] = 0;
	}

	//��������
	for (i = 0; i < BmpHeight; i++)
	{
		for (j = 0; j < LinePixel; j++)
		{
			//����任ǰ�������ֵ��������Ӧ�������±�
			int loc = i * LineByte + j * 3;
			int transloc;
			//����ƽ�Ʋ����������Ĳ�ͬ������ֵ�����Ӧ�������±���㷽����ͬ
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

			//������ɫ�ʿռ�ķ����ֱ���и�ֵ
			trans[transloc] = p[loc];
			trans[transloc + 1] = p[loc + 1];
			trans[transloc + 2] = p[loc + 2];
		}
	}
	//���ش�����λͼ������ʼָ��
	return trans;
}

//������
//���ܣ�
//��ͼ����������任�������ж�ά���Բ�ֵ��
//������
//imgBuf��ԭʼͼ���λͼ������ʼָ��
//xsca������������������
//ysca������������������
unsigned char*  scale(unsigned char *imgBuf, double xsca, double ysca)
{
	unsigned char* sca;
	unsigned char*p = imgBuf;
	int i, j;

	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);
	//����������ÿ��������
	int ScaleWidth = floor(BmpWidth*xsca);
	//����������洢ÿ��λͼ����������ֽ���
	int ScaleLine = WIDTHBYTES(BmpByte * ScaleWidth);
	//����������ÿ��������
	int ScaleHeight = floor(BmpHeight*ysca);
	//���㲹���ÿ��������
	int ScalePixel = ScaleLine / 3;

	//��̬���ٿռ��Ŵ���������
	sca = new unsigned char[ScaleLine*ScaleHeight];

	//��������
	for (i = 0; i < ScaleHeight; i++)
	{
		for (j = 0; j < ScalePixel; j++)
		{
			//��������ǰ��Ķ�Ӧ�����Լ������Ӧ�������±�
			int scaloc = i * ScaleLine + j * 3;
			int loc = ((int)(i / ysca)) * LineByte + ((int)(j / xsca)) * 3;
			//������ɫ�ʿռ�ķ����ֱ���и�ֵ
			sca[scaloc] = p[loc];
			sca[scaloc + 1] = p[loc + 1];
			sca[scaloc + 2] = p[loc + 2];
		}
	}
	//���ش�����λͼ������ʼָ��
	return sca;
}

//������
//���ܣ�
//��ͼ����������任�����ж�ά���Բ�ֵ��
//������
//imgBuf��ԭʼͼ���λͼ������ʼָ��
//xsca������������������
//ysca������������������
unsigned char*  scale2(unsigned char *imgBuf, double xsca, double ysca)
{
	unsigned char* sca;
	unsigned char*p = imgBuf;
	int i, j;
	//ԭͼ��ÿ��������
	int LinePixel;
	int a, b, c, d;
	double aa, cc;
	int result[3];

	//����洢ÿ��λͼ����������ֽ���
	int LineByte = WIDTHBYTES(BmpByte * BmpWidth);
	//����������ÿ��������
	int ScaleWidth = floor(BmpWidth*xsca);
	//����������洢ÿ��λͼ����������ֽ���
	int ScaleLine = WIDTHBYTES(BmpByte * ScaleWidth);
	//����������ÿ��������
	int ScaleHeight = floor(BmpHeight*ysca);
	//���㲹���ÿ��������
	int ScalePixel = ScaleLine / 3;

	int scaloc;
	double xloc;
	double yloc;
	int ii;
	int jj;

	//����Ԫ����ÿ��������
	LinePixel = LineByte / 3;
	//��̬���ٿռ��Ŵ���������
	sca = new unsigned char[ScaleLine*ScaleHeight];


	//��������
	for (i = 0; i < ScaleHeight; i++)
	{
		for (j = 0; j < ScalePixel; j++)
		{
			//������;�������Ӧ�������±�
			scaloc = i * ScaleLine + j * 3;
			//�����ɹ�������ԭͼ�����꣨Ϊ�������ͣ�
			xloc = i / ysca;
			yloc = j / xsca;
			ii = floor(i / ysca);
			jj = floor(j / xsca);


			int loc1 = ii * LineByte + jj * 3;
			int loc2 = ii * LineByte + (jj + 1) * 3;
			int loc3 = (ii + 1) * LineByte + jj * 3;
			int loc4 = (ii + 1) * LineByte + (jj + 1) * 3;
			//��ÿ�����ص���ж�λ���Բ�ֵ
			//�߽�ֵȡ�ٽ�ֵ
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
			//�Խ��ͼ���еĸ����ص�����ɫ�ʷ����ֱ�ֵ
			sca[scaloc] = result[0];
			sca[scaloc + 1] = result[1];
			sca[scaloc + 2] = result[2];
		}
	}
	//���ش�����λͼ������ʼָ��
	return sca;
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
	for (int i = 0; i < LineByte * BmpHeight; i++) 
	{
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




