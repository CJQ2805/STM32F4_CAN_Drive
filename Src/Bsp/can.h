#ifndef __CAN_H
#define __CAN_H
#include "bsp.h"

#define CAN1_FILTER_BANK  0             ///< 主CAN过滤器组编号
#define CAN2_FILTER_BANK  14            ///< 从CAN过滤器组编号

#define CAN1_CHANNEL    0
#define CAN2_CHANNEL    1

void CAN_InitHandle(u8 u8channel, u16 u16baud_rate_k);
void CAN_Init(void);
#endif
