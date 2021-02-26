/*****************************************************************************************************
* @file    Receiver_PPM.c									     * 
* @author  WanBean										     *
* @version V1.0                                                         			     *
* @date    2020-03-27    									     *
* @brief                                               					             *
******************************************************************************************************/
/* ---------------------------------------Include--------------------------------------------------- */
#include "Receiver_PWM.h"
#include "Light_Control.h"
#include "stm8l10x_usart.h"
#include "stm8l10x_tim3.h"
#include "stm8l10x_gpio.h"
#include "stm8l10x_flash.h"
/* ---------------------------------------Private typedef ------------------------------------------ */

/* ---------------------------------------Private define ------------------------------------------- */
#define TIM_Period 65535
/* ---------------------------------------Private macro -------------------------------------------- */

/* ---------------------------------------Private variables ---------------------------------------- */
PWM_CURRENTDATA PWM_CurrentData = {3,2000,2000,1520,1520,1000,1000,1520,1520,0,0,0};
CONTROL_DATA Control_Data;
/* ---------------------------------------Private function prototypes  ----------------------------- */
static uint16_t (* GetCapture)(void);

/**
  * @brief  PWM�����ʼ��
  * @param  None
  * @retval None
  */
void PWM_Reveiver_Init(void)
{
  //ʹ��TIM3����ʱ���ſ�
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE);
    
  //����TIM3 ͨ��1--PB1��GPIO�� �������벻���ж�
  GPIO_Init(GPIOB, GPIO_Pin_1, GPIO_Mode_In_PU_No_IT);
  //����TIM3 ͨ��2--PD0��GPIO�� �������벻���ж�
  GPIO_Init(GPIOD, GPIO_Pin_0, GPIO_Mode_In_PU_No_IT);
  //TIM2_TimeBaseInit(TIM2_Prescaler_16, TIM2_CounterMode_Up,59999);
  TIM3_PrescalerConfig(TIM3_Prescaler_16,TIM3_PSCReloadMode_Immediate);//TIM2_PSCReloadMode_Update
  //�Զ�Ԥװֵ
  TIM3_SetAutoreload(65535);
  //�Զ�Ԥװ��ʹ��
  TIM3_ARRPreloadConfig(ENABLE);

  /* Capture only every 8 events!!! */
  //����TIM3���벶��ͨ��1���½��ش��������ӵ���ʱ������ͨ��1 ���˲���
  TIM3_ICInit(TIM3_Channel_1, TIM3_ICPolarity_Rising, TIM3_ICSelection_DirectTI, TIM3_ICPSC_Div1, 0);
  //����TIM3���벶��ͨ��2���½��ش��������ӵ���ʱ������ͨ��2 ���˲���
  TIM3_ICInit(TIM3_Channel_2, TIM3_ICPolarity_Rising, TIM3_ICSelection_DirectTI, TIM3_ICPSC_Div1, 0);
  //ʹ��TIM2����
  TIM3_Cmd(ENABLE);
  //�����ж�
  TIM3_ITConfig(TIM3_IT_CC1,ENABLE);
  TIM3_ITConfig(TIM3_IT_CC2,ENABLE);
  //�岶���־
  TIM3_ClearFlag(TIM3_FLAG_CC1);
  TIM3_ClearFlag(TIM3_FLAG_CC2);
}

/**
  * @brief  PWMͨ��1����
  * @param  None
  * @retval None
  */
