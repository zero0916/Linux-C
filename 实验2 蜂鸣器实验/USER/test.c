#include "sys.h"
#include "delay.h" 
#include "led.h"
#include "beep.h"
//ALIENTEK ̽����STM32F407������ ʵ��2
//������ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾
  
int main(void)
{ 
	Stm32_Clock_Init(336,8,2,7);//����ʱ��,168Mhz
	delay_init(168);		//��ʼ����ʱ����
	LED_Init();				//��ʼ��LEDʱ��  
	BEEP_Init();         	//��ʼ���������˿�
	while(1)
	{
		LED0=0;
		BEEP=0;		  
		delay_ms(300);
		LED0=1;	  
		BEEP=1;  
		delay_ms(300);
	}	
}

















