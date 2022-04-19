#include "can.h"
#include "bsp.h"
#include "j1939.h"
struct _can_param_t{
	
	CAN_HandleTypeDef hcan;
	CAN_TxHeaderTypeDef tcan_tx_header;
	
	u8 au8tx_data[8];
	u32 u32tx_mail_box;
	
	volatile CAN_RxHeaderTypeDef  tcan_rx_header;
	volatile u8 au8rx_data[8];
	
};

struct _can_param_t g_atcan_param[2];


/**
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */

void CAN_Init(void)
{	
	CAN_InitHandle(CAN1_CHANNEL, 250);
	CAN_InitHandle(CAN2_CHANNEL, 250);	
}

void CAN_InitHandle(u8 u8channel, u16 u16baud_rate_k)
{
	CAN_FilterTypeDef  sFilterConfig;

	//CAN 波特率的计算方式是 BAUD = 45M/10/(1+9+8) = 250k	
	if(250 == u16baud_rate_k)
	{
		g_atcan_param[u8channel].hcan.Init.Prescaler = 10;
		g_atcan_param[u8channel].hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
		g_atcan_param[u8channel].hcan.Init.TimeSeg1 = CAN_BS1_9TQ;
		g_atcan_param[u8channel].hcan.Init.TimeSeg2 = CAN_BS2_8TQ;
	}
	
	if(0 == u8channel)
	{
		g_atcan_param[u8channel].hcan.Instance = CAN1; 
		sFilterConfig.FilterBank = CAN1_FILTER_BANK;
		sFilterConfig.SlaveStartFilterBank = 0;
	}
	else
	{
		g_atcan_param[u8channel].hcan.Instance = CAN2;
		sFilterConfig.FilterBank =CAN2_FILTER_BANK;
		sFilterConfig.SlaveStartFilterBank = 14;
	}
	
	g_atcan_param[u8channel].hcan.Init.Mode = CAN_MODE_NORMAL;
	
    g_atcan_param[u8channel].hcan.Init.TimeTriggeredMode = DISABLE;
	g_atcan_param[u8channel].hcan.Init.AutoBusOff = DISABLE;
	g_atcan_param[u8channel].hcan.Init.AutoWakeUp = ENABLE;
	g_atcan_param[u8channel].hcan.Init.AutoRetransmission = ENABLE;
	g_atcan_param[u8channel].hcan.Init.ReceiveFifoLocked = DISABLE;
	g_atcan_param[u8channel].hcan.Init.TransmitFifoPriority = ENABLE;	

    if (HAL_CAN_Init(&g_atcan_param[u8channel].hcan) != HAL_OK)
    {
	   Error_Handler();
    }	
	
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = 0x0000;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	sFilterConfig.FilterActivation = ENABLE;
	
	
	if (HAL_CAN_ConfigFilter(&g_atcan_param[u8channel].hcan, &sFilterConfig) != HAL_OK)
	{
	  /* Filter configuration Error */
	   Error_Handler();
	}

	if (HAL_CAN_ActivateNotification(&g_atcan_param[u8channel].hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
	{
	  /* Notification Error */
	   Error_Handler();
	}	
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	/*Get rx message */
	if(hcan->Instance == g_atcan_param[CAN1_CHANNEL].hcan.Instance)
	{
		if(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, (CAN_RxHeaderTypeDef*)&g_atcan_param[CAN1_CHANNEL].tcan_rx_header, (u8*)g_atcan_param[CAN1_CHANNEL].au8rx_data) != HAL_OK)
		{
			printf("error");
			
		}else{
			j1939_rx_data((CAN_RxHeaderTypeDef*)&g_atcan_param[CAN1_CHANNEL].tcan_rx_header,(u8 *)g_atcan_param[CAN1_CHANNEL].au8rx_data);
			//这里就是CAN1接收到的整包数据，可以单独封装函数进行处理
			
		}
	}
	
	if(hcan->Instance == g_atcan_param[CAN2_CHANNEL].hcan.Instance)
	{
		if(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, (CAN_RxHeaderTypeDef*)&g_atcan_param[CAN2_CHANNEL].tcan_rx_header, (u8*)g_atcan_param[CAN2_CHANNEL].au8rx_data) != HAL_OK)
		{
		
		}
		else
		{
			//这里就是CAN2接收到的整包数据，可以单独封装函数进行处理
		}
	}

}

