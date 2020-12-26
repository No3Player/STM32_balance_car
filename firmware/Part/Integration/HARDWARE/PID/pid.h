#ifndef __PID_H__
#define __PID_H__
#include "sys.h"
typedef struct 
{
	signed int  Sv;			//�û��趨ƽ��λ��ֵ
	float Pv;
					/*ƽ�⻷�����趨 */
	float  Kd;					//ƽ��΢����ϵ��
	signed int  Kp;			//ƽ�������ϵ��
	

	
					/*�ٶȻ������趨 */

	signed int target_speed;//Ŀ���ٶ�
	float  Ki_speed;				//�ٶȻ�������ϵ��
	signed int Kp_speed;		//�ٶȻ�������ϵ��
	signed int Least_speed;  	//�ٶ�ƫ��
	signed int Encoder;   //һ�׵�ͨ�˲�
  signed int Encoder_Integral;   //�ٶȻ�����
  
					/*ת�򻷲����趨 */
	float Kp_turn;					//ת�򻷱�����ϵ��
	float Angle_turn;				//Ŀ��ת��Ƕ�
	
}PID;

int balance(float Angle);
void PID_Init(void);
int velocity(int encoder_left,int encoder_right);
//int turn(float gyro);
void calculation(void);
#endif
