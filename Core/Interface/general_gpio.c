/** 
	**************************************************************************
	* @file general_gpio.c
	* @brief Модуль для инициализации портов.
	**************************************************************************
		@verbatim
		//-------------------Функции-------------------//
		Functions: users
			- GPIO_Clock_Enable							Инициализация тактирования порта
		@endverbatim
***************************************************************************/
#include "general_gpio.h"

//-------------------------------------------------------------------
//------------------------GPIO INIT FUNCTIONS------------------------

HAL_StatusTypeDef GPIO_Clock_Enable(GPIO_TypeDef *GPIOx)
{	
	HAL_StatusTypeDef status = HAL_OK;
	// choose port for enable clock
	if (GPIOx==GPIOA)
		__HAL_RCC_GPIOA_CLK_ENABLE();
	else if (GPIOx==GPIOB)
		__HAL_RCC_GPIOB_CLK_ENABLE();
	else if (GPIOx==GPIOC)
		__HAL_RCC_GPIOC_CLK_ENABLE();
	else if (GPIOx==GPIOD)
		__HAL_RCC_GPIOD_CLK_ENABLE();
	else if (GPIOx==GPIOE)
		__HAL_RCC_GPIOE_CLK_ENABLE();
	else
		status = HAL_ERROR;
	
	return status;
}

//------------------------GPIO INIT FUNCTIONS------------------------
//-------------------------------------------------------------------


//-------------------------------------------------------------------
//------------------------GPIO LED FUNCTIONS-------------------------

/**
  * @brief  Инициализировать светодиод (структуру светодиода)
  * @param led          - указатель на структуру светодиода
  * @param GPIOx        - указатель на структуру порта для светодиода
  * @param GPIO_PIN_X   - пин для светодиода
  * @param LED_On_State - состояния пина, при котором светодиод будет включен
  */
HAL_StatusTypeDef GPIO_LED_Init(GPIO_LEDTypeDef *led, GPIO_TypeDef *GPIOx, uint32_t GPIO_PIN_X, uint8_t LED_ActiveLevel)
{
  if(check_null_ptr_2(led, GPIOx))
    return HAL_ERROR;
  
  led->LED_Port       = GPIOx;
  led->LED_Pin        = GPIO_PIN_X;
  led->LED_ActiveLvl  = LED_ActiveLevel;
  
  GPIO_LED_Off(led);
  return HAL_OK;
}

/**
  * @brief  Включить светодиод 
  * @param led - указатель на структуру светодиода
  */
HAL_StatusTypeDef GPIO_LED_On(GPIO_LEDTypeDef *led)
{
  if(check_null_ptr_1(led))
    return HAL_ERROR;
  
  led->state = LED_IS_ON;
  if(led->LED_Port != NULL)
    HAL_GPIO_WritePin(led->LED_Port, led->LED_Pin, led->LED_ActiveLvl);
  else
    return HAL_ERROR;
  
  return HAL_OK;
}
/**
  * @brief  Выключить светодиод 
  * @param led - указатель на структуру светодиода
  */
HAL_StatusTypeDef GPIO_LED_Off(GPIO_LEDTypeDef *led)
{
  if(check_null_ptr_1(led))
    return HAL_ERROR;
  
  led->state = LED_IS_OFF;
  if(led->LED_Port != NULL)
    HAL_GPIO_WritePin(led->LED_Port, led->LED_Pin, !led->LED_ActiveLvl);
  else
    return HAL_ERROR;
  
  
  return HAL_OK;
}

/**
  * @brief Активировать моргание светодиодом
  * @param led - указатель на структуру светодиода
  * @details  Функция ставит режим моргания, который после управляется в @ref GPIO_LED_Dynamic_Handle
	*/
HAL_StatusTypeDef GPIO_LED_Blink_Start(GPIO_LEDTypeDef *led, uint32_t period)
{
  if(check_null_ptr_2(led, led->LED_Port))
    return HAL_ERROR;  
  
  led->state = LED_IS_BLINKING;
  led->LED_Period = period;
    
  return HAL_OK;
}
  
/**
  * @brief Активировать моргание светодиодом
  * @param led - указатель на структуру светодиода
  * @details  Функция ставит режим моргания, который после управляется в @ref GPIO_LED_Dynamic_Handle
	*/
HAL_StatusTypeDef GPIO_LED_Fading_Start(GPIO_LEDTypeDef *led, uint32_t period)
{
  if(check_null_ptr_2(led, led->LED_Port))
    return HAL_ERROR;  
  
  led->state = LED_IS_FADING;
  led->LED_Period = period;
  
    
  return HAL_OK;
}

//uint8_t LED_PWM_FADING_DUTYS[LED_PWM_TICKS] = {0 1 2 3 4 5 6 7 8 9 10 11 12 }
/**
  * @brief Управление динамическими режимами свечения светодиода
  * @param led - указатель на структуру светодиода
  * @details  Функция моргает/плавно моргает светодиодом в неблокирующем режиме
  * Т.е. функцию надо вызывать постоянно, чтобы она мониторила тики 
  * и в нужный момент переключала светодиод
	*/
