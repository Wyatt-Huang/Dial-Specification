/**********************************************************************************************************
 * @文件     can.c
 * @说明     can通信配置
 * @版本  	 V1.0
 * @作者     hzx
 * @日期     2019.10
**********************************************************************************************************/
#include "main.h"


/**********************************************************************************************************
*函 数 名: CAN_Config
*功能说明: can初始化
*形    参: 无
*返 回 值: 无
**********************************************************************************************************/
void CAN_Config(void)
{
	  CAN_InitTypeDef        can;              //新板子CAN1
    CAN_FilterInitTypeDef  can_filter;
    GPIO_InitTypeDef       gpio;
    NVIC_InitTypeDef       nvic;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

		GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_CAN1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_CAN1);

    gpio.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    gpio.GPIO_Mode = GPIO_Mode_AF;
	  gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
	  gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &gpio);

    nvic.NVIC_IRQChannel = CAN1_RX0_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 2;
    nvic.NVIC_IRQChannelSubPriority = 2;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
		
		nvic.NVIC_IRQChannel = CAN1_RX1_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = 1;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
    
    CAN_DeInit(CAN1);
    CAN_StructInit(&can);
    
    can.CAN_TTCM = DISABLE;
    can.CAN_ABOM = ENABLE;
    can.CAN_AWUM = ENABLE;
    can.CAN_NART = ENABLE;
    can.CAN_RFLM = DISABLE;
    can.CAN_TXFP = ENABLE;
    can.CAN_Mode = CAN_Mode_Normal;
                                      // can.CAN_Mode =CAN_Mode_LoopBack
    can.CAN_SJW  = CAN_SJW_1tq;
    can.CAN_BS1 = CAN_BS1_9tq;
    can.CAN_BS2 = CAN_BS2_4tq;
		can.CAN_Prescaler = 3;   //CAN BaudRate 42/(1+9+4)/3=1Mbps
    CAN_Init(CAN1, &can);


    can_filter.CAN_FilterNumber=0;
		can_filter.CAN_FilterMode=CAN_FilterMode_IdMask;
		can_filter.CAN_FilterScale=CAN_FilterScale_32bit;
		can_filter.CAN_FilterIdHigh=0x0000;
		can_filter.CAN_FilterIdLow=0x0000;
		can_filter.CAN_FilterMaskIdHigh=0x0000;
		can_filter.CAN_FilterMaskIdLow=0x0000;
		can_filter.CAN_FilterFIFOAssignment=0;
		can_filter.CAN_FilterActivation=ENABLE;
		CAN_FilterInit(&can_filter);
		
		can_filter.CAN_FilterNumber = 1; //选择过滤器1
		can_filter.CAN_FilterMode = CAN_FilterMode_IdList; //列表模式
		can_filter.CAN_FilterScale = CAN_FilterScale_32bit;
		can_filter.CAN_FilterIdHigh = 0x100<<5;
		can_filter.CAN_FilterIdLow =  0 | CAN_ID_STD;
		can_filter.CAN_FilterMaskIdHigh = 0x205<<5;  
		can_filter.CAN_FilterMaskIdLow = 0 | CAN_ID_STD;
		can_filter.CAN_FilterFIFOAssignment = 1;//fifo
		can_filter.CAN_FilterActivation = ENABLE;
		CAN_FilterInit(&can_filter);
    
		CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);
   CAN_ITConfig(CAN1,CAN_IT_FMP1,ENABLE);
    CAN_ITConfig(CAN1,CAN_IT_TME,ENABLE); 
}


/**********************************************************************************************************
*函 数 名: trigger_F105_CurSend
*功能说明: F105 can通信
*形    参: 发送拨弹电机电流值
*返 回 值: 无
**********************************************************************************************************/
void trigger_F105_CurSend(short a)
{
    CanTxMsg tx_message;
    tx_message.IDE = CAN_ID_STD;    
    tx_message.RTR = CAN_RTR_DATA; 
    tx_message.DLC = 0x08;    
    tx_message.StdId = 0x1ff;
	  a=LIMIT_MAX_MIN(a,8000,-8000);
    tx_message.Data[0] = (unsigned char)((a>>8)&0xff);
    tx_message.Data[1] = (unsigned char)(a&0xff);  
    CAN_Transmit(CAN1,&tx_message);
}