void PWM1_Decode(void)
{
  static uint32_t ICValue1,ICValue2;
  static uint16_t PWM_DataTemp;
  GetCapture = TIM3_GetCapture1;
  
  if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) != RESET)
  {
    ICValue1 = GetCapture();
    if(ICValue1 < 0)
    {
      ICValue1 = 0;
      //PWM_Ready = 0;
      return;
    }
    TIM3_ICInit(TIM3_Channel_1, TIM3_ICPolarity_Falling, TIM3_ICSelection_DirectTI, TIM3_ICPSC_Div1, 0);
  }
  else
  {
    ICValue2 = GetCapture();
    if(ICValue2 < 0)
    {
      ICValue2 = 0;
      //PWM_Ready = 0;
      return;
    }
    TIM3_ICInit(TIM3_Channel_1, TIM3_ICPolarity_Rising, TIM3_ICSelection_DirectTI, TIM3_ICPSC_Div1, 0);
    if(ICValue2 < ICValue1)
    {
      ICValue2 = ICValue1 + TIM_Period;
    }
    PWM_DataTemp = ICValue2 - ICValue1;
    if((PWM_DataTemp > 1000)&&(PWM_DataTemp < 2000))
    {
     PWM_CurrentData.PWM_Data[0] = PWM_DataTemp;
    }
    else
    {
      ICValue1 = 0;
      ICValue2 = 0;
      PWM_DataTemp = 0;
      //PWM_Ready = 0;
    }
  }
}

/**
  * @brief  PWMͨ��2����
  * @param  None
  * @retval None
  */
void PWM2_Decode(void)
{
  static uint32_t ICValue1,ICValue2;
  static uint16_t PWM_DataTemp;
  GetCapture = TIM3_GetCapture2;
  
  if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_0) != RESET)
  {
    ICValue1 = GetCapture();
    if(ICValue1 < 0)
    {
      ICValue1 = 0;
      //PWM_Ready = 0;
      return;
    }
    TIM3_ICInit(TIM3_Channel_2, TIM3_ICPolarity_Falling, TIM3_ICSelection_DirectTI, TIM3_ICPSC_Div1, 0);
  }
  else
  {
    ICValue2 = GetCapture();
    if(ICValue2 < 0)
    {
      ICValue2 = 0;
      //PWM_Ready = 0;
      return;
    }
    TIM3_ICInit(TIM3_Channel_2, TIM3_ICPolarity_Rising, TIM3_ICSelection_DirectTI, TIM3_ICPSC_Div1, 0);
    if(ICValue2 < ICValue1)
    {
      ICValue2 = ICValue1 + TIM_Period;
    }
    PWM_DataTemp = ICValue2 - ICValue1;
    if((PWM_DataTemp > 1000)&&(PWM_DataTemp < 2000))
    {
      PWM_CurrentData.PWM_Data[1] = PWM_DataTemp;
      //��ͨ�����ɼ���,��ʼ����
      PWM_Process();
    }
    else
    {
      ICValue1 = 0;
      ICValue2 = 0;
      PWM_DataTemp = 0;
      //PWM_Ready = 0;
    }
  }
}

/**
  * @brief  ��ȡFlash�е�PWMֵ
  * @param  None
  * @retval None
  */
static void Read_Flash_PWM(void)
{
  uint8_t Val_H = 0x0,Val_L = 0x0;
  
  FLASH_SetProgrammingTime(FLASH_ProgramTime_Standard);//��׼���ʱ��
  FLASH_Unlock(FLASH_MemType_Program);  //�������
  while (FLASH_GetFlagStatus(FLASH_FLAG_PUL) == RESET);
  FLASH_Unlock(FLASH_MemType_Data);  //�������
  while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET);
  
  /* ���Flash����ֵ */
  //��ȡPWM1_Min Flash�б��������
  Val_H = FLASH_ReadByte(PWM1_MINAdd);
  Val_L = FLASH_ReadByte(PWM1_MINAdd + 1);
  PWM_CurrentData.PWM_Min[0] = (uint16_t)(Val_H << 8) + Val_L;

  
  //��ȡPWM1_Mid Flash�б��������
  Val_H = FLASH_ReadByte(PWM1_MIDAdd);
  Val_L = FLASH_ReadByte(PWM1_MIDAdd + 1);
  PWM_CurrentData.PWM_Centre[0] = (uint16_t)(Val_H << 8) + Val_L;
  
  //��ȡPWM1_Max Flash�б��������
  Val_H = FLASH_ReadByte(PWM1_MAXAdd);
  Val_L = FLASH_ReadByte(PWM1_MAXAdd + 1);
  PWM_CurrentData.PWM_Max[0] = (uint16_t)(Val_H << 8) + Val_L;
  
  //��ȡPWM1_Max Flash�б��������
  Val_H = FLASH_ReadByte(PWM2_MIDAdd);
  Val_L = FLASH_ReadByte(PWM2_MIDAdd + 1);
  PWM_CurrentData.PWM_Centre[1] = (uint16_t)(Val_H << 8) + Val_L;
}