void GPIO_LED_Dynamic_Handle(GPIO_LEDTypeDef *led)
{
  if(check_null_ptr_2(led, led->LED_Port))
    return;
  
  /* Режим моргания светодиода */
  if(led->state == LED_IS_BLINKING)
  {
    uint32_t tickcurrent = HAL_GetTick();
    /* Ожидание истечения периода моргания */
    if((tickcurrent - led->tickprev) > led->LED_Period)
    {
      /* Моргание */
      HAL_GPIO_TogglePin(led->LED_Port, led->LED_Pin);
      
      led->tickprev = tickcurrent;
    }
  }
  /* Режим плавного моргания светодиода */
  else if(led->state == LED_IS_FADING)
  {
    static unsigned direction = 0;
    static int duty = 0;
    uint32_t tickcurrent = HAL_GetTick();
    /* Ожидание момента изменения яркости */
    /* Период ШИМ 20 мс, поэтому менять яроксть надо 40 раз за период (туда обратно) */
    if((tickcurrent - led->tickprev) > led->LED_Period/(LED_PWM_TICKS*2))
    {
      /* Формирование разтухания */
      if(direction == 0)
      {
        if(++duty >= LED_PWM_TICKS)
        {
          direction = 1;
          duty = LED_PWM_TICKS;
        }
      }
      /* Формирование затухания */
      else
      {
        if(--duty  <= 0)
        {
          direction = 0;
          duty = 0;
        }
      }
      led->tickprev = tickcurrent;
    }
    /* Формирование ШИМ для изменения яркости */
    int duty_crt = (duty*duty/LED_PWM_TICKS);
    if(tickcurrent%LED_PWM_TICKS < duty_crt)
    {
      HAL_GPIO_WritePin(led->LED_Port, led->LED_Pin, led->LED_ActiveLvl);
    }
    else
    {
      HAL_GPIO_WritePin(led->LED_Port, led->LED_Pin, !led->LED_ActiveLvl);
    }
  }
}
//------------------------GPIO LED FUNCTIONS-------------------------
//-------------------------------------------------------------------

//-------------------------------------------------------------------
//------------------------GPIO SW FUNCTIONS-------------------------

/**
* @brief  Инициализировать кнопку (структуру кнопки)
  * @param sw             - указатель на структуру кнопки
  * @param GPIOx          - указатель на структуру порта для кнопки
  * @param GPIO_PIN_X     - пин для кнопки
  * @param SW_ActiveLevel - состояния пина, когда кнопка нажата
  */
HAL_StatusTypeDef GPIO_Switch_Init(GPIO_SwitchTypeDef *sw, GPIO_TypeDef *GPIOx, uint32_t GPIO_PIN_X, uint8_t SW_ActiveLevel)
{
  if(check_null_ptr_2(sw, GPIOx))
    return HAL_ERROR;
  
  sw->Sw_Port       = GPIOx;
  sw->Sw_Pin        = GPIO_PIN_X;
  sw->Sw_ActiveLvl  = SW_ActiveLevel;
	
	if(sw->Sw_FilterDelay == 0)
		sw->Sw_FilterDelay = 50;
	
  return HAL_OK;
}

/**
  * @brief Считать состоянии кнопки 
  * @param  sw - указатель на структуру кнопки
  * @return 1 - если кнопка нажата, 0 - если отжата
  * @details  Функция включает в себя неблокирующую проверку на дребезг
  * Т.е. функцию надо вызывать постоянно, чтобы она мониторила состояние кнопки
  */
uint8_t GPIO_Read_Swich(GPIO_SwitchTypeDef *sw)
{
  if(check_null_ptr_1(sw))
    return 0;
 
//	if(sw->tickprev == 0)
	sw->Sw_PrevState = sw->Sw_State;
	
  if(HAL_GPIO_ReadPin(sw->Sw_Port, sw->Sw_Pin) == sw->Sw_ActiveLvl)
  {
    
    if(sw->tickprev == 0)
      sw->tickprev = HAL_GetTick();
    
    if((HAL_GetTick() - sw->tickprev) >= sw->Sw_FilterDelay)
    {
      if(HAL_GPIO_ReadPin(sw->Sw_Port, sw->Sw_Pin) == sw->Sw_ActiveLvl)
      {
				sw->Sw_State = 1;
        return 1;
      }
      else
      {
        sw->tickprev = 0;
				sw->Sw_State = 0;
        return 0;
      }
    }
  }
  else
  {
		sw->tickprev = 0;
    sw->Sw_State = 0;
  }
  return 0;
}
//------------------------GPIO SW FUNCTIONS-------------------------
//-------------------------------------------------------------------