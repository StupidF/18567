#include <REG52.H>		//���������ļ�
#include "ds1302.h"
#include <intrins.h>
#include <math.h> 
sfr P4 = 0XC0;

#define Key_Data P2_0 //����Keyboard����
#define Key_CLK  P3_2

#define Busy    0x80 //���ڼ��LCM״̬���е�Busy��ʶ
//ιʳ��־
unsigned char WeiYuFlag = 0;
unsigned char WeiYuTimeFlag = 0;
//��ˮ��־
unsigned char huanshuiFlag = 0;

//1602Һ������
#define LCM_Data  P2
sbit LCM_RW = P0^6; //����LCD����
sbit LCM_RS = P0^7;
sbit LCM_E  = P0^5;

//��������ź�
sbit K1 = P0^0;		//��ŷ�1
sbit K2 = P0^1;		//��ŷ�2
sbit K3 = P0^2;		//��ŷ�3
sbit K4 = P0^3;		//��ŷ�4
sbit K5 = P0^4;		//ˮ��

//����
sbit BT1 = P3^4;
sbit BT2 = P3^5;
sbit BT3 = P3^6;

//����������
sbit  RX =  P1^0;
sbit  TX  = P1^1;

//��������ź�
sbit X01 = P1^5;
sbit X02 = P1^6;
sbit X03 = P1^7;
//LED �ƹ�
sbit LED = P3^7;
//������
sbit BEEP = P3^3;
//ιʳ
sbit WS = P3^5;

//������ʱ��
unsigned int  LEDBeginTime= 0;

unsigned char Manual = 0;

unsigned char month, day,hour,minite;
unsigned char oldhour=0,oldminite=0;
unsigned char ReadDataLCM(void);
unsigned char ReadStatusLCM(void);
unsigned char code ASCII[15] =    {'0','1','2','3','4','5','6','7','8','9','.','-','M'};

static unsigned char DisNum = 0; //��ʾ��ָ��				  
       unsigned int  time=0;
	   unsigned long S=0;
	   unsigned long oldS=0;
	   unsigned long oldST=0;
	   bit      flag =0;
	   unsigned char disbuff[4]	   ={ 0,0,0,0,};

extern unsigned char dis_time_buf[16];	   
	   //5ms��ʱ
void Delay5Ms(void)
{
	unsigned int TempCyc = 5552;
	while(TempCyc--);
}

//400ms��ʱ
void Delay400Ms(void)
{
	unsigned char TempCycA = 5;
	unsigned int TempCycB;
	while(TempCycA--)
	{
		TempCycB=7269;
		while(TempCycB--);
	};
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
	while (DData[ListLength]>0x19) //�������ִ�β���˳�
	{
		if (X <= 0xF) //X����ӦС��0xF
		{
			DisplayOneChar(X, Y, DData[ListLength]); //��ʾ�����ַ�
			ListLength++;
			X++;
		}
	}
}


/********************************************************/
void Conut(void)
{
	time=TH0*256+TL0;
	TH0=0;
	TL0=0;

	S=(time*1.7)/100;     //�������CM
	if((S>=700)||flag==1) //����������Χ��ʾ��-��
	{	 
		flag=0;

		DisplayOneChar(0, 1, ASCII[11]);
		DisplayOneChar(1, 1, ASCII[11]);
		DisplayOneChar(2, 1, ASCII[11]);
	}
	else
	{
		disbuff[0]=S%1000/100;
		disbuff[1]=S%1000%100/10;
		disbuff[2]=S%1000%10 %10;
		DisplayOneChar(0, 1, ASCII[disbuff[0]]);
		DisplayOneChar(1, 1, ASCII[disbuff[1]]);
		DisplayOneChar(2, 1, ASCII[disbuff[2]]);
	}
}

