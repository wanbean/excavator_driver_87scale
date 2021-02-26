/*****************************************************************************************************
* @file    Motor_Control.c									     * 
* @author  WanBean										     *
* @version V1.0                                                         			     *
* @date    2020-04-11    									     *
* @brief                                               					             *
******************************************************************************************************/
/* ---------------------------------------Include--------------------------------------------------- */
#include "Motor_Control.h"
/* ---------------------------------------Private typedef ------------------------------------------ */

/* ---------------------------------------Private define ------------------------------------------- */

/* ---------------------------------------Private macro -------------------------------------------- */

/* ---------------------------------------Private variables ---------------------------------------- */
uint16_t Motor_PWMPeriod = 0;
/* ---------------------------------------Private function prototypes  ----------------------------- */

/**
  * @brief  ������ų�ʼ��
  * @param  None
  * @retval None
  */
static void MotorGPIO_Init(void)
{
  //һ�ŵ������ת PWM
  GPIO_Init(MOTOR1_PWM1_PORT, MOTOR1_PWM1_PIN, GPIO_Mode_Out_PP_High_Fast);
  GPIO_Init(MOTOR1_PWM2_PORT, MOTOR1_PWM2_PIN, GPIO_Mode_Out_PP_High_Fast);
}

/**
  * @brief  ���PWM��ʱ����ʼ��
  * @param  None
  * @retval None
  */
static void MotorTIM_Init(uint16_t Period)
{
  //ʹ��TIM2����ʱ���ſ�
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
  
  //TIM2ʱ�����ã�8��Ƶ(0.5usһ����)������50us,Ƶ��20K ���ϼ�����
  TIM2_TimeBaseInit(TIM2_Prescaler_128, TIM2_CounterMode_Up, Period);
  //TIM2 PWM2ģʽ�����ʹ�ܣ�
  TIM2_OC1Init(TIM2_OCMode_PWM2, TIM2_OutputState_Enable, 0,  TIM2_OCPolarity_Low, TIM2_OCIdleState_Set);
  TIM2_OC1PreloadConfig(ENABLE);
  TIM2_OC2Init(TIM2_OCMode_PWM2, TIM2_OutputState_Enable, 0,  TIM2_OCPolarity_Low, TIM2_OCIdleState_Set);
  TIM2_OC2PreloadConfig(ENABLE);
  
  //�Զ�Ԥװ��ʹ��
  TIM2_ARRPreloadConfig(ENABLE);

  //ʹ��TIM2���
  TIM2_CtrlPWMOutputs(ENABLE);
  
  //TIM2������ʹ��
  TIM2_Cmd(ENABLE);
}

/**
  * @brief  �����ʼ��
  * @param  None
  * @retval None
  */
void Motor_Init(uint16_t Period)
{
  MotorGPIO_Init();
  MotorTIM_Init(Period);
}

/**
  * @brief  ���1PWM����
  * @param  Count:�������� Dir:��ת���� Duty:ռ�ձ�0-100
  * @retval None
  */
void Motor1_PWM(int8_t Dir,int16_t Duty)
{
  //�������
  if(Duty > Motor_PWMPeriod) Duty = Motor_PWMPeriod;
  if(Duty < 0) Duty = 0;
  //��ת
  if(Dir == 1)
  {
    TIM2_SetCompare1(Duty);
    TIM2_SetCompare2(0);
  }
  //ֹͣ
  else if(Dir == 0)
  {
    TIM2_SetCompare1(0);
    TIM2_SetCompare2(0);
  }
  //��ת
  else if(Dir == -1)
  {
    TIM2_SetCompare1(0);
    TIM2_SetCompare2(Duty * MOTOR_BackUp_Scale);
  }
}