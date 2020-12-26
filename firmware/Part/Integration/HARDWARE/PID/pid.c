#include "pid.h"
#include "MPU6050.h"
#include "motor.h"
#include "encoder.h"
PID pid;
void PID_Init()
{
    /*ƽ��PID�����Ʋ�����ʼ��*/
    pid.Sv =  8;
    pid.Kp = 650;
    pid.Kd = -5;

    /*�ٶ�PID�����Ʋ�����ʼ��*/
    pid.target_speed = 0;
    pid.Kp_speed = 100 ;   //max 130 about
    pid.Ki_speed = 0.56;//pid.Kp_speed/200;
    pid.Least_speed = 0;
    pid.Encoder = 0;
    pid.Encoder_Integral = 0;


    /*ת��PID�����Ʋ�����ʼ��*/
    pid.Kp_turn = 35;
    pid.Angle_turn = 0;
}

/*С��ƽ�⻷�s��֣�΢�?��������
΢�ֱ���Ϊֱ�Ӷ�ȡ�ļ��ٶ�*/

int balance(float Angle)
{

    signed  int  Bias;
    int bal;
    extern short gyrox;
    Bias = (Angle - pid.Sv);
    bal = pid.Kp * Bias + gyrox * pid.Kd;
    return bal;
}

/*С���ٶȻ����֣� ����+��������*/
int velocity(int encoder_left, int encoder_right)
{

    static int vel;
    pid.Least_speed =  (encoder_left + encoder_right) - pid.target_speed;
    pid.Encoder *= 0.7;
    pid.Encoder += pid.Least_speed * 0.3;
    pid.Encoder_Integral += pid.Encoder;
    vel = pid.Kp_speed * pid.Encoder + (int)(pid.Ki_speed * pid.Encoder_Integral);
    return vel;
}

void calculation(void)
{
    extern int Encoder_Left, Encoder_Right;      //���ұ��������������
    extern short gyrox, gyroy, gyroz;							//������ԭʼ����
    extern float Pitch, Roll, Yaw; 								//ŷ����
    extern int Moto1, Moto2;  							//���PWM����

    Read_DMP  (&Pitch, &Roll, &Yaw);
    MPU_Get_Gyroscope(&gyrox, &gyroy, &gyroz);									//�õ�����������
    Encoder_Left = Read_Encoder(2);                           //===��ȡ��������ֵ
    Encoder_Right = Read_Encoder(4);

    pid.Pv = Roll ;																				//�Ƕ�*ʮ��
    printf("\n\r ƫ����=%.2f \n\r",  pid.Pv);
    Moto1 = balance(pid.Pv)+ velocity(Encoder_Left, Encoder_Right) ;		//���PWM���ֵ
    Moto2 = balance(pid.Pv)+ velocity(Encoder_Left, Encoder_Right) ;
    //printf("\n\r ƫ����=%.d \n\r", Moto1 );
    //printf(" \n\r  Encoder_Left= %d   Encoder_Right= %d   \n\r ", Encoder_Left,Encoder_Left);
    Xianfu_Pwm();																							//��PWM�����޷�
    Set_Pwm(Moto1, Moto2);  																		//����PWM
}

