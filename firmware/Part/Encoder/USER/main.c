#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "led.h"



/*
-------------------------------------IO�ڶ�Ӧ���ܱ�-------------------------------------------------------
���������                B12->BIN2       B13->BIN1      B14->AIN1     B15->AIN2
	PWM   ��                A8 ->PWMA                        A11->PWMB
MUP6050 :                 B8 ->SCL                         B9 ->SDA
�����������              B6 B7 һ��                       A0 A1һ��
                    �����ֵΪ�������ٶ����ֵ��60����

*/

int Encoder_Left, Encoder_Right;      //���ұ��������������

/**************************************************************************
�������ܣ�������
��ڲ�������
����  ֵ����
**************************************************************************/

int main(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    delay_init();	    						  //��ʱ������ʼ��
    uart_init(115200);	 							//���ڳ�ʼ��Ϊ9600
    LED_Init();		  								//��ʼ����LED���ӵ�Ӳ���ӿ�

    Encoder_Init_TIM2();            //=====�������ӿ�
    Encoder_Init_TIM4();            //=====��ʼ��������2

    while(1)
    {
        Encoder_Left = Read_Encoder(2);                           //===��ȡ��������ֵ
        Encoder_Right = Read_Encoder(4);
        printf(" \n\r  Encoder_Left= %d   \n\r ",Encoder_Left);
        delay_ms(200);
        LED0 =!LED0;

    }
}

