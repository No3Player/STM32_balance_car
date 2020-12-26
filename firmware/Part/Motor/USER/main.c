#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "motor.h"


/*
-------------------------------------IO�ڶ�Ӧ���ܱ�-------------------------------------------------------
���������                B12->BIN2       B13->BIN1      B14->AIN1     B15->AIN2
	PWM   ��                A8 ->PWMA                        A11->PWMB
MUP6050 :                 B8 ->SCL                         B9 ->SDA
�����������              B6 B7 һ��                       A0 A1һ��
                    �����ֵΪ�������ٶ����ֵ��60����

*/

u8 stop_flag = 0;    										//С��ֹͣ��־λ
extern int Moto1, Moto2;  							//���PWM����

/**************************************************************************
�������ܣ�������
��ڲ�������
����  ֵ����
**************************************************************************/

int main(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    delay_init();	    						  //��ʱ������ʼ��
    uart_init(9600);	 							//���ڳ�ʼ��Ϊ9600
    LED_Init();		  								//��ʼ����LED���ӵ�Ӳ���ӿ�
    MiniBalance_Motor_Init();
    MiniBalance_PWM_Init(7199, 0);  //��ʼ��PWM 10KHZ������������� ,PWM���7200

    while(1)
    {
        delay_ms(200);
        LED0 = ~LED0;
        Set_Pwm(7199 ,7199);  																	//����PWM

    }
}

