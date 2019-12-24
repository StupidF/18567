#include "ds1302.h"

//DS1302引脚定义
sbit RST  	=	P1^4;
sbit IO		=	P1^3;
sbit SCK	=	P1^2;


unsigned char dis_time_buf[16]={0};

//DS1302地址定义
#define ds1302_sec_add			0x80		//秒数据地址
#define ds1302_min_add			0x82		//分数据地址
#define ds1302_hr_add			0x84		//时数据地址
#define ds1302_date_add			0x86		//日数据地址
#define ds1302_month_add		0x88		//月数据地址
#define ds1302_day_add			0x8a		//星期数据地址
#define ds1302_year_add			0x8c		//年数据地址
#define ds1302_control_add		0x8e		//控制数据地址
#define ds1302_charger_add		0x90 					 
#define ds1302_clkburst_add		0xbe
//初始化时间
uchar time_buf[8] = {0x20,0x10,0x06,0x15,0x07,0x47,0x55,0x06};//初始时间2010年6月1号23点59分55秒 星期二

//DS1302初始化函数
void ds1302_init(void) 
{
	RST=0;			//RST脚置低
	SCK=0;			//SCK脚置低
}
//向DS1302写入一字节数据
void ds1302_write_byte(uchar addr, uchar d) 
{
	uchar i;
	RST=1;					//启动DS1302总线	
	//写入目标地址：addr
	addr = addr & 0xFE;   //最低位置零，寄存器0位为0时写，为1时读
	for (i = 0; i < 8; i ++) {
		if (addr & 0x01) {
			IO=1;
			}
		else {
			IO=0;
			}
		SCK=1;      //产生时钟
		SCK=0;
		addr = addr >> 1;
		}	
	//写入数据：d
	for (i = 0; i < 8; i ++) {
		if (d & 0x01) {
			IO=1;
			}
		else {
			IO=0;
			}
		SCK=1;    //产生时钟
		SCK=0;
		d = d >> 1;
		}
	RST=0;		//停止DS1302总线
}

//从DS1302读出一字节数据
uchar ds1302_read_byte(uchar addr) 
{

	uchar i,temp;	
	RST=1;					//启动DS1302总线
	//写入目标地址：addr
	addr = addr | 0x01;    //最低位置高，寄存器0位为0时写，为1时读
	for (i = 0; i < 8; i ++) {
		if (addr & 0x01) {
			IO=1;
			}
		else {
			IO=0;
			}
		SCK=1;
		SCK=0;
		addr = addr >> 1;
		}	
	//输出数据：temp
	for (i = 0; i < 8; i ++) {
		temp = temp >> 1;
		if (IO) {
			temp |= 0x80;
			}
		else {
			temp &= 0x7F;
			}
		SCK=1;
		SCK=0;
		}	
	RST=0;					//停止DS1302总线
	return temp;
}
//向DS302写入时钟数据
void ds1302_write_time(void) 
{
	ds1302_write_byte(ds1302_control_add,0x00);			//关闭写保护 
	ds1302_write_byte(ds1302_sec_add,0x80);				//暂停时钟 
//	ds1302_write_byte(ds1302_charger_add,0xa9);	    //涓流充电 
	ds1302_write_byte(ds1302_year_add,time_buf[1]);		//年 
	ds1302_write_byte(ds1302_month_add,time_buf[2]);	//月 
	ds1302_write_byte(ds1302_date_add,time_buf[3]);		//日 
	ds1302_write_byte(ds1302_hr_add,time_buf[4]);		//时 
	ds1302_write_byte(ds1302_min_add,time_buf[5]);		//分
	ds1302_write_byte(ds1302_sec_add,time_buf[6]);		//秒
	ds1302_write_byte(ds1302_day_add,time_buf[7]);		//周 
	ds1302_write_byte(ds1302_control_add,0x80);			//打开写保护     
}
//从DS302读出时钟数据
void ds1302_read_time(void)  
{
	time_buf[1]=ds1302_read_byte(ds1302_year_add);		//年 
	time_buf[2]=ds1302_read_byte(ds1302_month_add);		//月 
	time_buf[3]=ds1302_read_byte(ds1302_date_add);		//日 
	time_buf[4]=ds1302_read_byte(ds1302_hr_add);		//时 
	time_buf[5]=ds1302_read_byte(ds1302_min_add);		//分 
	time_buf[6]=(ds1302_read_byte(ds1302_sec_add))&0x7f;//秒，屏蔽秒的第7位，避免超出59
	time_buf[7]=ds1302_read_byte(ds1302_day_add);		//周 	
	
	dis_time_buf[0]=(time_buf[0]>>4); //年   
	dis_time_buf[1]=(time_buf[0]&0x0f);

	dis_time_buf[2]=(time_buf[1]>>4);   
	dis_time_buf[3]=(time_buf[1]&0x0f);

	dis_time_buf[4]=(time_buf[2]>>4); //月  
	dis_time_buf[5]=(time_buf[2]&0x0f);

	dis_time_buf[6]=(time_buf[3]>>4); //日   
	dis_time_buf[7]=(time_buf[3]&0x0f);

	dis_time_buf[14]=(time_buf[7]&0x07); //星期
	//第2行显示  
	dis_time_buf[8]=(time_buf[4]>>4); //时   
	dis_time_buf[9]=(time_buf[4]&0x0f);   

	dis_time_buf[10]=(time_buf[5]>>4); //分   
	dis_time_buf[11]=(time_buf[5]&0x0f);   

	dis_time_buf[12]=(time_buf[6]>>4); //秒   
	dis_time_buf[13]=(time_buf[6]&0x0f);
}

//定时器2初始化
void Init_timer2(void)
{
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x00;		//设置定时初值
	TH1 = 0xB8;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
}


//定时器中断函数
void Timer2() interrupt 5	  //定时器2是5号中断
{
	static uchar t;
	TF2=0;
	t++;
	if(t==4)               //间隔200ms(50ms*4)读取一次时间
	{
		t=0;
		ds1302_read_time();  //定时读取时间 	
		dis_time_buf[0]=(time_buf[0]>>4); //年   
		dis_time_buf[1]=(time_buf[0]&0x0f);

		dis_time_buf[2]=(time_buf[1]>>4);   
		dis_time_buf[3]=(time_buf[1]&0x0f);

		dis_time_buf[4]=(time_buf[2]>>4); //月  
		dis_time_buf[5]=(time_buf[2]&0x0f);

		dis_time_buf[6]=(time_buf[3]>>4); //日   
		dis_time_buf[7]=(time_buf[3]&0x0f);

		dis_time_buf[14]=(time_buf[7]&0x07); //星期
		//第2行显示  
		dis_time_buf[8]=(time_buf[4]>>4); //时   
		dis_time_buf[9]=(time_buf[4]&0x0f);   

		dis_time_buf[10]=(time_buf[5]>>4); //分   
		dis_time_buf[11]=(time_buf[5]&0x0f);   

		dis_time_buf[12]=(time_buf[6]>>4); //秒   
		dis_time_buf[13]=(time_buf[6]&0x0f);

	}
}





