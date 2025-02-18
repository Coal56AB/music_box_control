/** 
**************************************************************************
* @file mylibs_defs.h
* @brief Заголочный файл для дефайнов библиотеки MyLibsGeneral.
**************************************************************************
* @defgroup MYLIBS_DEFINES  My Libs defines
* @brief 		Базовые дефайны для всего проекта
*
*************************************************************************/
#ifndef __MYLIBS_DEFINES_H_
#define __MYLIBS_DEFINES_H_

#include "mylibs_config.h"

/***************************************************************************
******************************ERROR_HANDLER********************************/
/** 
	* @addtogroup ERROR_HANDLER_DEFINES   Error Handler defines
	* @ingroup 		MYLIBS_DEFINES
  * @brief 		  Дефайны для определения функции обработки ошибок
	@{
	*/

/* extern Error_Handler from main.h */
extern void Error_Handler(void);

/* Define error handler for MyLibs */
#define MyLibs_Error_Handler(_params_) 	Error_Handler(_params_)
/* If error handler not defined - set void */
#ifndef MyLibs_Error_Handler
#define ((void)0U)
#endif // MyLibs_Error_Handler

/** @brief Check one pointer */
#define check_null_ptr_1(_p1_)                          (_p1_ == NULL)
/** @brief Check two pointers */
#define check_null_ptr_2(_p1_, _p2_)                    ((_p1_ == NULL) || (_p2_ == NULL))
/** @brief Check three pointers */
#define check_null_ptr_3(_p1_, _p2_, _p3_)              ((_p1_ == NULL) || (_p2_ == NULL) || (_p3_ == NULL))
/** @brief Check four pointers */
#define check_null_ptr_4(_p1_, _p2_, _p3_, _p4_)        ((_p1_ == NULL) || (_p2_ == NULL) || (_p3_ == NULL) || (_p4_ == NULL))
/** @brief Check five pointers */
#define check_null_ptr_5(_p1_, _p2_, _p3_, _p4_, _p5_)  ((_p1_ == NULL) || (_p2_ == NULL) || (_p3_ == NULL) || (_p4_ == NULL) || (_p5_ == NULL))

/** ERROR_HANDLER_DEFINES
  * @}
  */
  

/***************************************************************************
********************************ACCESS_DEFINES*****************************/

#define ClearStruct(_struct_)     memset(&(_struct_), 0, sizeof(_struct_))



/***************************************************************************
******************************DELAYS_DEFINES*******************************/
/** 
	* @addtogroup DELAYS_DEFINES    Delays defines
	* @ingroup 		MYLIBS_DEFINES
  * @brief 		  Дефайны для реализации задержек
	@{
	*/

#ifdef FREERTOS_DELAY
	#define msDelay(_ms_)			  osDelay(_ms_)
#else
	#define msDelay(_ms_)			  HAL_Delay(_ms_)
#endif

/**
	* @brief	Save tick at starting delay (tickstart in HAL)
  *	@param 	_pvar_  - указатель для переменной для сохранения тиков.
  *	@details Сохраняет начало отсчета задержки (текущее количество тиков)
	*/
#define msDelayStart(_pvar_)            *(_pvar_) = HAL_GetTick()

/**
	* @brief	Wait for delay after tickstart in _pvar_
  *	@param 	_pvar_  - указатель для переменной для сохранения тиков.
  *	@details Выставляет 1, когда задержка активна
	*/
#define msDelayWhileActive(_ms_, _pvar_)    (HAL_GetTick() - *(_pvar_) < _ms_)

/**
	* @brief	Wait until delay is done tickstart in _pvar_
  *	@param 	_pvar_  - указатель для переменной для сохранения тиков.
  *	@details Выставляет 1, когда задержка истекла
	*/
#define msDelayWaitDone(_ms_, _pvar_)    (HAL_GetTick() - *(_pvar_) >= _ms_)

/** DELAYS_DEFINES
  * @}
  */


/***************************************************************************
*******************************MATH_DEFINES********************************/
/** 
	* @addtogroup MATH_DEFINES      Math defines
	* @ingroup 		MYLIBS_DEFINES
  * @brief 		  Дефайны для различных математических функций
	@{
	*/

/**
	* @brief	Calc dividing including remainder
	*	@param 	_val_		- делимое.
	*	@param 	_div_		- делитель.
	*	@details 	Если результат деления без остатка: он возвращается как есть
						Если с остатком - округляется вверх
	*/
//#define Divide_Up(_val_, _div_)												(((_val_)%(_div_))? (_val_)/(_div_)+1 : (_val_)/_div_)	/* через тернарный оператор */
#define Divide_Up(_val_, _div_)												((_val_ - 1) / _div_) + 1																	/* через мат выражение */	

/** 
	* @brief 	Swap between Little Endian and Big Endian
	* @param 	v 			- Переменная для свапа.
  * @return v (new)	- Свапнутая переменная.
  * @details 	Переключения между двумя типами хранения слова: HI-LO байты и LO-HI байты.
	*/
#define ByteSwap16(v)																	(((v&0xFF00) >> (8)) | ((v&0x00FF) << (8)))

/** 
	* @brief 	Absolute
	* @param 	x 			- Переменная для модудя.
  * @return x (new)	- Число по модулю.
  * @details  Берет число по модулю. Хз как работает библиотечный abs в stdlib.h, мб это быстрее, но вряд ли конечно.
	*/
#define ABS(x) ( ((x) > 0)? (x) : -(x))?

/** MATH_DEFINES
  * @}
  */
  
#ifndef LED_PWM_TICKS
#define LED_PWM_TICKS 15
#endif
  
#ifndef LED_ON
#define LED_ON 1
#endif
#ifndef LED_OFF
#define LED_OFF 0
#endif

#ifndef SW_ON
#define SW_ON 1
#endif
#ifndef SW_OFF
#define SW_OFF 0
#endif

#endif //__MYLIBS_DEFINES_H_