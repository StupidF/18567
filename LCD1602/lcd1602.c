#include "lcd1602.h"

#define Busy    0x80 //���ڼ��LCM״̬���е�Busy��ʶ

//1602Һ������
#define LCM_Data  P2
sbit LCM_RW = P0^6; //����LCD����
sbit LCM_RS = P0^7;
sbit LCM_E  = P0^5;


unsigned char code ASCII[15] =    {'0','1','2','3','4','5','6','7','8','9','.','-','M'};

//��״̬
unsigned char ReadStatusLCM(void)
{
	LCM_Data = 0xFF; 
	LCM_RS = 0;
	LCM_RW = 1;
	LCM_E = 0;
	LCM_E = 0;
	LCM_E = 1;
	while (LCM_Data & Busy); //���æ�ź�
	return(LCM_Data);
}
//д����
void WriteDataLCM(unsigned char WDLCM) 
{
	ReadStatusLCM(); //���æ
	LCM_Data = WDLCM;
	LCM_RS = 1;
	LCM_RW = 0;
	LCM_E = 0; //�������ٶ�̫�߿���������С����ʱ
	LCM_E = 0; //��ʱ
	LCM_E = 1;
}

//дָ��
void WriteCommandLCM(unsigned char WCLCM,BuysC) //BuysCΪ0ʱ����æ���
{
	if (BuysC) ReadStatusLCM(); //������Ҫ���æ
	LCM_Data = WCLCM;
	LCM_RS = 0;
	LCM_RW = 0;	
	LCM_E = 0;
	LCM_E = 0;
	LCM_E = 1;	
}

//������
unsigned char ReadDataLCM(void)
{
	LCM_RS = 1; 
	LCM_RW = 1;
	LCM_E = 0;
	LCM_E = 0;
	LCM_E = 1;
	return(LCM_Data);
}



void LCMInit(void) //LCM��ʼ��
{
	LCM_Data = 0;
	WriteCommandLCM(0x38,0); //������ʾģʽ���ã������æ�ź�
	Delay5Ms(); 
	WriteCommandLCM(0x38,0);
	Delay5Ms(); 
	WriteCommandLCM(0x38,0);
	Delay5Ms(); 

	WriteCommandLCM(0x38,1); //��ʾģʽ����,��ʼҪ��ÿ�μ��æ�ź�
	WriteCommandLCM(0x0c,1); //�ر���ʾ
	WriteCommandLCM(0x01,1); //��ʾ����
	WriteCommandLCM(0x06,0); // ��ʾ����ƶ�����
}

//��ָ��λ����ʾһ���ַ�
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData)
{
	Y &= 0x1;
	X &= 0xF; //����X���ܴ���15��Y���ܴ���1
	if (Y) X |= 0x40; //��Ҫ��ʾ�ڶ���ʱ��ַ��+0x40;
	X |= 0x80; //���ָ����
	WriteCommandLCM(X, 1); //��������
	WriteDataLCM(DData); //������
}

//��ָ��λ����ʾһ���ַ�
void DisplayListChar(unsigned char X, unsigned char Y, unsigned char code *DData)
{
	unsigned char ListLength;

	ListLength = 0;
	Y &= 0x1;
	X &= 0xF; //����X���ܴ���15��Y���ܴ���1
	while (DData[ListLength] > 0x19) //�������ִ�β���˳�
	{
		if (X <= 0xF) //X����ӦС��0xF
		{
			DisplayOneChar( X, Y, DData[ListLength]); //��ʾ�����ַ�
			ListLength++;
			X++;
		}
	}
}

