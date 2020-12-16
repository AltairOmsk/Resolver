/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define STATOR_7_Pin GPIO_PIN_13
#define STATOR_7_GPIO_Port GPIOB
#define STATOR_8_Pin GPIO_PIN_14
#define STATOR_8_GPIO_Port GPIOB
#define STATOR_5_Pin GPIO_PIN_8
#define STATOR_5_GPIO_Port GPIOA
#define STATOR_6_Pin GPIO_PIN_9
#define STATOR_6_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

#define __LED(__LED_STATE__)            HAL_GPIO_WritePin (LED_GPIO_Port, LED_Pin, __LED_STATE__)
#define __LED_TOGGLE                    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin)

#define __STATOR_5(__STATOR_5_STATE__)  HAL_GPIO_WritePin(STATOR_5_GPIO_Port, STATOR_5_Pin, __STATOR_5_STATE__)
#define __STATOR_6(__STATOR_6_STATE__)  HAL_GPIO_WritePin(STATOR_6_GPIO_Port, STATOR_6_Pin, __STATOR_6_STATE__)
#define __STATOR_7(__STATOR_7_STATE__)  HAL_GPIO_WritePin(STATOR_7_GPIO_Port, STATOR_7_Pin, __STATOR_7_STATE__)
#define __STATOR_8(__STATOR_8_STATE__)  HAL_GPIO_WritePin(STATOR_8_GPIO_Port, STATOR_8_Pin, __STATOR_8_STATE__)





#define BUF_SIZE        400                                                      // 20 на раскачку фильтра и 50 в зачет



/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