int CAN_Tx(u8 u8channel, u32 u32ExtId, u8 *pu8data, u8 u8len)
{
	HAL_StatusTypeDef  hal_type = HAL_OK;
	int ierro = 0;
	
	if(u8len > 8)
	{
		return -1;
	}
	
	g_atcan_param[u8channel].tcan_tx_header.ExtId   			= u32ExtId;
	g_atcan_param[u8channel].tcan_tx_header.DLC       			= u8len;
	g_atcan_param[u8channel].tcan_tx_header.RTR					= CAN_RTR_DATA;
	g_atcan_param[u8channel].tcan_tx_header.IDE					= CAN_ID_EXT;
	g_atcan_param[u8channel].tcan_tx_header.TransmitGlobalTime  = ENABLE;

	memcpy(g_atcan_param[u8channel].au8tx_data, pu8data, u8len);
	
	/*三次重发机制*/
	for(u8 i = 0; i < 3; i++)
	{
		hal_type = HAL_CAN_AddTxMessage(&g_atcan_param[u8channel].hcan, &g_atcan_param[u8channel].tcan_tx_header, g_atcan_param[u8channel].au8tx_data, &g_atcan_param[u8channel].u32tx_mail_box);
		if(hal_type != HAL_OK)
		{
			ierro = -1;
			continue;
		}
		else if(hal_type == HAL_OK)
		{
			ierro = 0;
			break;
		}
	}
	
	return ierro;
}




static uint32_t HAL_RCC_CAN1_CLK_ENABLED=0;

/**
* @brief CAN MSP Initialization
* This function configures the hardware resources used in this example
* @param hcan: CAN handle pointer
* @retval None
*/
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hcan->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* Peripheral clock enable */
    HAL_RCC_CAN1_CLK_ENABLED++;
    if(HAL_RCC_CAN1_CLK_ENABLED==1){
      __HAL_RCC_CAN1_CLK_ENABLE();
    }

    __HAL_RCC_GPIOI_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN1 GPIO Configuration
    PI9    	 ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
  else if(hcan->Instance==CAN2)
  {
  /* USER CODE BEGIN CAN2_MspInit 0 */

  /* USER CODE END CAN2_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_CAN2_CLK_ENABLE();
    HAL_RCC_CAN1_CLK_ENABLED++;
    if(HAL_RCC_CAN1_CLK_ENABLED==1){
      __HAL_RCC_CAN1_CLK_ENABLE();
    }

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**CAN2 GPIO Configuration
    PB12     ------> CAN2_RX
    PB13     ------> CAN2_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* CAN2 interrupt Init */
    HAL_NVIC_SetPriority(CAN2_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn);
  /* USER CODE BEGIN CAN2_MspInit 1 */

  /* USER CODE END CAN2_MspInit 1 */
  }

}

/**
* @brief CAN MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hcan: CAN handle pointer
* @retval None
*/
void HAL_CAN_MspDeInit(CAN_HandleTypeDef* hcan)
{
  if(hcan->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    HAL_RCC_CAN1_CLK_ENABLED--;
    if(HAL_RCC_CAN1_CLK_ENABLED==0){
      __HAL_RCC_CAN1_CLK_DISABLE();
    }

    /**CAN1 GPIO Configuration
    PI9     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOI, GPIO_PIN_9);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_12);

    /* CAN1 interrupt DeInit */
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
  else if(hcan->Instance==CAN2)
  {
  /* USER CODE BEGIN CAN2_MspDeInit 0 */

  /* USER CODE END CAN2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN2_CLK_DISABLE();
    HAL_RCC_CAN1_CLK_ENABLED--;
    if(HAL_RCC_CAN1_CLK_ENABLED==0){
      __HAL_RCC_CAN1_CLK_DISABLE();
    }

    /**CAN2 GPIO Configuration
    PB12     ------> CAN2_RX
    PB13     ------> CAN2_TX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12|GPIO_PIN_13);

    /* CAN2 interrupt DeInit */
    HAL_NVIC_DisableIRQ(CAN2_RX0_IRQn);
  /* USER CODE BEGIN CAN2_MspDeInit 1 */

  /* USER CODE END CAN2_MspDeInit 1 */
  }

}


/**
  * @brief This function handles CAN2 RX0 interrupts.
  */
void CAN2_RX0_IRQHandler(void)
{
  /* USER CODE BEGIN CAN2_RX0_IRQn 0 */

  /* USER CODE END CAN2_RX0_IRQn 0 */
  HAL_CAN_IRQHandler(&g_atcan_param[0].hcan);
  /* USER CODE BEGIN CAN2_RX0_IRQn 1 */

  /* USER CODE END CAN2_RX0_IRQn 1 */
}


/**
  * @brief This function handles CAN1 RX0 interrupts.
  */
void CAN1_RX0_IRQHandler(void)
{
  /* USER CODE BEGIN CAN1_RX0_IRQn 0 */

  /* USER CODE END CAN1_RX0_IRQn 0 */
  HAL_CAN_IRQHandler(&g_atcan_param[1].hcan);
  /* USER CODE BEGIN CAN1_RX0_IRQn 1 */

  /* USER CODE END CAN1_RX0_IRQn 1 */
}
