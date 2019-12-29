#include "lcd1602.h"

#define Busy    0x80 //用于检测LCM状态字中的Busy标识

//1602液晶引脚
#define LCM_Data  P2
sbit LCM_RW = P0^6; //定义LCD引脚
sbit LCM_RS = P0^7;
sbit LCM_E  = P0^5;


unsigned char code ASCII[15] =    {'0','1','2','3','4','5','6','7','8','9','.','-','M'};

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
	while (DData[ListLength] > 0x19) //若到达字串尾则退出
	{
		if (X <= 0xF) //X坐标应小于0xF
		{
			DisplayOneChar( X, Y, DData[ListLength]); //显示单个字符
			ListLength++;
			X++;
		}
	}
}

