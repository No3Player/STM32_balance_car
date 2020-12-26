#include "motor.h"
int Moto1, Moto2; //���PWM����

/**************************************************************************
�������ܣ�����������IO��ʼ��
��ڲ�������
����  ֵ����
**************************************************************************/
void MiniBalance_Motor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //ʹ��PB�˿�ʱ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;	//�˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //50M
    GPIO_Init(GPIOB, &GPIO_InitStructure);					      //�����趨������ʼ��GPIOB
}
/**************************************************************************
�������ܣ��������PWM���IO�ڳ�ʼ��
��ڲ�������
����  ֵ����
**************************************************************************/
void MiniBalance_PWM_Init(u16 arr, u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    MiniBalance_Motor_Init();
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);//
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   //ʹ��GPIO����ʱ��ʹ��
    //���ø�����Ϊ�����������,���TIM1 CH1 CH4��PWM���岨��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11; //TIM_CH1 //TIM_CH4
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler = psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  ����Ƶ
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ


    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_Pulse = 0;                           //���ô�װ�벶��ȽϼĴ���������ֵ
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     //�������:TIM����Ƚϼ��Ը�
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
    TIM_OC4Init(TIM1, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx

    TIM_CtrlPWMOutputs(TIM1, ENABLE);	//MOE �����ʹ��

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH1Ԥװ��ʹ��
    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);  //CH4Ԥװ��ʹ��
    TIM_SetCompare1(TIM1, 2000);
    TIM_SetCompare4(TIM1, 2000);
    TIM_ARRPreloadConfig(TIM1, ENABLE); //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���

    TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1

}
/**************************************************************************
�������ܣ������������ֵ����
��ڲ���������
����  ֵ������ֵ
**************************************************************************/
int myabs(int a)
{
    int temp;

    if(a < 0)  temp = -a;
    else temp = a;

    return temp;
}
/**************************************************************************
�������ܣ��������PWM������������Ƿ�رյ��
��ڲ�������
����  ֵ����
**************************************************************************/
u8 stop_flag;
void Set_Pwm(int moto1, int moto2)
{
    if(stop_flag == 0)
    {
        if(moto1 > 0)			AIN2 = 0,			AIN1 = 1;
        else 	          AIN2 = 1,			AIN1 = 0;

        PWMA = myabs(moto1);

        if(moto2 > 0)	BIN1 = 0,			BIN2 = 1;
        else        BIN1 = 1,			BIN2 = 0;

        PWMB = myabs(moto2);
    }

}
/**************************************************************************
�������ܣ�PWM�޷�����
��ڲ�������
����  ֵ����
**************************************************************************/
void Xianfu_Pwm(void)
{
    int Amplitude = 7000;  //===PWM������7200 ������6900

    if(Moto1 < -Amplitude) Moto1 = -Amplitude;

    if(Moto1 > Amplitude)  Moto1 = Amplitude;

    if(Moto2 < -Amplitude) Moto2 = -Amplitude;

    if(Moto2 > Amplitude)  Moto2 = Amplitude;

}
/**************************************************************************
�������ܣ�����쳣�رպ���
��ڲ������Ƕ�
����  ֵ��1���رգ�0�����ر�
**************************************************************************/
u8 Turn_Off(signed int angle)
{
    u8 temp = 0;

    if(angle < -400 || angle > 400)
    {
        //===��Ǵ���40�ȹرյ��
        temp = 1;                                          //===Flag_Stop��1�رյ��
        AIN1 = 0;
        AIN2 = 0;
        BIN1 = 0;
        BIN2 = 0;
    }

    return temp;
}
