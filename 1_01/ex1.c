#include<reg52.h>
#define uchar unsigned char
#define uint unsigned int
sbit EN=P2^7; //LCD使能
sbit RS=P2^6;
sbit KEY=P3^5; //清0键
uchar table[]="0123456789:-";
uchar table1[]={"Wait..."};	 //
uchar a,b,c,d,i=0,flag;
uchar table0[62];
float S;
uint Speed_dat,Speed_dat1;
uint Total_distance;
uchar num1=0;
uchar time_flag=0;
void delay(uint z)//延时函数
{
   uint x,y;
   for(x=z;x>0;x--)
      for(y=110;y>0;y--);
}
/*******************1602液晶***********************/

void write_date(uchar date)//写数据
{ EN=0;	RS=1;  P0=date;		EN=0;  delay(5); EN=1;}
void write_com(uchar date)//写命令
{	EN=0;RS=0;P0=date;	EN=0;delay(5); EN=1; }
void init_1602()  //LCD初始化
{
	uchar num;
	write_com(0x38); write_com(0x0c);write_com(0x06);
	write_com(0x01); write_com(0x80);
	for(num=0;num<7;num++)	  //显示“Wait...”	
	{
		write_date(table1[num]);//通过循环将要显示的字符注意显示出来
	}
	delay(1000);   //等待大约3s
}

void display()
{
	if(flag==1)
    {  
	   flag=0;  
       if(table0[4]=='M'&&(table0[18]=='A'))//确定是否收到"GPRMC"这一帧数据	且是否为有效数据
       {    
	   	    if(table0[43]=='E')      // 
			{
				if(table0[46]=='.')  //<10海里每小时
					S=(table0[45]-48)*100+(table0[47]-48)*10+(table0[48]-48);
				else if(table0[47]=='.')
					S=(table0[45]-48)*1000+(table0[46]-48)*100+(table0[48]-48)*10+(table0[49]-48);
			}
			Speed_dat=(uint)(S*1.85);     //把海里/h转换成km/h
			Speed_dat1=(uint)(S*0.514);	  //把海里/h转换成m/s 计算总里程中用到
			if(time_flag==1)
			{
				time_flag=0;
				Total_distance+=(Speed_dat1/100);	//每秒刷新一次里程单位m
				TR0=1;
			}	 

			write_com(0x80);		//显示速度
			write_date('S');
			write_date('P');
			write_date('E');
			write_date('E');
			write_date('D');
			write_date(':');
			if(Speed_dat/100<10){write_date(' ');write_date(table[Speed_dat/100]);}					  
			else{write_date(table[Speed_dat/1000]);write_date(table[Speed_dat%1000/100]);}	  
		    write_date('.');
		    write_date(table[Speed_dat%100/10]);
		    write_date('K');   write_date('m'); write_date('/');  write_date('h'); 

			write_com(0x80+0x40);		//显示里程
			write_date('J');write_date('O');write_date('U');write_date('R');
			write_date('N');write_date('E');write_date('Y');
			write_date(':');
			write_date(table[Total_distance/10000]);
			write_date(table[Total_distance%10000/1000]);
			write_date('.');
			write_date(table[Total_distance%10000%1000/100]);
			write_date(table[Total_distance%10000%1000%100/10]);
			write_date('K');	write_date('m');
       }
       ES=1; //开串口中断                                       			
   } 
}

void init()//初始化
{
       i=0;
       TMOD=0x21;  PCON=0x00;
       TH1=0xfD;   TL1=0xfD;  //波特率9600
	   TH0=(65536-50000)/256; //给定时器高四位赋初值
 	   TL0=(65536-50000)%256; //给定时器第四位赋初值
	   ET0=1;	    //打开定时器T1中段
 	   TR0=1;		//启动定时器T0
       SM0=0;	   //串行口方式1
       SM1=1;
       REN=1;	   ////允许接收
       EA=1;	   //开中断
       TR1=1;      //启动T1
	   ES=0;	   //关闭串行口中断
}
void main()
{					
      init();     //系统初始化
	  init_1602();//LCD1602初始化
      ES=1;		  //开串行口中断
      while(1)
      {
          display(); //显示速度
		  if(!KEY)	 //清0键按下
		  {		delay(10);
				if(!KEY){Total_distance=0;while(!KEY); 	}
			}
      }
}

void time0() interrupt  1		  //定时器中断0
{
	TH0=(65536-50000)/256; //给定时器高四位赋初值
 	TL0=(65536-50000)%256; //给定时器第四位赋初值
	num1++;
	if(num1==20)   //1秒到
	{
		TR0=0;
		num1=0;
		time_flag=1;
	}	
}

void ser() interrupt 4//串口中断、初次分离'$'开头的字符串，并写入接收数组
{
       RI=0;  table0[i]=SBUF; 	   i++;
       if(table0[0]==0x24)//如果是此字符；马上接收数据
       {
	   		  if(i==62){ i=0;flag=1;ES=0; } 				             
       } 
       else	      i=0; 
}