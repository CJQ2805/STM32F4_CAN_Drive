#ifndef __BSP_H
#define __BSP_H
#include "bsp.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_it.h"

typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef const int32_t sc32;  
typedef const int16_t sc16;  
typedef const int8_t sc8;  

typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef __I int32_t vsc32;  
typedef __I int16_t vsc16; 
typedef __I int8_t vsc8;   


typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;  
typedef const uint16_t uc16;  
typedef const uint8_t uc8; 

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef __I uint32_t vuc32;  
typedef __I uint16_t vuc16; 
typedef __I uint8_t vuc8;

typedef enum
{
    Ok = 0u,                       ///< No error
    Error = 1u,                    ///< Non-specific error code
    ErrorAddressAlignment = 2u,    ///< Address alignment does not match
    ErrorAccessRights = 3u,        ///< Wrong mode (e.g. user/system) mode is set
    ErrorInvalidParameter = 4u,    ///< Provided parameter is not valid
    ErrorOperationInProgress = 5u, ///< A conflicting or requested operation is still in progress
    ErrorInvalidMode = 6u,         ///< Operation not allowed in current mode
    ErrorUninitialized = 7u,       ///< Module (or part of it) was not initialized properly
    ErrorBufferFull = 8u,          ///< Circular buffer can not be written because the buffer is full
    ErrorTimeout = 9u,             ///< Time Out error occurred (e.g. I2C arbitration lost, Flash time-out, etc.)
    ErrorNotReady = 10u,           ///< A requested final state is not reached
    ErrorCommunication = 11u,      ///< Communication error occured between MCU and peripheral
    ErrorVerificationFailed = 12u, ///<Datas verification failed(e.g. CRC/checksum)
    OperationInProgress = 13u      ///< Indicator for operation in progress
} en_result_t;
/* Delay counter type */
typedef uint32_t u32Tim_dly;


void Error_Handler(void);

#endif
