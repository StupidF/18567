#include <REG52.H>		//器件配置文件
#include "ds1302.h"
#include <intrins.h>
#include <math.h> 
sfr P4 = 0XC0;

#define Key_Data P2_0 //定义Keyboard引脚
#define Key_CLK  P3_2

#define Busy    0x80 //用于检测LCM状态字中的Busy标识
//喂食标志
unsigned char WeiYuFlag = 0;
unsigned char WeiYuTimeFlag = 0;
//换水标志
unsigned char huanshuiFlag = 0;

//1602液晶引脚
#define LCM_Data  P2
sbit LCM_RW = P0^6; //定义LCD引脚
sbit LCM_RS = P0^7;
sbit LCM_E  = P0^5;

//控制输出信号
sbit K1 = P0^0;		//电磁阀1
sbit K2 = P0^1;		//电磁阀2
sbit K3 = P0^2;		//电磁阀3
sbit K4 = P0^3;		//电磁阀4
sbit K5 = P0^4;		//水泵

//按键
sbit BT1 = P3^4;
sbit BT2 = P3^5;
sbit BT3 = P3^6;

//超声波引脚
sbit  RX =  P1^0;
sbit  TX  = P1^1;

//光电输入信号
sbit X01 = P1^5;
sbit X02 = P1^6;
sbit X03 = P1^7;
//LED 灯光
sbit LED = P3^7;
//蜂鸣器
sbit BEEP = P3^3;
//喂食
sbit WS = P3^5;

//灯亮的时间
unsigned int  LEDBeginTime= 0;

unsigned char Manual = 0;

unsigned char month, day,hour,minite;
unsigned char oldhour=0,oldminite=0;
unsigned char ReadDataLCM(void);
unsigned char ReadStatusLCM(void);
unsigned char code ASCII[15] =    {'0','1','2','3','4','5','6','7','8','9','.','-','M'};

static unsigned char DisNum = 0; //显示用指针				  
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


//写数据
void WriteDataLCM(unsigned char WDLCM) 
{
	ReadStatusLCM(); //检测忙
	LCM_Data = WDLCM;
	LCM_RS = 1;
	LCM_RW = 0;
	LCM_E = 0; //若晶振速度太高可以在这后加小的延时
	LCM_E = 0; //延时
	LCM_E = 1;
}

//写指令
void WriteCommandLCM(unsigned char WCLCM,BuysC) //BuysC为0时忽略忙检测
{
	if (BuysC) ReadStatusLCM(); //根据需要检测忙
	LCM_Data = WCLCM;
	LCM_RS = 0;
	LCM_RW = 0;	
	LCM_E = 0;
	LCM_E = 0;
	LCM_E = 1;	
}

//读数据
unsigned char ReadDataLCM(void)
{
	LCM_RS = 1; 
	LCM_RW = 1;
	LCM_E = 0;
	LCM_E = 0;
	LCM_E = 1;
	return(LCM_Data);
}

//读状态
unsigned char ReadStatusLCM(void)
{
	LCM_Data = 0xFF; 
	LCM_RS = 0;
	LCM_RW = 1;
	LCM_E = 0;
	LCM_E = 0;
	LCM_E = 1;
	while (LCM_Data & Busy); //检测忙信号
	return(LCM_Data);
}

void LCMInit(void) //LCM初始化
{
	LCM_Data = 0;
	WriteCommandLCM(0x38,0); //三次显示模式设置，不检测忙信号
	Delay5Ms(); 
	WriteCommandLCM(0x38,0);
	Delay5Ms(); 
	WriteCommandLCM(0x38,0);
	Delay5Ms(); 

	WriteCommandLCM(0x38,1); //显示模式设置,开始要求每次检测忙信号
	WriteCommandLCM(0x0c,1); //关闭显示
	WriteCommandLCM(0x01,1); //显示清屏
	WriteCommandLCM(0x06,0); // 显示光标移动设置
}

//按指定位置显示一个字符
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData)
{
	Y &= 0x1;
	X &= 0xF; //限制X不能大于15，Y不能大于1
	if (Y) X |= 0x40; //当要显示第二行时地址码+0x40;
	X |= 0x80; //算出指令码
	WriteCommandLCM(X, 1); //发命令字
	WriteDataLCM(DData); //发数据
}