/**
  * @brief  PWMֵд��Flash
  * @param  None
  * @retval None
  */
static void Write_Flash_PWM1Max(uint16_t Data)
{
  //д���ֵ
  FLASH_ProgramByte(PWM1_MAXAdd,BYTE_1(Data));
  while (FLASH_GetFlagStatus(FLASH_FLAG_EOP) == RESET);
  FLASH_ProgramByte(PWM1_MAXAdd + 1,BYTE_0(Data));
  while (FLASH_GetFlagStatus(FLASH_FLAG_EOP) == RESET);
}

/**
  * @brief  PWMֵд��Flash
  * @param  None
  * @retval None
  */
static void Write_Flash_PWM1Mid(uint16_t Data)
{
  //д��ֵ
  FLASH_ProgramByte(PWM1_MIDAdd,BYTE_1(Data));
  while (FLASH_GetFlagStatus(FLASH_FLAG_EOP) == RESET);
  FLASH_ProgramByte(PWM1_MIDAdd + 1,BYTE_0(Data));
  while (FLASH_GetFlagStatus(FLASH_FLAG_EOP) == RESET);
}

/**
  * @brief  PWMֵд��Flash
  * @param  None
  * @retval None
  */
static void Write_Flash_PWM1Min(uint16_t Data)
{
  //д��Сֵ
  FLASH_ProgramByte(PWM1_MINAdd,BYTE_1(Data));
  while (FLASH_GetFlagStatus(FLASH_FLAG_EOP) == RESET);
  FLASH_ProgramByte(PWM1_MINAdd + 1,BYTE_0(Data));
  while (FLASH_GetFlagStatus(FLASH_FLAG_EOP) == RESET);
  //д��ֵ
}

/**
  * @brief  д�����PWM��ֵ
  * @param  None
  * @retval None
  */
static void Write_Flash_PWM2Mid(uint16_t Data)
{
  //д��Сֵ
  FLASH_ProgramByte(PWM2_MIDAdd,BYTE_1(Data));
  while (FLASH_GetFlagStatus(FLASH_FLAG_EOP) == RESET);
  FLASH_ProgramByte(PWM2_MIDAdd + 1,BYTE_0(Data));
  while (FLASH_GetFlagStatus(FLASH_FLAG_EOP) == RESET);
  //д��ֵ
}


/**
  * @brief  PWM�����ʼ��
  * @param  None
  * @retval None
  */
int16_t ABS(int16_t Data)
{
  if(Data < 0) Data = -Data;
  return Data;
}





/**
  * @brief  PWM��ֵ�ɼ�
  * @param  PWM_Current:��ǰ�ɼ���PWMֵ
  * @retval None
  */
