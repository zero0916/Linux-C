#include<reg52.h>
#define uchar unsigned char
#define uint unsigned int
sbit EN=P2^7; //LCDʹ��
sbit RS=P2^6;
sbit KEY=P3^5; //��0��
uchar table[]="0123456789:-";
uchar table1[]={"Wait..."};	 //
uchar a,b,c,d,i=0,flag;
uchar table0[62];
float S;
uint Speed_dat,Speed_dat1;
uint Total_distance;
uchar num1=0;
uchar time_flag=0;
void delay(uint z)//��ʱ����
{
   uint x,y;
   for(x=z;x>0;x--)
      for(y=110;y>0;y--);
}
/*******************1602Һ��***********************/

void write_date(uchar date)//д����
{ EN=0;	RS=1;  P0=date;		EN=0;  delay(5); EN=1;}
void write_com(uchar date)//д����
{	EN=0;RS=0;P0=date;	EN=0;delay(5); EN=1; }
void init_1602()  //LCD��ʼ��
{
	uchar num;
	write_com(0x38); write_com(0x0c);write_com(0x06);
	write_com(0x01); write_com(0x80);
	for(num=0;num<7;num++)	  //��ʾ��Wait...��	
	{
		write_date(table1[num]);//ͨ��ѭ����Ҫ��ʾ���ַ�ע����ʾ����
	}
	delay(1000);   //�ȴ���Լ3s
}

void display()
{
	if(flag==1)
    {  
	   flag=0;  
       if(table0[4]=='M'&&(table0[18]=='A'))//ȷ���Ƿ��յ�"GPRMC"��һ֡����	���Ƿ�Ϊ��Ч����
       {    
	   	    if(table0[43]=='E')      // 
			{
				if(table0[46]=='.')  //<10����ÿСʱ
					S=(table0[45]-48)*100+(table0[47]-48)*10+(table0[48]-48);
				else if(table0[47]=='.')
					S=(table0[45]-48)*1000+(table0[46]-48)*100+(table0[48]-48)*10+(table0[49]-48);
			}
			Speed_dat=(uint)(S*1.85);     //�Ѻ���/hת����km/h
			Speed_dat1=(uint)(S*0.514);	  //�Ѻ���/hת����m/s ������������õ�
			if(time_flag==1)
			{
				time_flag=0;
				Total_distance+=(Speed_dat1/100);	//ÿ��ˢ��һ����̵�λm
				TR0=1;
			}	 

			write_com(0x80);		//��ʾ�ٶ�
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

			write_com(0x80+0x40);		//��ʾ���
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
       ES=1; //�������ж�                                       			
   } 
}

void init()//��ʼ��
{
       i=0;
       TMOD=0x21;  PCON=0x00;
       TH1=0xfD;   TL1=0xfD;  //������9600
	   TH0=(65536-50000)/256; //����ʱ������λ����ֵ
 	   TL0=(65536-50000)%256; //����ʱ������λ����ֵ
	   ET0=1;	    //�򿪶�ʱ��T1�ж�
 	   TR0=1;		//������ʱ��T0
       SM0=0;	   //���пڷ�ʽ1
       SM1=1;
       REN=1;	   ////�������
       EA=1;	   //���ж�
       TR1=1;      //����T1
	   ES=0;	   //�رմ��п��ж�
}
void main()
{					
      init();     //ϵͳ��ʼ��
	  init_1602();//LCD1602��ʼ��
      ES=1;		  //�����п��ж�
      while(1)
      {
          display(); //��ʾ�ٶ�
		  if(!KEY)	 //��0������
		  {		delay(10);
				if(!KEY){Total_distance=0;while(!KEY); 	}
			}
      }
}

void time0() interrupt  1		  //��ʱ���ж�0
{
	TH0=(65536-50000)/256; //����ʱ������λ����ֵ
 	TL0=(65536-50000)%256; //����ʱ������λ����ֵ
	num1++;
	if(num1==20)   //1�뵽
	{
		TR0=0;
		num1=0;
		time_flag=1;
	}	
}

void ser() interrupt 4//�����жϡ����η���'$'��ͷ���ַ�������д���������
{
       RI=0;  table0[i]=SBUF; 	   i++;
       if(table0[0]==0x24)//����Ǵ��ַ������Ͻ�������
       {
	   		  if(i==62){ i=0;flag=1;ES=0; } 				             
       } 
       else	      i=0; 
}