//按指定位置显示一串字符
void DisplayListChar(unsigned char X, unsigned char Y, unsigned char code *DData)
{
	unsigned char ListLength;

	ListLength = 0;
	Y &= 0x1;
	X &= 0xF; //限制X不能大于15，Y不能大于1
	while (DData[ListLength]>0x19) //若到达字串尾则退出
	{
		if (X <= 0xF) //X坐标应小于0xF
		{
			DisplayOneChar(X, Y, DData[ListLength]); //显示单个字符
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

	S=(time*1.7)/100;     //算出来是CM
	if((S>=700)||flag==1) //超出测量范围显示“-”
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


void displayTimes()
{
	ds1302_read_time();	//读取时间
	month =  dis_time_buf[4]*10+dis_time_buf[5];
	day = dis_time_buf[6]*10+dis_time_buf[7];
	hour = dis_time_buf[8]*10+dis_time_buf[9];
	minite = dis_time_buf[10]*10+dis_time_buf[11];
	
	DisplayOneChar(1, 0, month%100/10+'0');
	DisplayOneChar(2, 0, month%10+'0');//时
	
	DisplayOneChar(4, 0, day%100/10+'0');
	DisplayOneChar(5, 0, day%10+'0');//时
	
	DisplayOneChar(7, 0, hour%100/10+'0');
	DisplayOneChar(8, 0, hour%10+'0');//分
	DisplayOneChar(9, 0, ':');
	DisplayOneChar(10, 0, minite%100/10+'0');
	DisplayOneChar(11, 0, minite%10+'0');//分
	DisplayOneChar(12, 0, ':');
	DisplayOneChar(13, 0, dis_time_buf[12]+'0');
	DisplayOneChar(14, 0, dis_time_buf[13]+'0');//秒
	
}
/*********************************************************/
void main(void)
{
	unsigned char TempCyc;
	unsigned char Step=0;
	unsigned int  Timetick=0;
	unsigned int  WTimetick=0;
	Delay400Ms(); 	//启动等待，等LCM讲入工作状态
	LCMInit(); 		//LCM初始化
	Delay5Ms(); 	//延时片刻(可不要)
	ReadDataLCM();	//测试用句无意义
	for (TempCyc=0; TempCyc<10; TempCyc++)
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
	BEEP = 1;
	
	StartModule();		//启动测量
	while(!RX);			//当RX为零时等待
	TR0=1;			    //开启计数
	while(RX);			//当RX为1计数并等待
	TR0=0;				//关闭计数
	Conut();			//计算
	oldST = S;
	
	while(1)
	{
		displayTimes();		//显示时间
		//测量液位
		StartModule();		//启动测量
		while(!RX);			//当RX为零时等待
		TR0=1;			    //开启计数
		while(RX);			//当RX为1计数并等待
		TR0=0;				//关闭计数
		Conut();			//计算
		if(abs(S-oldST)>50)
		{
		//	huanshuiFlag = 1;
		//	BEEP = 0;
		//	LED = 0;
			
		}
		oldST = S;
		
		if((hour == 6|| hour == 7 || hour == 18 || hour == 19)&& LED == 1)		//亮灯
		{
			if(X01 == 0)
			{
				LED = 0;	//开灯
				LEDBeginTime=0;
			}
			if(X02 == 0)
			{
				LED = 0;	//开灯
				LEDBeginTime=0;
			}
			if(X03 == 0)
			{
				LED = 0;	//开灯
				LEDBeginTime=0;
			}
			
//			if(X01 == 1 || X02 == 0 || X03 == 0)	//灯亮
//			{
//				LED = 0;	//开灯
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
				LED = 1;//关灯
				LEDBeginTime = 0;//清除计时
			}
		}
		if(hour == 19 && WeiYuFlag == 0 )	//隔天晚上7点喂食
		{
			WeiYuTimeFlag = ~WeiYuTimeFlag;	//隔天喂一次
			if(WeiYuTimeFlag)
			{
				WS = 0;	//开始喂食
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
		DisplayOneChar(10, 1, Step+'0');	//显示当前步骤
			
		disbuff[0]=oldS%1000/100;
		disbuff[1]=oldS%1000%100/10;
		disbuff[2]=oldS%1000%10 %10;
		DisplayOneChar(5, 1, ASCII[disbuff[0]]);
		DisplayOneChar(6, 1, ASCII[disbuff[1]]);
		DisplayOneChar(7, 1, ASCII[disbuff[2]]);
		if(day==4 && huanshuiFlag==0 || Manual==1)	//每月十五日换水
		{
			
			if(Step == 0)
			{
				Timetick = 0;
				Step = 1;
			}
			else if(Step == 1)
			{
				Timetick++;
				BEEP = 0;//蜂鸣器叫
				
				if(Timetick>2)
				{
					K5 = 1;//关闭循环水泵
					K1=0;K2=0;	//排水阀门开
					oldS = S;
					Timetick = 0;
					BEEP = 1;
					Step = 2;
				}
			}
			else if(Step == 2)
			{
				if(S<300)//水位降低三分之一
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
void zd0() interrupt 1 		 //T0中断用来计数器溢出,超过测距范围
{
	flag=1;							 //中断溢出标志
}

                