static void PWM_AVG_Calculate(uint16_t *PWM_AVG,uint16_t *PWM_Current)
{
  static uint8_t CentreCaptureStatus = 0;//0-4:�ɼ� 5:����ƽ��ֵ 6:ready
  static uint16_t PWM_CentreTemp,Last_PWM_Current;
  //ҡ�����������ֲ���1S
  if(ABS(Last_PWM_Current - *PWM_Current) < (DeadZone/2))
  {
    CentreCaptureStatus ++;
  }
  else
  {
    CentreCaptureStatus = 0;
    PWM_CentreTemp = 0;
  }
  //ҡ�˲���2S��ʼ�ɼ�
  if(CentreCaptureStatus > 40)
  {
    PWM_CentreTemp += *PWM_Current;
  }
  //�ɼ�10��
  if(CentreCaptureStatus == 50)
  {
    *PWM_AVG = PWM_CentreTemp/10;
    CentreCaptureStatus = 0;
    PWM_CentreTemp = 0;
    Last_PWM_Current = 0;
    //������ֵ �粻�Ϸ�,���²ɼ�
    if(PWM_CurrentData.PWM_Ready == 0)
    {
      //������ ���߳���2000
      if((*PWM_AVG < PWM_CentreRef + SwitchThreshold)||(*PWM_AVG > PWM_MaxRef))
      {
        PWM_CurrentData.PWM_Ready = 0;
        return;
      }
      //�Ϸ�,���浽Flash
      else
      {
        Write_Flash_PWM1Max(*PWM_AVG);
      }
    }
    //�����ֵ �粻�Ϸ�,���²ɼ�
    else if(PWM_CurrentData.PWM_Ready == 1)
    {
      if(ABS(*PWM_AVG - PWM_CentreRef) > 20)
      {
        PWM_CurrentData.PWM_Ready = 1;
        return;
      }
      //�Ϸ�,���浽Flash
      else
      {
        //д��������ֵ
        Write_Flash_PWM1Mid(*PWM_AVG);
        //д�������ֵ
        
        Write_Flash_PWM2Mid(PWM_CurrentData.PWM_Data[1]);
      }
    }
    //�����Сֵ �粻�Ϸ�,���²ɼ�
    else if(PWM_CurrentData.PWM_Ready == 2)
    {
      //����С ����С��2000
      if((*PWM_AVG > PWM_CentreRef - SwitchThreshold)||(*PWM_AVG < PWM_MinRef))
      {
        PWM_CurrentData.PWM_Ready = 2;
        return;
      }
      //�Ϸ�,���浽Flash
      else
      {
        Write_Flash_PWM1Min(*PWM_AVG);
      }
    }
    PWM_CurrentData.PWM_Ready ++;
  }
  Last_PWM_Current = *PWM_Current;
}

/**
  * @brief  �ϵ�PWM��Сֵ ��ֵ ���ֵУ׼
  * @param  None
  * @retval None
  */
static void PWM_Calibration(uint16_t *PWM_Current)
{
  static uint8_t CentreCaptureStatus = 0;
  static uint16_t Last_PWM_Current = 0;
  
  /* ���Flash����ֵ */
  Read_Flash_PWM();
  //�ж��ĸ�ֵ��ҪУ׼
  if(PWM_CurrentData.PWM_Min[0] == 0)
  {
    PWM_CurrentData.PWM_Ready = 2;//У׼��Сֵ
  }
  if(PWM_CurrentData.PWM_Centre[0] == 0)
  {
    PWM_CurrentData.PWM_Ready = 1;//У׼��ֵ
  }
  if(PWM_CurrentData.PWM_Max[0] == 0)
  {
    PWM_CurrentData.PWM_Ready = 0;//У׼���ֵ
  }
  /* ��ҪУ׼ */
  if(PWM_CurrentData.PWM_Ready != 3)
  {
    return;
  }
  /* ���Flash����ֵ */
  else//Flash�е�ֵ��ȷ
  {
    /* �ж�����λ�� */
    //ҡ�����������ֲ���1S
    if(ABS(Last_PWM_Current - *PWM_Current) < DeadZone)
    {
      CentreCaptureStatus ++;
    }
    else
    {
      CentreCaptureStatus = 0;
    }
    //ҡ�˲���1S��ʼ�ж�����λ��
    if(CentreCaptureStatus == 20)
    {
      //������� ����У׼
      if(*PWM_Current > PWM_CurrentData.PWM_Centre[0] + SwitchThreshold)
      {
        PWM_CurrentData.PWM_Ready = 0;
      }
      //��������λ ����ִ��
      else if(ABS(*PWM_Current - PWM_CurrentData.PWM_Centre[0]) < (2 * DeadZone))
      {
        PWM_CurrentData.PWM_Ready = 4;
      }
    }
    Last_PWM_Current = *PWM_Current;
  }
  /* �ж�����λ�� */
}

