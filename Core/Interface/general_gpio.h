/** 
	**************************************************************************
	* @file general_gpio.h
	* @brief Заголовочный файл для модуля инициализации портов.
	*************************************************************************/
#ifndef __GPIO_GENERAL_H_
#define __GPIO_GENERAL_H_

#include "mylibs_defs.h"

typedef enum
{
  LED_IS_OFF = 0,
  LED_IS_ON = 1,
  LED_IS_BLINKING = 2,
  LED_IS_FADING = 3,
}GPIO_LEDStateTypeDef;

typedef struct
{
  GPIO_LEDStateTypeDef state;
  
  GPIO_TypeDef  *LED_Port;
  uint32_t      LED_Pin;
  
  uint8_t       LED_ActiveLvl;
  uint32_t      LED_Period;
  uint32_t      tickprev;
}GPIO_LEDTypeDef;

typedef struct
{
  GPIO_TypeDef  *Sw_Port;
  uint32_t      Sw_Pin;
  
  uint8_t       Sw_ActiveLvl;
  uint32_t      Sw_PrevState;
  uint32_t      Sw_FilterDelay;
  uint32_t      tickprev;
}GPIO_SwitchTypeDef;

/////////////////////////////////////////////////////////////////////
///////////////////////////---FUNCTIONS---///////////////////////////
HAL_StatusTypeDef GPIO_Clock_Enable(GPIO_TypeDef *GPIOx);
/* Инициализировать кнопку (структуру кнопки) */
HAL_StatusTypeDef GPIO_Switch_Init(GPIO_SwitchTypeDef *sw, GPIO_TypeDef *GPIOx, uint32_t GPIO_PIN_X, uint8_t SW_On_State);
/* Считать состоянии кнопки запуска */
uint8_t GPIO_Read_Swich(GPIO_SwitchTypeDef *swstart);

/* Инициализировать светодиод (структуру светодиода) */
HAL_StatusTypeDef GPIO_LED_Init(GPIO_LEDTypeDef *led, GPIO_TypeDef *GPIOx, uint32_t GPIO_PIN_X, uint8_t LED_On_State);
/* Включить светодиод */
HAL_StatusTypeDef GPIO_LED_On(GPIO_LEDTypeDef *led);
/* Выключить светодиод */
HAL_StatusTypeDef GPIO_LED_Off(GPIO_LEDTypeDef *led);
/* Активировать моргание светодиодом */
HAL_StatusTypeDef GPIO_LED_Blink_Start(GPIO_LEDTypeDef *led, uint32_t period);
/* Активировать моргание светодиодом */
HAL_StatusTypeDef GPIO_LED_Fading_Start(GPIO_LEDTypeDef *led, uint32_t period);
/* Управление динамическими режимами свечения светодиода */
void GPIO_LED_Dynamic_Handle(GPIO_LEDTypeDef *led);
///////////////////////////---FUNCTIONS---///////////////////////////


#endif // __GPIO_GENERAL_H_
