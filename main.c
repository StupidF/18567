#include <REG52.H>		//器件配置文件
#include "ds1302.h"
#include "lcd1602.h"
#include <intrins.h>
#include <math.h> 

//喂食标志
unsigned char WeiYuFlag = 0;
unsigned char WeiYuTimeFlag = 0;
//换水标志
unsigned char huanshuiFlag = 0;
unsigned char alarm       = 0;


//控制输出信号
sbit K1 = P0^0;		//电磁阀1
sbit K2 = P0^1;		//电磁阀2
sbit K3 = P0^2;		//电磁阀3
sbit K4 = P0^3;		//电磁阀4
sbit K5 = P0^4;		//水泵
//蜂鸣器
sbit BEEP = P3^3;
//喂食
sbit WS = P3^2;

//按键
sbit BT1 = P3^4;
sbit BT2 = P3^5;
sbit BT3 = P3^6;
//LED 灯光
sbit LED = P3^7;

//超声波引脚
sbit  RX =  P1^0;
sbit  TX  = P1^1;

//光电输入信号
sbit X01 = P1^5;
sbit X02 = P1^6;
sbit X03 = P1^7;


//灯亮的时间
unsigned int  LEDBeginTime= 0;
unsigned char LEDflag     = 0;
//手动标志
unsigned char Manual = 0;

static unsigned char DisNum = 0; //显示用指针zz				  
       unsigned int  time=0;
	   unsigned long S=0;
	   unsigned long oldS=0;
	   unsigned long oldST=0;
	   bit      flag =0;
	   unsigned char disbuff[4]	   ={ 0,0,0,0,};

extern unsigned char dis_time_buf[16];	   
	   //5ms延时
void Delay5Ms(void)
{
	unsigned int TempCyc = 5552;
	while(TempCyc--);
}

//400ms延时
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
void  StartModule() 		         //启动模块
{
	  TX=1;			                     //启动一次模块
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

//定时器初始化
void TimerInit(void)
{
	TMOD=0x01;		   //设T0为方式1，GATE=1；
	TH0=0;
	TL0=0;          
	ET0=1;             //允许T0中断
	EA=1;			   //开启总中断	
}


/********************************************************/
void Conut(void)
{
	StartModule();		//启动测量
	while(!RX);			//当RX为零时等待
	TR0 = 1;			    //开启计数
	while(RX);			//当RX为1计数并等待
	TR0 = 0;				//关闭计数
	
	time=TH0*256+TL0;
	TH0=0;
	TL0=0;

	S=(time*1.7)/100;     //算出来是CM
	if((S>=700) || flag == 1) //超出测量范围显示“-”
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

//显示时间
void displayTimes()
{
	ds1302_read_time();	//读取时间
	
	month  = dis_time_buf[4 ]*10 + dis_time_buf[5 ];
	day    = dis_time_buf[6 ]*10 + dis_time_buf[7 ];
	hour   = dis_time_buf[8 ]*10 + dis_time_buf[9 ];
	minite = dis_time_buf[10]*10 + dis_time_buf[11];
	sec    = dis_time_buf[12]*10 + dis_time_buf[13];
	
	DisplayOneChar(1 , 0, month%100/10 +'0');
	DisplayOneChar(2 , 0, month%10     +'0');//月份
	DisplayOneChar(3 , 0, '-');
	DisplayOneChar(4 , 0, day%100/10   +'0');
	DisplayOneChar(5 , 0, day%10       +'0');//日
	
	DisplayOneChar(7 , 0, hour%100/10  +'0');
	DisplayOneChar(8 , 0, hour%10      +'0');//时
	DisplayOneChar(9 , 0, ':');
	DisplayOneChar(10, 0, minite%100/10+'0');
	DisplayOneChar(11, 0, minite%10+'0');//分
	DisplayOneChar(12, 0, ':');
	DisplayOneChar(13, 0, sec%100/10+'0');
	DisplayOneChar(14, 0, sec%10+'0');//秒
	
}
//喂食动作
void Act_WS(void)
{
	WS = 0;
	Delay400Ms(); 
	Delay400Ms();
	WS = 1;
}

void Sys_Init()
{
	Delay400Ms(); 	//启动等待，等LCM讲入工作状态
	LCMInit(); 		//LCM初始化
	Delay400Ms(); 	//延时
	TimerInit();	//定时器初始化
	Init_timer2();
	ds1302_write_time(); //写入初始值
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
		displayTimes();		//显示时间
		
		if(sec_old != sec)//一秒检测一次液位
		{
			LEDBeginTime++;	//灯光计时
			secs++;
			Conut();			//测量液位
		}
		sec_old = sec;
		if(abs( S - oldST )>2)	//报警
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
		
		if((hour >= 6 && hour <= 8)|| (hour >= 18 && hour <= 22))			//6-8点   18点到22点 
		{
			if(LED == 1 && LEDflag ==0)
			{
				if( X01 == 0 || X02 == 0 || X03 == 0)		//有信号亮灯
				{
					LED = 0;	//开灯
					LEDBeginTime = 0;
				}
			}
			else 
			{
				if(BT1 == 0)	//手动关灯
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
				if(LEDBeginTime > 10)//7200秒  自动关灯
				{
					LED = 1;//关灯
					LEDBeginTime = 0;//清除计时
				}
			}
		}
		else 
		{
			LEDflag = 0;
			LED = 1;
			LEDBeginTime = 0;
		}
		if(day %2 ==0 && hour == 7 && WeiYuFlag == 0 )	//getian 晚上7点喂食
		{
			Act_WS();
			WeiYuFlag = 1;
		}
		if(hour != 7)
		{
			WeiYuFlag = 0;
		}
		if(BT2 == 0)	//手动喂食
		{
			delayms(1);
			if(BT2 == 0)
			{
				while(!BT2);
				DisplayOneChar(13, 1, '2');	
				Act_WS();
			}
		}
		if(BT3 == 0)//手动换水
		{
			delayms(1);			

			if(BT3 == 0)
			{
				while(!BT3);
				DisplayOneChar(13, 1, '3');	
				Manual=1;
			}
		}
		DisplayOneChar(10, 1, Step+'0');	//显示当前步骤
			
		DisplayOneChar(5, 1, oldS%1000/100+0x30);
		DisplayOneChar(6, 1, oldS%1000%100/10+0x30);
		DisplayOneChar(7, 1, oldS%1000%10 %10+0x30);
		if(((day==2 || day==16)&& huanshuiFlag==0 && LED == 0 && alarm == 0) || Manual==1)	//每月4日 20日在亮灯情况下换水
		{
			if(Step == 0)
			{
				Timetick = 0;
				Step = 1;
			}
			else if(Step == 1)
			{
				Timetick++;
				BEEP = 1;//蜂鸣器叫
				
				if(Timetick > 2)
				{
					K5 = 1;//关闭循环水泵
					K1 = 0;
					K2 = 0;	//排水阀门开
					oldS = S;
					oldS = oldS/3*2;
					Timetick = 0;
					BEEP = 0;
					Step = 2;
				}
			}
			else if(Step == 2)
			{
				if(S < oldS)	//水位降低三分之一
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
		if(day!=2 && day!=16)
		{
			
			huanshuiFlag = 0;
		}
		if(alarm == 0 && Step == 0)
			K5 = 0;
	
		delayms(100);		//80MS
	}
}

/********************************************************/
void zd0() interrupt 1 		 //T0中断用来计数器溢出,超过测距范围
{
	flag=1;							 //中断溢出标志
}

                