/********************************************************/
void  StartModule() 		         //����ģ��
{
	  TX=1;			                     //����һ��ģ��
	  _nop_();_nop_();_nop_(); _nop_(); 
	  _nop_(); 
	  _nop_(); 
	  _nop_(); 
	  _nop_(); 
	  _nop_(); 
	  _nop_(); 
	  _nop_(); 
	  _nop_(); 
	  _nop_(); 
	  _nop_(); 
	  _nop_(); 
	  _nop_(); 
	  _nop_();
	  _nop_(); 
	  _nop_(); 
	  _nop_(); 
	  _nop_();
	  _nop_(); 
	  _nop_(); 
	  _nop_(); 
	  _nop_(); 
	  _nop_(); 
	  _nop_(); 
	  _nop_(); 
	  _nop_(); 
	  _nop_(); 
	  _nop_(); 
	  _nop_(); 
	  _nop_(); 
	  _nop_(); 
	  _nop_(); 
	  _nop_(); 
	  _nop_(); 
	  _nop_();
	  _nop_(); 
	  _nop_(); 
	  _nop_(); 
	  _nop_();
	  TX=0;
  }
/********************************************************/ 
void delayms(unsigned int ms)
{
	unsigned char i=100,j;
	for(;ms;ms--)
	{
		while(--i)
		{
			j=10;
			while(--j);
		}
	}
}

//��ʱ����ʼ��
void TimerInit(void)
{
	TMOD=0x01;		   //��T0Ϊ��ʽ1��GATE=1��
	TH0=0;
	TL0=0;          
	ET0=1;             //����T0�ж�
	EA=1;			   //�������ж�	
}


