#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "string.h"		
#include "led.h"
#include "TIME.h"
#include "mpu6050.h"


float Pitch,Roll,Yaw;
int temp;					//�¶�	
short aacx,aacy,aacz;		//���ٶȴ�����ԭʼ����
short gyrox,gyroy,gyroz;	//������ԭʼ����
int main(void)
{	
	//u8 string[10] = {0};	
	delay_init();
	uart_init(115200);
	LED_Init();

	IIC_Init();
	MPU6050_initialize();     //=====MPU6050��ʼ��	
	DMP_Init();
	
	TIM2_Getsample_Int(1999,719);		//50ms����ʱ�ж�
  
	while(1)
	{	
    
//    temp= Read_Temperature();
//    MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������
//		MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//�õ�����������
//    printf(" \n\r aacx=%.d  temp=%.d  \n\r",aacx,temp);
//    delay_ms(20);
    /*********
		delay_ms(999);
		sprintf((char *)string,"Pitch:%.2f",Pitch);//0300
		
		sprintf((char *)string,"Roll :%.2f",Roll);//0300

		sprintf((char *)string,"Yaw  :%.2f",Yaw);//0300
    *********/
	
  }
}