///**
//  * @brief  PWM�е�ɼ�
//  * @param  PWM_Current:��ǰ�ɼ���PWMֵ
//  * @retval None
//  */
//static void PWM_CentreCalculate(uint16_t *PWM_Current)
//{
//  static uint8_t CentreCaptureStatus = 0;//0-4:�ɼ� 5:����ƽ��ֵ 6:ready
//  static uint16_t PWM1_CentreTemp,PWM2_CentreTemp;
//  //������ǰ��βɼ�Ϊ��ֵ
//  if(CentreCaptureStatus < 5)
//  {
//    PWM1_CentreTemp += *(PWM_Current);
//    PWM2_CentreTemp += *(PWM_Current+1);
//    CentreCaptureStatus++;
//
//  }
//  //������ֵ
//  if(CentreCaptureStatus == 5)
//  {
//    PWM_CurrentData.PWM_Centre[0] = PWM1_CentreTemp/5;
//    PWM_CurrentData.PWM_Centre[1] = PWM2_CentreTemp/5;
//    CentreCaptureStatus = 6;
//    PWM_CurrentData.PWM_Ready = 1;
//    //������ֵ����ʧ��
//    if((ABS(PWM_CurrentData.PWM_Centre[0] - PWM_CentreRef) > 20)||(ABS(PWM_CurrentData.PWM_Centre[1] - PWM_CentreRef) > 20))
//    {
//      CentreCaptureStatus = 0;
//      PWM_CurrentData.PWM_Ready = 0;
//      PWM1_CentreTemp = 0;
//      PWM2_CentreTemp = 0;
//    }
//
//  }
//}

/**
  * @brief  ����ͨ������
  * @param  Throttle_Data:���ջ�����PWMֵ
  * @retval None
  */
static void Throttle_Analysis(uint16_t Throttle_Data)
{
  static uint8_t LastThrottle_Dir;
  //ǰ��
  if(Throttle_Data > (PWM_CurrentData.PWM_Centre[0] + DeadZone))
  {
    Control_Data.Throttle_Dir = 1;
    //�رյ�����
    Lamp_Mode.BackUp_Lame_Mode = Backup_Lamp_OFF;
    //ѡ��ɲ����ģʽ
    if(Lamp_Mode.Head_Lamp_Mode != Head_Lamp_OFF)
    {
      Lamp_Mode.Stop_Lamp_Mode = Stop_Lamp_Normal;
    }
    else
    {
      Lamp_Mode.Stop_Lamp_Mode = Stop_Lamp_OFF;
    }
    Stop_Lamp_Delay = 0;
  }
  //����
  else if(Throttle_Data < (PWM_CurrentData.PWM_Centre[0] - DeadZone))
  {
    Control_Data.Throttle_Dir = -1;
    //����������
    Lamp_Mode.BackUp_Lame_Mode = Backup_Lamp_Normal;
    //ѡ��ɲ����ģʽ
    if(Lamp_Mode.Head_Lamp_Mode != Head_Lamp_OFF)
    {
      Lamp_Mode.Stop_Lamp_Mode = Stop_Lamp_Normal;
    }
    else
    {
      Lamp_Mode.Stop_Lamp_Mode = Stop_Lamp_OFF;
    }
    Stop_Lamp_Delay = 0;
  }
  else//ɲ��״̬
  {
    Control_Data.Throttle_Dir = 0;
    //�رյ�����
    Lamp_Mode.BackUp_Lame_Mode = Backup_Lamp_OFF;
    //ѡ��ɲ����ģʽ
    if((Lamp_Mode.Head_Lamp_Mode != Head_Lamp_OFF)&&(Lamp_Mode.Stop_Lamp_Mode != Stop_Lamp_Brake))
    {
      Lamp_Mode.Stop_Lamp_Mode = Stop_Lamp_Normal;
    }
    if(LastThrottle_Dir != 0)
    {
      //ɲ���Ʊ�־��λ
      Lamp_Mode.Stop_Lamp_Mode = Stop_Lamp_Brake;     
    }

  }
  LastThrottle_Dir = Control_Data.Throttle_Dir;
  Control_Data.Throttle_Magnitude = ABS(Throttle_Data - PWM_CurrentData.PWM_Centre[0]);
}