void displayTimes()
{
	ds1302_read_time();	//��ȡʱ��
	month =  dis_time_buf[4]*10+dis_time_buf[5];
	day = dis_time_buf[6]*10+dis_time_buf[7];
	hour = dis_time_buf[8]*10+dis_time_buf[9];
	minite = dis_time_buf[10]*10+dis_time_buf[11];
	
	DisplayOneChar(1, 0, month%100/10+'0');
	DisplayOneChar(2, 0, month%10+'0');//ʱ
	
	DisplayOneChar(4, 0, day%100/10+'0');
	DisplayOneChar(5, 0, day%10+'0');//ʱ
	
	DisplayOneChar(7, 0, hour%100/10+'0');
	DisplayOneChar(8, 0, hour%10+'0');//��
	DisplayOneChar(9, 0, ':');
	DisplayOneChar(10, 0, minite%100/10+'0');
	DisplayOneChar(11, 0, minite%10+'0');//��
	DisplayOneChar(12, 0, ':');
	DisplayOneChar(13, 0, dis_time_buf[12]+'0');
	DisplayOneChar(14, 0, dis_time_buf[13]+'0');//��
	
}
/*********************************************************/
void main(void)
{
	unsigned char TempCyc;
	unsigned char Step=0;
	unsigned int  Timetick=0;
	unsigned int  WTimetick=0;
	Delay400Ms(); 	//�����ȴ�����LCM���빤��״̬
	LCMInit(); 		//LCM��ʼ��
	Delay5Ms(); 	//��ʱƬ��(�ɲ�Ҫ)
	ReadDataLCM();	//�����þ�������
	for (TempCyc=0; TempCyc<10; TempCyc++)
		Delay400Ms(); 	//��ʱ
	TimerInit();	//��ʱ����ʼ��
	Init_timer2();
	ds1302_write_time(); //д���ʼֵ
	K1 = 1;
	K2 = 1;
	K3 = 1;
	K4 = 1;
	K5 = 1;
	LED = 1;
	BEEP = 1;
	
	StartModule();		//��������
	while(!RX);			//��RXΪ��ʱ�ȴ�
	TR0=1;			    //��������
	while(RX);			//��RXΪ1�������ȴ�
	TR0=0;				//�رռ���
	Conut();			//����
	oldST = S;
	
	while(1)
	{
		displayTimes();		//��ʾʱ��
		//����Һλ
		StartModule();		//��������
		while(!RX);			//��RXΪ��ʱ�ȴ�
		TR0=1;			    //��������
		while(RX);			//��RXΪ1�������ȴ�
		TR0=0;				//�رռ���
		Conut();			//����
		if(abs(S-oldST)>50)
		{
		//	huanshuiFlag = 1;
		//	BEEP = 0;
		//	LED = 0;
			
		}
		oldST = S;
		
		if((hour == 6|| hour == 7 || hour == 18 || hour == 19)&& LED == 1)		//����
		{
			if(X01 == 0)
			{
				LED = 0;	//����
				LEDBeginTime=0;
			}
			if(X02 == 0)
			{
				LED = 0;	//����
				LEDBeginTime=0;
			}
			if(X03 == 0)
			{
				LED = 0;	//����
				LEDBeginTime=0;
			}
			
//			if(X01 == 1 || X02 == 0 || X03 == 0)	//����
//			{
//				LED = 0;	//����
//				LEDBeginTime=0;
//			}
		}
		if(LED == 0)
		{
			if(BT1 == 0)
			{
				delayms(1);
				if(BT1== 0)
				{
					while(!BT1);
					LEDBeginTime=1*10;
				}
			}
			LEDBeginTime++;
			if(LEDBeginTime>(1*10*20))
			{
				LED = 1;//�ص�
				LEDBeginTime = 0;//�����ʱ
			}
		}
		if(hour == 19 && WeiYuFlag == 0 )	//��������7��ιʳ
		{
			WeiYuTimeFlag = ~WeiYuTimeFlag;	//����ιһ��
			if(WeiYuTimeFlag)
			{
				WS = 0;	//��ʼιʳ
				WTimetick++;
				if(WTimetick>1*10*20)
				{
					WS = 1;
				}
				WeiYuFlag = 1;
			}
		}
		if(hour != 19)
		{
			WeiYuFlag = 0;
		}
		if(BT2 == 0)
		{
			delayms(1);
			if(BT2== 0)
			{
				while(!BT2);
				Manual=1;
			}
		}
		DisplayOneChar(10, 1, Step+'0');	//��ʾ��ǰ����
			
		disbuff[0]=oldS%1000/100;
		disbuff[1]=oldS%1000%100/10;
		disbuff[2]=oldS%1000%10 %10;
		DisplayOneChar(5, 1, ASCII[disbuff[0]]);
		DisplayOneChar(6, 1, ASCII[disbuff[1]]);
		DisplayOneChar(7, 1, ASCII[disbuff[2]]);
		if(day==4 && huanshuiFlag==0 || Manual==1)	//ÿ��ʮ���ջ�ˮ
		{
			
			if(Step == 0)
			{
				Timetick = 0;
				Step = 1;
			}
			else if(Step == 1)
			{
				Timetick++;
				BEEP = 0;//��������
				
				if(Timetick>2)
				{
					K5 = 1;//�ر�ѭ��ˮ��
					K1=0;K2=0;	//��ˮ���ſ�
					oldS = S;
					Timetick = 0;
					BEEP = 1;
					Step = 2;
				}
			}
			else if(Step == 2)
			{
				if(S<300)//ˮλ��������֮һ
				{
					K1 = 1;
					K2 = 1;
					Timetick = 0;
					Step = 3;
				}
			}
			else if(Step == 3)
			{
				K3 = 0;
				K4 = 0;
				if(S>60)
				{
					K3 = 1;
					K4 = 1;
					Timetick = 0;
					Step = 4;
				}
			}
			else if(Step == 4)
			{
				Timetick++;
				K5=0;
				BEEP = 0;
				if(Timetick>5)
				{
					Timetick=0;
					BEEP = 1;
					Step = 0;
					Manual = 0;
					huanshuiFlag = 1;
				}
				
			}
			
		}
		if(day!=4 && Manual == 0)
		{
			K5 = 0;
			huanshuiFlag = 0;
		}
		delayms(100);		//80MS
	}
}

/********************************************************/
void zd0() interrupt 1 		 //T0�ж��������������,������෶Χ
{
	flag=1;							 //�ж������־
}

                