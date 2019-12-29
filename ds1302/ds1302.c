#include "ds1302.h"

//DS1302���Ŷ���
sbit RST  	=	P1^4;
sbit IO		=	P1^3;
sbit SCK	=	P1^2;


unsigned char dis_time_buf[16]={0};

//DS1302��ַ����
#define ds1302_sec_add			0x80		//�����ݵ�ַ
#define ds1302_min_add			0x82		//�����ݵ�ַ
#define ds1302_hr_add			0x84		//ʱ���ݵ�ַ
#define ds1302_date_add			0x86		//�����ݵ�ַ
#define ds1302_month_add		0x88		//�����ݵ�ַ
#define ds1302_day_add			0x8a		//�������ݵ�ַ
#define ds1302_year_add			0x8c		//�����ݵ�ַ
#define ds1302_control_add		0x8e		//�������ݵ�ַ
#define ds1302_charger_add		0x90 					 
#define ds1302_clkburst_add		0xbe
//��ʼ��ʱ��
uchar time_buf[8] = {0x20,0x19,0x12,0x20,0x19,0x19,0x55,0x07};//��ʼʱ��2010��6��1��23��59��55�� ���ڶ�

////DS1302��ʼ������
//void ds1302_init(void) 
//{
//	RST=0;			//RST���õ�
//	SCK=0;			//SCK���õ�
//}
//��DS1302д��һ�ֽ�����
void ds1302_write_byte(uchar addr, uchar d) 
{
	uchar i;
	RST=1;					//����DS1302����	
	//д��Ŀ���ַ��addr
	addr = addr & 0xFE;   //���λ���㣬�Ĵ���0λΪ0ʱд��Ϊ1ʱ��
	for (i = 0; i < 8; i ++) {
		if (addr & 0x01) {
			IO=1;
			}
		else {
			IO=0;
			}
		SCK=1;      //����ʱ��
		SCK=0;
		addr = addr >> 1;
		}	
	//д�����ݣ�d
	for (i = 0; i < 8; i ++) {
		if (d & 0x01) {
			IO=1;
			}
		else {
			IO=0;
			}
		SCK=1;    //����ʱ��
		SCK=0;
		d = d >> 1;
		}
	RST=0;		//ֹͣDS1302����
}

//��DS1302����һ�ֽ�����
uchar ds1302_read_byte(uchar addr) 
{

	uchar i,temp;	
	RST=1;					//����DS1302����
	//д��Ŀ���ַ��addr
	addr = addr | 0x01;    //���λ�øߣ��Ĵ���0λΪ0ʱд��Ϊ1ʱ��
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
	//������ݣ�temp
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
	RST=0;					//ֹͣDS1302����
	return temp;
}
//��DS302д��ʱ������
void ds1302_write_time(void) 
{
	ds1302_write_byte(ds1302_control_add,0x00);			//�ر�д���� 
	ds1302_write_byte(ds1302_sec_add,0x80);				//��ͣʱ�� 
	ds1302_write_byte(ds1302_charger_add,0xa9);	    //������ 
	ds1302_write_byte(ds1302_year_add,time_buf[1]);		//�� 
	ds1302_write_byte(ds1302_month_add,time_buf[2]);	//�� 
	ds1302_write_byte(ds1302_date_add,time_buf[3]);		//�� 
	ds1302_write_byte(ds1302_hr_add,time_buf[4]);		//ʱ 
	ds1302_write_byte(ds1302_min_add,time_buf[5]);		//��
	ds1302_write_byte(ds1302_sec_add,time_buf[6]);		//��
	ds1302_write_byte(ds1302_day_add,time_buf[7]);		//�� 
	ds1302_write_byte(ds1302_control_add,0x80);			//��д����     
}
//��DS302����ʱ������
void ds1302_read_time(void)  
{
	time_buf[1]=ds1302_read_byte(ds1302_year_add);		//�� 
	time_buf[2]=ds1302_read_byte(ds1302_month_add);		//�� 
	time_buf[3]=ds1302_read_byte(ds1302_date_add);		//�� 
	time_buf[4]=ds1302_read_byte(ds1302_hr_add);		//ʱ 
	time_buf[5]=ds1302_read_byte(ds1302_min_add);		//�� 
	time_buf[6]=(ds1302_read_byte(ds1302_sec_add))&0x7f;//�룬������ĵ�7λ�����ⳬ��59
	time_buf[7]=ds1302_read_byte(ds1302_day_add);		//�� 	
	
	dis_time_buf[0]=(time_buf[0]>>4); //��   
	dis_time_buf[1]=(time_buf[0]&0x0f);

	dis_time_buf[2]=(time_buf[1]>>4);   
	dis_time_buf[3]=(time_buf[1]&0x0f);

	dis_time_buf[4]=(time_buf[2]>>4); //��  
	dis_time_buf[5]=(time_buf[2]&0x0f);

	dis_time_buf[6]=(time_buf[3]>>4); //��   
	dis_time_buf[7]=(time_buf[3]&0x0f);

	dis_time_buf[14]=(time_buf[7]&0x07); //����
	//��2����ʾ  
	dis_time_buf[8]=(time_buf[4]>>4); //ʱ   
	dis_time_buf[9]=(time_buf[4]&0x0f);   

	dis_time_buf[10]=(time_buf[5]>>4); //��   
	dis_time_buf[11]=(time_buf[5]&0x0f);   

	dis_time_buf[12]=(time_buf[6]>>4); //��   
	dis_time_buf[13]=(time_buf[6]&0x0f);
}

//��ʱ��2��ʼ��
void Init_timer2(void)
{
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TL1 = 0x00;		//���ö�ʱ��ֵ
	TH1 = 0xB8;		//���ö�ʱ��ֵ
	TF1 = 0;		//���TF1��־
	TR1 = 1;		//��ʱ��1��ʼ��ʱ
}


//��ʱ���жϺ���
void Timer2() interrupt 5	  //��ʱ��2��5���ж�
{
	static uchar t;
	TF2=0;
	t++;
	if(t==4)               //���200ms(50ms*4)��ȡһ��ʱ��
	{
		t=0;
		ds1302_read_time();  //��ʱ��ȡʱ�� 	
		dis_time_buf[0]=(time_buf[0]>>4); //��   
		dis_time_buf[1]=(time_buf[0]&0x0f);

		dis_time_buf[2]=(time_buf[1]>>4);   
		dis_time_buf[3]=(time_buf[1]&0x0f);

		dis_time_buf[4]=(time_buf[2]>>4); //��  
		dis_time_buf[5]=(time_buf[2]&0x0f);

		dis_time_buf[6]=(time_buf[3]>>4); //��   
		dis_time_buf[7]=(time_buf[3]&0x0f);

		dis_time_buf[14]=(time_buf[7]&0x07); //����
		//��2����ʾ  
		dis_time_buf[8]=(time_buf[4]>>4); //ʱ   
		dis_time_buf[9]=(time_buf[4]&0x0f);   

		dis_time_buf[10]=(time_buf[5]>>4); //��   
		dis_time_buf[11]=(time_buf[5]&0x0f);   

		dis_time_buf[12]=(time_buf[6]>>4); //��   
		dis_time_buf[13]=(time_buf[6]&0x0f);

	}
}





