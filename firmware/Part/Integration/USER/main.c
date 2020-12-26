#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "string.h"
#include "led.h"
#include "TIME.h"
#include "mpu6050.h"
#include "motor.h"
#include "pid.h"

//#define IMU_GYRO
//#define IMU_ANGLE

int temp;					//�¶�
short aacx, aacy, aacz;		//���ٶȴ�����ԭʼ����
short gyrox, gyroy, gyroz;	//������ԭʼ����
float Pitch, Roll, Yaw;
extern int Moto1, Moto2;  							//���PWM����
int Encoder_Left, Encoder_Right;      //���ұ��������������

int main(void)
{
    //u8 string[10] = {0};
    delay_init();
    uart_init(115200);
    LED_Init();

    IIC_Init();
    MPU6050_initialize();     //=====MPU6050��ʼ��
    DMP_Init();
    PID_Init();
    TIM3_Config(99, 7199);		//10ms����ʱ�ж�

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    MiniBalance_Motor_Init();
    MiniBalance_PWM_Init(7199, 0);  //��ʼ��PWM 10KHZ������������� ,PWM���7200

    Encoder_Init_TIM2();            //=====�������ӿ�
    Encoder_Init_TIM4();            //=====��ʼ��������2

    while(1)
    {
        delay_ms(10);
        #ifdef IMU_GYRO
        MPU_Get_Gyroscope(&gyrox, &gyroy, &gyroz);	//�õ�����������
        printf("\n\r x=%.d      y=%.d     z=%.d \n\r", gyrox, gyroy, gyroz);
        #endif

        #ifdef IMU_ANGLE
        Read_DMP(&Pitch, &Roll, &Yaw);
        printf("\n\r ������=%.2f      �����=%.2f      ƫ����=%.2f \n\r", Pitch, Roll, Yaw);
        #endif
        //    pv = Roll ;
//    Moto1 = balance(pv) ;		//���PWM���ֵ
//    Moto2 = balance(pv) ;
//    //printf("\n\r moto=%.2d   ",Moto1 );
//    Xianfu_Pwm();																							//��PWM�����޷�
//    Set_Pwm(Moto1, Moto2);  																		//����PWM


    }
}