/**********************************************************************************************************
*函 数 名: CAN_Set
*功能说明: 底盘can1通信
*形    参: 发送底盘电机电流值
*返 回 值: 无
**********************************************************************************************************/
void CAN_Set(int a,int b,int c,int d)
{
	CanTxMsg CAN_TX_DATA;
	
//	CAN_TX_DATA.StdId=0x1FF;
	CAN_TX_DATA.StdId=0x200;
	CAN_TX_DATA.RTR=CAN_RTR_DATA;
	CAN_TX_DATA.IDE=CAN_ID_STD;
	CAN_TX_DATA.DLC=0x08;
	a=LIMIT_MAX_MIN(a,16000,-16000);
	b=LIMIT_MAX_MIN(b,16000,-16000);
	c=LIMIT_MAX_MIN(c,16000,-16000);
	d=LIMIT_MAX_MIN(d,16000,-16000);
	CAN_TX_DATA.Data[0]=(unsigned char)((a>> 8) & 0xff);
	CAN_TX_DATA.Data[1]=(unsigned char)(a& 0xff);
	CAN_TX_DATA.Data[2]=(unsigned char)((b>> 8) & 0xff);
	CAN_TX_DATA.Data[3]=(unsigned char)(b& 0xff);
	CAN_TX_DATA.Data[4]=(unsigned char)((c>> 8) & 0xff);
	CAN_TX_DATA.Data[5]=(unsigned char)(c& 0xff);
	CAN_TX_DATA.Data[6]=(unsigned char)((d>> 8) & 0xff);
	CAN_TX_DATA.Data[7]=(unsigned char)(d& 0xff);
	
	CAN_Transmit(CAN1,&CAN_TX_DATA);
}

void  CAN1_RX0_IRQHandler(void)
{
    CanRxMsg rx_message;
    
    if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET)
		{
        CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
        CAN_Receive(CAN1, CAN_FIFO0, &rx_message);
			  CAN1_Receive0Task(rx_message);
    }
}


/**
  * @brief  CAN2接收中断  F105
  * @param  None
  * @retval None
  */
void CAN1_RX1_IRQHandler(void)
{
	  CanRxMsg rx_message1;
    if (CAN_GetITStatus(CAN1,CAN_IT_FMP1)!= RESET)
    {
			  CAN_ClearITPendingBit(CAN1, CAN_IT_FMP1);
        CAN_Receive(CAN1, CAN_FIFO1, &rx_message1);
			  Can1Receive1(rx_message1);
    }
}


extern Motor_Typedef Trigger;


/**********************************************************************************************************
*函 数 名: Can1Receive1
*功能说明: can1解码
*形    参: 
*返 回 值: 无
**********************************************************************************************************/
void Can1Receive1(CanRxMsg rx_message1)
{
	switch(rx_message1.StdId)
	{ 		
		case 0x205:
				 Trigger.Receive.Angle=rx_message1.Data[0]<<8 | rx_message1.Data[1];
				 Trigger.Receive.RealSpeed=rx_message1.Data[2]<<8 | rx_message1.Data[3];
 		break;
	}
}



extern M3508Receive_Typedef BigFrict[2];

/**********************************************************************************************************
*函 数 名: CAN1_Receive0Task
*功能说明: can1解码
*形    参: 
*返 回 值: 无
**********************************************************************************************************/
void CAN1_Receive0Task(CanRxMsg rx_message)
{
			switch(rx_message.StdId)
			{ 								
				 case 0x201:      // Fric Right;
					 BigFrict[1].Angle 			= (rx_message.Data[0]<<8) | rx_message.Data[1];
					 BigFrict[1].RealSpeed 	= (rx_message.Data[2]<<8) | rx_message.Data[3];	
					 break;
				 case 0x202:     //Frict  Left
					 BigFrict[0].Angle 			= (rx_message.Data[0]<<8) | rx_message.Data[1];
					 BigFrict[0].RealSpeed 	= (rx_message.Data[2]<<8) | rx_message.Data[3];
					 break;
			}	
}