/**
  * @brief  ����ͨ������
  * @param  Light_Data:���ջ�����PWMֵ
  * @retval None
  */
static void Light_Analysis(uint16_t Light_Data)
{
  static uint8_t SwitchH_Click,SwitchL_Click;
  static uint16_t LightPWMH_Count,LightPWMM_Count,LightPWML_Count;
  //���ο�������
  if(Light_Data > PWM_CurrentData.PWM_Centre[1] + SwitchThreshold)
  {
    Control_Data.Light_Status = 1;
  }
  //���ο�������
  else if(Light_Data < PWM_CurrentData.PWM_Centre[1] - SwitchThreshold)
  {
    Control_Data.Light_Status = -1;
  }
  //���ο�����λ
  else
  {
    Control_Data.Light_Status = 0;
  }
  
  if(Control_Data.Light_Status == -1)
  {
    LightPWMH_Count = 0;
    LightPWMM_Count = 0;
    LightPWML_Count ++;
    if(SwitchL_Click == 0) SwitchL_Click = 1;
    if(SwitchL_Click == 2) SwitchL_Click = 3;
    //���³���
    if(LightPWML_Count > 25)//����1000ms,��ת��
    {
      Lamp_Mode.Turning_Signal_Lamp_Mode = Turning_Signal_Turn_Left;
      SwitchL_Click = 0;
      //��������
      LightPWML_Count = 26;
    }
  }
  else if(Control_Data.Light_Status == 0)
  {
    LightPWMH_Count = 0;
    LightPWMM_Count ++;
    LightPWML_Count = 0;
    if(Lamp_Mode.Turning_Signal_Lamp_Mode != Turning_Signal_Beacon)
    {
      Lamp_Mode.Turning_Signal_Lamp_Mode = Turning_Signal_OFF;
    }
    //����
    if(LightPWMM_Count > 25)
    {
      //�� ��ʾ��
      if(SwitchL_Click == 2)
      {
        if(Lamp_Mode.Turning_Signal_Lamp_Mode != Turning_Signal_Beacon)
        {
          Lamp_Mode.Turning_Signal_Lamp_Mode = Turning_Signal_Beacon;
        }
        else
        {
          Lamp_Mode.Turning_Signal_Lamp_Mode = Turning_Signal_OFF;
        }
      }
      //�� ���
      if(SwitchH_Click == 2)
      {
        if(Lamp_Mode.Head_Lamp_Mode == Head_Lamp_OFF)
        {
          Lamp_Mode.Head_Lamp_Mode = Head_Lamp_LB;
//          Lamp_Mode.Stop_Lamp_Mode = Stop_Lamp_Normal;
        }
        else if(Lamp_Mode.Head_Lamp_Mode == Head_Lamp_LB)
        {
          Lamp_Mode.Head_Lamp_Mode = Head_Lamp_HB;
//          Lamp_Mode.Stop_Lamp_Mode = Stop_Lamp_Normal;
        }
        else
        {
          Lamp_Mode.Head_Lamp_Mode = Head_Lamp_OFF;
          Lamp_Mode.Stop_Lamp_Mode = Stop_Lamp_OFF;
        }
      }
      SwitchL_Click = 0;
      SwitchH_Click = 0;
    }
    if(SwitchL_Click == 1) SwitchL_Click = 2;
    if(SwitchH_Click == 1) SwitchH_Click = 2;
    //˫��
    if(SwitchL_Click == 3)
    {
      SwitchL_Click = 0;
      //����ģʽ ѭ���л�
      if(Lamp_Mode.Police_Lamp_Mode == Police_Lamp_OFF)
      {
        Lamp_Mode.Police_Lamp_Mode = Police_Lamp_M1;
      }
      else if(Lamp_Mode.Police_Lamp_Mode == Police_Lamp_M1)
      {
        Lamp_Mode.Police_Lamp_Mode = Police_Lamp_M2;
      }
      else if(Lamp_Mode.Police_Lamp_Mode == Police_Lamp_M2)
      {
        Lamp_Mode.Police_Lamp_Mode = Police_Lamp_M3;
      }
      else if(Lamp_Mode.Police_Lamp_Mode == Police_Lamp_M3)
      {
        Lamp_Mode.Police_Lamp_Mode = Police_Lamp_OFF;
      }
    }
    //˫��
    if(SwitchH_Click == 3)
    {
      SwitchH_Click = 0;
    }
    
    //���Ƽ���
    if(LightPWMM_Count > 25)
    {
      LightPWMM_Count = 26;
    }
  }
  else
  {
    LightPWMH_Count ++;
    LightPWMM_Count = 0;
    LightPWML_Count = 0;
    if(SwitchH_Click == 0) SwitchH_Click = 1;
    if(SwitchH_Click == 2) SwitchH_Click = 3;
    //���ϳ���
    if(LightPWMH_Count > 25)//����1000ms,��ת��
    {
      Lamp_Mode.Turning_Signal_Lamp_Mode = Turning_Signal_Turn_Right;
      SwitchH_Click = 0;
      //��������
      LightPWMH_Count = 26;
    }
  }
}

