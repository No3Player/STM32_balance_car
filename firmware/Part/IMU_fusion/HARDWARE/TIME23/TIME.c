#include "TIME.h"

/*********************************************���������ݶ�ȡ��PID����ˢ��ʱ��2***********************************/
void TIM2_Getsample_Int(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��

    TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
    TIM_TimeBaseStructure.TIM_Prescaler = psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    TIM_ITConfig( TIM2, TIM_IT_Update | TIM_IT_Trigger, ENABLE); //ʹ�ܶ�ʱ��2���´����ж�

    TIM_Cmd(TIM2, ENABLE);  //ʹ��TIMx����

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure);
}
//***************TIME2���ж�*******************/
void TIM2_IRQHandler(void)
{
    int temp;					//�¶�
    short aacx, aacy, aacz;		//���ٶȴ�����ԭʼ����
    short gyrox, gyroy, gyroz;	//������ԭʼ����

    if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) //����ж�
    {
//        Read_DMP(&Pitch, &Roll, &Yaw);
//        printf("\n\r ������=%.2f      �����=%.2f      ƫ����=%.2f \n\r", Pitch, Roll, Yaw);

//        temp = Read_Temperature();
//        MPU_Get_Accelerometer(&aacx, &aacy, &aacz);	//�õ����ٶȴ���������
//        MPU_Get_Gyroscope(&gyrox, &gyroy, &gyroz);	//�õ�����������
//        printf("\n\r gyrox=%.d    gyroy=%.d   gyroz=%.d \n\r", gyrox,gyroy, gyroz);

    }

    TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //����жϱ�־λ
}

