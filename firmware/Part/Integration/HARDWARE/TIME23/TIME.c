#include "TIME.h"
#include "pid.h"
#include "led.h"

/*********************************************���������ݶ�ȡ��PID����ˢ��ʱ��2***********************************/
void TIM3_Config(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

    TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
    TIM_TimeBaseStructure.TIM_Prescaler = psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    TIM_ITConfig( TIM3, TIM_IT_Update | TIM_IT_Trigger, ENABLE); //ʹ�ܶ�ʱ��2���´����ж�

    TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx����

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�0��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�3��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure);
}

////***************TIME2���ж�*******************/
void TIM3_IRQHandler(void)
{

    if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET) //����ж�
    {
          calculation();
//      LED0=!LED0;
    }

    TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //����жϱ�־λ
}

