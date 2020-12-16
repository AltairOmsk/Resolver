#ifndef _RESOLVER_H // Block reuse module here
#define _RESOLVER_H
//******************************************************************************
// Section include: connecting header files and modules here
//******************************************************************************
#include "stm32f1xx_hal.h"
#include "main.h"
#include "stdbool.h"
#include "stdio.h"
#include "string.h"
//******************************************************************************
// Constants
//******************************************************************************
//#define MY_CONST1 1
//#define MY_CONST2 2
//#define ...
//******************************************************************************
// Types
//******************************************************************************
typedef struct{
  float S1;
  float A2;
  float A3;
  float Prev1;
  float Prev2;
  float Out;
}
BPF2_t;


//******************************************************************************
// Global variables definitions
//******************************************************************************
extern uint8_t  MEAS_Enable;
extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
extern uint16_t ADC_buf[BUF_SIZE];
extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_tx;
//******************************************************************************
// Global functions prototypes
//******************************************************************************
void routine_8kHz_INT (void);
void angle_calculate  (void);
//******************************************************************************
// Macros definitions
//******************************************************************************
//#define MACRO1 ...
//#define MACRO2 ...
//#define ...
#endif // Close #endif for block reusing file
//******************************************************************************
// ENF OF FILE
//******************************************************************************
