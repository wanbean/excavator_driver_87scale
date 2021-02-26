/*****************************************************************************************************
* @file    Servo_Control.c									     * 
* @author  WanBean										     *
* @version V1.0                                                         			     *
* @date    2020-04-16    									     *
* @brief                                               					             *
******************************************************************************************************/
/* ---------------------------------------Include--------------------------------------------------- */
#include "Servo_Control.h"
/* ---------------------------------------Private typedef ------------------------------------------ */

/* ---------------------------------------Private define ------------------------------------------- */

/* ---------------------------------------Private macro -------------------------------------------- */

/* ---------------------------------------Private variables ---------------------------------------- */

/* ---------------------------------------Private function prototypes  ----------------------------- */

/**
  * @brief  ��ʱ��3����
  * @param  None
  * @retval None
  */
static void ServoTIM_Config(void)
{
  //ʹ��TIM3����ʱ���ſ�
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE);
  TIM3_DeInit();
  //TIM3ʱ�����ã�1��Ƶ�����ϼ�����
  TIM3_TimeBaseInit(TIM3_Prescaler_8, TIM3_CounterMode_Up,40000);
  //TIM2 PWM2ģʽ�����ʹ�ܣ�
  TIM3_OC1Init(TIM3_OCMode_PWM2, TIM3_OutputState_Enable, 3000,  TIM3_OCPolarity_Low, TIM3_OCIdleState_Set);
  TIM3_OC1PreloadConfig(ENABLE);
  TIM3_OC2Init(TIM3_OCMode_PWM2, TIM3_OutputState_Enable, 3000,  TIM3_OCPolarity_Low, TIM3_OCIdleState_Set);
  TIM3_OC2PreloadConfig(ENABLE);
  
  //�Զ�Ԥװ��ʹ��
  TIM3_ARRPreloadConfig(ENABLE);

  //ʹ��TIM2���
  TIM3_CtrlPWMOutputs(ENABLE);
  TIM3_ITConfig(TIM3_IT_Update,ENABLE);
  //TIM3������ʹ��
  TIM3_Cmd(ENABLE);
  
  //�岶���־
  TIM3_ClearFlag(TIM3_FLAG_Update);
}

/**
  * @brief  ������ų�ʼ��
  * @param  None
  * @retval None
  */
static void ServoGPIO_Config(void)
{
  //һ�Ŷ��PWM
  GPIO_Init(SERVO1_PWM_PORT, SERVO1_PWM_PIN, GPIO_Mode_Out_PP_High_Fast);
  //���Ŷ��PWM
  GPIO_Init(SERVO2_PWM_PORT, SERVO2_PWM_PIN, GPIO_Mode_Out_PP_High_Fast);
}

/**
  * @brief  ������ų�ʼ��
  * @param  None
  * @retval None
  */
void Servo_Init(void)
{
  ServoTIM_Config();
  ServoGPIO_Config();
}

/**
  * @brief  ���1PWM����
  * @param  Count:�������� Pulse:������1000-2000
  * @retval None
  */
void Servo1_PWM(uint32_t Count,uint16_t Pulse)
{
  uint16_t Count_Temp = 0;
  Count_Temp = Count%400;//���� = 50us*400=20ms

  if(Count_Temp < (Pulse/50))
  {
    //�ߵ�ƽ
    GPIO_SetBits(SERVO1_PWM_PORT,SERVO1_PWM_PIN);
  }
  else
  {
    //�͵�ƽ
    GPIO_ResetBits(SERVO1_PWM_PORT,SERVO1_PWM_PIN);
  }
}
//void Servo1_PWM(uint32_t Count,uint8_t Pulse)
//{
////  uint16_t Count_Temp = 0;
////  Count_Temp = Count%2000;//���� = 10us*2000=20ms
//  
//  if(Count < Pulse)
//  {
//    //�ߵ�ƽ
//    GPIO_WriteBit(SERVO1_PWM_PORT,SERVO1_PWM_PIN,SET);
//    //printf("H\r\n");
//  }
//  else
//  {
//    //�͵�ƽ
//    GPIO_WriteBit(SERVO1_PWM_PORT,SERVO1_PWM_PIN,RESET);
//    //printf("H\r\n");
//  }
//}

/**
  * @brief  ���2PWM����
  * @param  Count:�������� Pulse:������100-200
  * @retval None
  */
void Servo2_PWM(uint32_t Count,uint16_t Pulse)
{
{
  uint16_t Count_Temp = 0;
  Count_Temp = Count%400;//���� = 50us*400=20ms

  if(Count_Temp < (Pulse/50))
  {
    //�ߵ�ƽ
    GPIO_SetBits(SERVO2_PWM_PORT,SERVO2_PWM_PIN);
  }
  else
  {
    //�͵�ƽ
    GPIO_ResetBits(SERVO2_PWM_PORT,SERVO2_PWM_PIN);
  }
}
}