/**
  * @brief  PWM���ݴ���
  * @param  None
  * @retval None
  */
void PWM_Process(void)
{
  PWM_CurrentData.Fail_Safe = 0;
  switch (PWM_CurrentData.PWM_Ready)
  {
    case 0://�ɼ����ֵ
      PWM_AVG_Calculate(&PWM_CurrentData.PWM_Max[0],&PWM_CurrentData.PWM_Data[0]);
      break;
    case 1://�ɼ���ֵ
      PWM_AVG_Calculate(&PWM_CurrentData.PWM_Centre[0],&PWM_CurrentData.PWM_Data[0]);
      break;
    case 2://�ɼ���Сֵ
      PWM_AVG_Calculate(&PWM_CurrentData.PWM_Min[0],&PWM_CurrentData.PWM_Data[0]);
      break;
    case 3://���Flash �ж�����λ��
      PWM_Calibration(&PWM_CurrentData.PWM_Data[0]);
      //ȡ��Сֵ��Ϊ��ʱ������
      if((PWM_CurrentData.PWM_Max[0] - PWM_CurrentData.PWM_Centre[0]) < (PWM_CurrentData.PWM_Centre[0] - PWM_CurrentData.PWM_Min[0]))
      {
        Motor_PWMPeriod = ABS(PWM_CurrentData.PWM_Max[0] - PWM_CurrentData.PWM_Centre[0]);
      }
      else
      {
        Motor_PWMPeriod = ABS(PWM_CurrentData.PWM_Min[0] - PWM_CurrentData.PWM_Centre[0]);
      }
      Motor_Init(Motor_PWMPeriod - 1);
      break;
    case 4://����ִ��
      //����״̬ת��
      Throttle_Analysis(PWM_CurrentData.PWM_Data[0]);
      //����״̬ת��
      Light_Analysis(PWM_CurrentData.PWM_Data[1]);
      break;
    default:
      
      break;
  }
  
//  //������ֵ
//  PWM_CentreCalculate(PWM_CurrentData.PWM_Data);
//  if(PWM_CurrentData.PWM_Ready == 1)
//  {
//    //����״̬ת��
//    Throttle_Analysis(PWM_CurrentData.PWM_Data[0]);
//    //����״̬ת��
//    Light_Analysis(PWM_CurrentData.PWM_Data[1]);
//  }

}






























