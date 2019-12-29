#include <REG52.H>		//���������ļ�
#include "ds1302.h"
#include "lcd1602.h"
#include <intrins.h>
#include <math.h> 

//ιʳ��־
unsigned char WeiYuFlag = 0;
unsigned char WeiYuTimeFlag = 0;
//��ˮ��־
unsigned char huanshuiFlag = 0;
unsigned char alarm       = 0;


//��������ź�
sbit K1 = P0^0;		//��ŷ�1
sbit K2 = P0^1;		//��ŷ�2
sbit K3 = P0^2;		//��ŷ�3
sbit K4 = P0^3;		//��ŷ�4
sbit K5 = P0^4;		//ˮ��
//������
sbit BEEP = P3^3;
//ιʳ
sbit WS = P3^2;

//����
sbit BT1 = P3^4;
sbit BT2 = P3^5;
sbit BT3 = P3^6;
//LED �ƹ�
sbit LED = P3^7;

//����������
sbit  RX =  P1^0;
sbit  TX  = P1^1;

//��������ź�
sbit X01 = P1^5;
sbit X02 = P1^6;
sbit X03 = P1^7;


//������ʱ��
unsigned int  LEDBeginTime= 0;
unsigned char LEDflag     = 0;
//�ֶ���־
unsigned char Manual = 0;

static unsigned char DisNum = 0; //��ʾ��ָ��zz				  
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


/********************************************************/
void Conut(void)
{
	StartModule();		//��������
	while(!RX);			//��RXΪ��ʱ�ȴ�
	TR0 = 1;			    //��������
	while(RX);			//��RXΪ1�������ȴ�
	TR0 = 0;				//�رռ���
	
	time=TH0*256+TL0;
	TH0=0;
	TL0=0;

	S=(time*1.7)/100;     //�������CM
	if((S>=700) || flag == 1) //����������Χ��ʾ��-��
	{	 
		flag=0;

		DisplayOneChar(0, 1, '-');
		DisplayOneChar(1, 1, '-');
		DisplayOneChar(2, 1, '-');
	}
	else
	{
		DisplayOneChar(0, 1, S%1000/100+0x30);
		DisplayOneChar(1, 1, S%1000%100/10+0x30);
		DisplayOneChar(2, 1, S%1000%10 %10+0x30);
	}
}

unsigned char month, day,hour,minite,sec;
unsigned char day_old,hour_old,minite_old,sec_old;
unsigned int  months, days,hours,minites,secs;
unsigned char oldhour=0,oldminite=0;

//��ʾʱ��
void displayTimes()
{
	ds1302_read_time();	//��ȡʱ��
	
	month  = dis_time_buf[4 ]*10 + dis_time_buf[5 ];
	day    = dis_time_buf[6 ]*10 + dis_time_buf[7 ];
	hour   = dis_time_buf[8 ]*10 + dis_time_buf[9 ];
	minite = dis_time_buf[10]*10 + dis_time_buf[11];
	sec    = dis_time_buf[12]*10 + dis_time_buf[13];
	
	DisplayOneChar(1 , 0, month%100/10 +'0');
	DisplayOneChar(2 , 0, month%10     +'0');//�·�
	DisplayOneChar(3 , 0, '-');
	DisplayOneChar(4 , 0, day%100/10   +'0');
	DisplayOneChar(5 , 0, day%10       +'0');//��
	
	DisplayOneChar(7 , 0, hour%100/10  +'0');
	DisplayOneChar(8 , 0, hour%10      +'0');//ʱ
	DisplayOneChar(9 , 0, ':');
	DisplayOneChar(10, 0, minite%100/10+'0');
	DisplayOneChar(11, 0, minite%10+'0');//��
	DisplayOneChar(12, 0, ':');
	DisplayOneChar(13, 0, sec%100/10+'0');
	DisplayOneChar(14, 0, sec%10+'0');//��
	
}
//ιʳ����
void Act_WS(void)
{
	WS = 0;
	Delay400Ms(); 
	Delay400Ms();
	WS = 1;
}

