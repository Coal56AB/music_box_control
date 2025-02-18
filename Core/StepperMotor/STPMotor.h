#ifndef STP_MOTOR_H
#define STP_MOTOR_H

#include "stm32f1xx_hal.h"
#include "arm_math.h"

//#define GPIO_SEPARATE_PORTS
//#define USE_GPIO_PORTS
#define USE_PWM_TIM


#define STP_GET_TIM_CLK_FREQ(_htim_) (SystemCoreClock/(_htim_->Instance->PSC+1))		
#define IS_MICROSTEP_VALID(_microstp_)  \
(((int)(_microstp_) == 2) || \
 ((int)(_microstp_) == 4) || \
 ((int)(_microstp_) == 8) || \
 ((int)(_microstp_) == 16) || \
 ((int)(_microstp_) == 32) || \
 ((int)(_microstp_) == 64) || \
 ((int)(_microstp_) == 128))


/** 
	* @brief 	Handle for ramp.
	*/
typedef struct {
	unsigned pid_initialized;
	/* Controller gains */
	float setpoint;	/*!< Parameters of PID-regulator */
	arm_pid_instance_f32 pid;	/*!< Parameters of PID-regulator */

	/* Output limits */
	float limMin;							/*!< Limit of PID-regulator: max rate of increase */
	float limMax;							/*!< Limit of PID-regulator: max rate of decrease */
	
	/* Integrator limits */
	float limMinInt;					/*!< Limit of integrator: max rate of increase */
	float limMaxInt;					/*!< Limit of integrator: max rate of decrease */

	/* Sample time (in seconds) */
	float SampleT;						/*!< Sample time of regulator (usualy 1/TIM_Hz that calls function of update PID-regulator) */
	
} STP_PIDHandleTypeDef;

typedef struct
{
	volatile uint32_t 	*timarr;
	volatile uint32_t 	*timcnt;
	uint16_t 	totalSteps;
	uint8_t 	microStepping;
	uint8_t 	sequenceSize;
	uint16_t 	*stepSequence;
	
	uint8_t 	direction;
	uint8_t 	stepCount;
	
	unsigned	stopFlag;
	unsigned	forceStopFlag;
	
}STP_RotationHandleTypeDef;


typedef struct
{
#ifdef USE_GPIO_PORTS
#ifdef GPIO_SEPARATE_PORTS
	GPIO_TypeDef 	*GPIOx_APlus;
	uint32_t 			GPIO_PIN_X_APlus;
	GPIO_TypeDef 	*GPIOx_AMinus;
	uint32_t 			GPIO_PIN_X_AMinus;
	GPIO_TypeDef 	*GPIOx_BPlus;
	uint32_t 			GPIO_PIN_X_BPlus;
	GPIO_TypeDef 	*GPIOx_BMinus;
	uint32_t 			GPIO_PIN_X_BMinus;
#else
	GPIO_TypeDef 	*GPIOx;
	uint32_t 			GPIO_PIN_X_APlus;
	uint32_t 			GPIO_PIN_X_AMinus;
	uint32_t 			GPIO_PIN_X_BPlus;
	uint32_t 			GPIO_PIN_X_BMinus;
#endif
#endif
  
#ifdef USE_PWM_TIM
  uint32_t      Channel_APlus;
  uint32_t      Channel_AMinus;
  uint32_t      Channel_BPlus;
  uint32_t      Channel_BMinus;
#endif
}STP_OutputsTypeDef;

typedef enum
{
	STP_MODE_UNKNOWN 		= 0,
	STP_MODE_FULL_STEP 	= 1,
	STP_MODE_HALF_STEP 	= 2,
}STP_MotorModeTypeDef;

typedef struct
{
	float                      	frequency;
	STP_MotorModeTypeDef				mode;
	
	TIM_HandleTypeDef						*htim;
	STP_PIDHandleTypeDef 				hramp;
	
	STP_RotationHandleTypeDef  	rhandle;
  
	STP_OutputsTypeDef					GPIOs;
	
}STP_MotorHandleTypeDef;



HAL_StatusTypeDef STP_MotorStart(STP_MotorHandleTypeDef *motor);
HAL_StatusTypeDef STP_MotorInit(STP_MotorHandleTypeDef *motor, TIM_HandleTypeDef *htim, STP_MotorModeTypeDef ControlMode, uint8_t totalSteps);
HAL_StatusTypeDef STP_SetMotorMicrosteps(STP_MotorHandleTypeDef *motor, uint8_t microsteps);
HAL_StatusTypeDef STP_SetMotorPIDParams(STP_MotorHandleTypeDef *motor, float Kp, float Ki, float Kd);
HAL_StatusTypeDef STP_SetMotorLimits(STP_MotorHandleTypeDef *motor, float speedMax, float speedMin, float acceleration, float slowdown);
#ifdef USE_GPIO_PORTS
HAL_StatusTypeDef STP_SetMotorPins(STP_MotorHandleTypeDef *motor, GPIO_TypeDef *gpiox, 
																			uint32_t pin_APlus, uint32_t pin_AMinus, uint32_t pin_BPlus, uint32_t pin_BMinus);
#endif
#ifdef USE_PWM_TIM
HAL_StatusTypeDef STP_SetMotorPins(STP_MotorHandleTypeDef *motor, TIM_HandleTypeDef *htim, 
																			uint32_t channel_APlus, uint32_t channel_AMinus, uint32_t channel_BPlus, uint32_t channel_BMinus);
#endif
HAL_StatusTypeDef STP_SetMotorFrequency(STP_MotorHandleTypeDef *motor, float Speed);

#ifdef USE_GPIO_PORTS
HAL_StatusTypeDef STP_MotorStep(STP_RotationHandleTypeDef *hrotor, STP_OutputsTypeDef *gpios);
#endif
#ifdef USE_PWM_TIM
HAL_StatusTypeDef STP_MotorStep(STP_RotationHandleTypeDef *hrotor, TIM_HandleTypeDef *htim, STP_OutputsTypeDef *channels, STP_MotorModeTypeDef mode);
#endif

HAL_StatusTypeDef STP_MotorHandleTIM(STP_MotorHandleTypeDef *motor);

static void Service_SetMotorMode(STP_MotorHandleTypeDef *motor);
static HAL_StatusTypeDef Service_SetMotorFrequency(STP_MotorHandleTypeDef *motor, float Speed);
static void Service_Ramp_ControlValue(STP_PIDHandleTypeDef *hramp, float *PID_Output);
static void Service_IsPIDInitialized(STP_PIDHandleTypeDef *hramp);
#endif /* STP_MOTOR_H */