void Sys_Init()
{
	Delay400Ms(); 	//�����ȴ�����LCM���빤��״̬
	LCMInit(); 		//LCM��ʼ��
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
	BEEP = 0;
	Conut();
	oldST = S;
}
/*********************************************************/
void main(void)
{
	unsigned char Step=0;
	unsigned int  Timetick=0;
	unsigned int  WTimetick=0;
	Sys_Init();

	while(1)
	{
		displayTimes();		//��ʾʱ��
		
		if(sec_old != sec)//һ����һ��Һλ
		{
			LEDBeginTime++;	//�ƹ��ʱ
			secs++;
			Conut();			//����Һλ
		}
		sec_old = sec;
		if(abs( S - oldST )>2)	//����
		{
			if(S<oldST)
			{
				alarm = 1;
				huanshuiFlag = 1;
			}
		}
		if(alarm == 1)
		{
			BEEP = 1;
			K1 = 1;
			K2 = 1;
			K3 = 1;
			K4 = 1;
			K5 = 1;
			Step = 0;
		}
		DisplayOneChar(15, 1, '0'+alarm);	
		oldST = S;
		
		if((hour >= 6 && hour <= 8)|| (hour >= 18 && hour <= 22))			//6-8��   18�㵽22�� 
		{
			if(LED == 1 && LEDflag ==0)
			{
				if( X01 == 0 || X02 == 0 || X03 == 0)		//���ź�����
				{
					LED = 0;	//����
					LEDBeginTime = 0;
				}
			}
			else 
			{
				if(BT1 == 0)	//�ֶ��ص�
				{	
					delayms(1);
					if(BT1== 0)
					{
						while( !BT1 );
						DisplayOneChar(13, 1, '1');	
						LEDflag = 1;
						LEDBeginTime = 7200;
					}
				}
				if(LEDBeginTime > 10)//7200��  �Զ��ص�
				{
					LED = 1;//�ص�
					LEDBeginTime = 0;//�����ʱ
				}
			}
		}
		else 
		{
			LEDflag = 0;
			LED = 1;
			LEDBeginTime = 0;
		}
		if(day %2 ==0 && hour == 19 && WeiYuFlag == 0 )	//getian ����7��ιʳ
		{
			Act_WS();
			WeiYuFlag = 1;
		}
		if(hour != 19)
		{
			WeiYuFlag = 0;
		}
		if(BT2 == 0)	//�ֶ�ιʳ
		{
			delayms(1);
			if(BT2 == 0)
			{
				while(!BT2);
				DisplayOneChar(13, 1, '2');	
				Act_WS();
			}
		}
		if(BT3 == 0)//�ֶ���ˮ
		{
			delayms(1);			

			if(BT3 == 0)
			{
				while(!BT3);
				DisplayOneChar(13, 1, '3');	
				Manual=1;
			}
		}
		DisplayOneChar(10, 1, Step+'0');	//��ʾ��ǰ����
			
		DisplayOneChar(5, 1, oldS%1000/100+0x30);
		DisplayOneChar(6, 1, oldS%1000%100/10+0x30);
		DisplayOneChar(7, 1, oldS%1000%10 %10+0x30);
		if(((day==15 || day==30)&& huanshuiFlag==0 && LED == 0 && alarm == 0) || Manual==1)	//ÿ��4�� 20������������»�ˮ
		{
			if(Step == 0)
			{
				Timetick = 0;
				Step = 1;
			}
			else if(Step == 1)
			{
				Timetick++;
				BEEP = 1;//��������
				
				if(Timetick > 2)
				{
					K5 = 1;//�ر�ѭ��ˮ��
					K1 = 0;
					K2 = 0;	//��ˮ���ſ�
					oldS = S;
					oldS = oldS/3*2;
					Timetick = 0;
					BEEP = 0;
					Step = 2;
				}
			}
			else if(Step == 2)
			{
				if(S < oldS)	//ˮλ��������֮һ
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
				if(S > 30)
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
				K5   = 0;
				BEEP = 1;
				if(Timetick>5)
				{
					Timetick = 0;
					BEEP = 0;
					Step = 0;
					Manual = 0;
					huanshuiFlag = 1;
				}
			}
		}
		if(day!=15 && day!=30)
		{
			
			huanshuiFlag = 0;
		}
		if(alarm == 0 && Step == 0)
			K5 = 0;
	
		delayms(100);		//80MS
	}
}

/********************************************************/
void zd0() interrupt 1 		 //T0�ж��������������,������෶Χ
{
	flag=1;							 //�ж������־
}

                