#include "STPMotor.h"


static uint16_t sequenceFullStep[] = {0b0001, 				0b0100, 				0b0010, 				0b1000};
static uint16_t sequenceHalfStep[] = {0b0001, 0b0101, 0b0100, 0b0110, 0b0010, 0b1010, 0b1000, 0b1001};
static uint16_t sequenceHalfStepSine[] = {0b0101, 0b0110, 0b1010, 0b1001};
static float sin_table[128];


HAL_StatusTypeDef STP_MotorInit(STP_MotorHandleTypeDef *motor, TIM_HandleTypeDef *htim, uint16_t totalSteps)
{
	if(motor == NULL)
		return HAL_ERROR;
	if(htim == NULL)
		return HAL_ERROR;
	
	motor->htim = htim;
	motor->rhandle.totalSteps = totalSteps;
  motor->rhandle.timarr = &htim->Instance->ARR;
  motor->rhandle.timcnt = &htim->Instance->CNT;

	
	return HAL_OK;
}


HAL_StatusTypeDef STP_SetMotorMode(STP_MotorHandleTypeDef *motor, STP_MotorModeTypeDef ControlMode, uint8_t microsteps)
{
	if(motor == NULL)
		return HAL_ERROR;
	
	if(microsteps != 0)
		STP_SetMotorMicrosteps(motor, microsteps);
	else
		motor->rhandle.microStepping = 0;
	
	motor->mode = ControlMode;

	Service_SetMotorMode(motor);	
  	
  return HAL_OK;
}

HAL_StatusTypeDef STP_SetMotorMicrosteps(STP_MotorHandleTypeDef *motor, uint8_t microsteps)
{
	if(motor == NULL)
		return HAL_ERROR;
  if(IS_MICROSTEP_VALID(microsteps) == 0)
    return HAL_ERROR;
  
  motor->rhandle.microStepping = microsteps;
  	
  // Заполнение первой половины от 0 до 1
	uint32_t half_steps = motor->rhandle.microStepping / 2;
	for (int i = 0; i < half_steps; i++)
	{
		sin_table[i] = sin((i + 0.5) * (PI / 2) / half_steps); // Полуволна [0,1]
	}

	// Зеркальное копирование от 1 до 0
	for (int i = 0; i < half_steps; i++)
	{
		sin_table[half_steps + i] = sin_table[half_steps - 1 - i];
	}
  
//  // Заполнение таблицы синусоидальных коэффициентов
//  for(int i = 0; i < motor->rhandle.microStepping; i++)
//  {
//		sin_table[i] = sin((i + 0.5) * (PI) / motor->rhandle.microStepping); // Полуволна [0,1]
//  }
  
  return HAL_OK;
}

HAL_StatusTypeDef STP_SetMotorPIDParams(STP_MotorHandleTypeDef *motor, float Kp, float Ki, float Kd)
{
	if(motor == NULL)
		return HAL_ERROR;
		
	motor->hramp.pid.Kp = Kp;
	motor->hramp.pid.Ki = Ki;
	motor->hramp.pid.Kd = Kd;	
		
	motor->hramp.pid_initialized = 1;
	
	if((motor->hramp.pid.Kp == 0) && (motor->hramp.pid.Ki == 0) && (motor->hramp.pid.Kd == 0)) 
	{
		motor->hramp.pid_initialized = 0;
	}
	if((motor->hramp.limMax == 0) && (motor->hramp.limMin == 0)) 
	{
		motor->hramp.pid_initialized = 0;
	}
	if((motor->hramp.limMinInt == 0) && (motor->hramp.limMaxInt == 0)) 
	{
		motor->hramp.pid_initialized = 0;
	}
	
	return HAL_OK;
}
HAL_StatusTypeDef STP_SetMotorLimits(STP_MotorHandleTypeDef *motor, float speedMax, float speedMin, float acceleration, float slowdown)
{
	if(motor == NULL)
		return HAL_ERROR;
	
	motor->hramp.limMax = speedMax;
	motor->hramp.limMin = speedMin;
	motor->hramp.limMaxInt = acceleration;
	motor->hramp.limMinInt = slowdown;
	
	motor->hramp.pid_initialized = 1;
	
	if((motor->hramp.pid.Kp == 0) && (motor->hramp.pid.Ki == 0) && (motor->hramp.pid.Kd == 0)) 
	{
		motor->hramp.pid_initialized = 0;
	}
	if((motor->hramp.limMax == 0) && (motor->hramp.limMin == 0)) 
	{
		motor->hramp.pid_initialized = 0;
	}
	if((motor->hramp.limMinInt == 0) && (motor->hramp.limMaxInt == 0)) 
	{
		motor->hramp.pid_initialized = 0;
	}
	
	
	return HAL_OK;
}

#ifdef USE_GPIO_PORTS
HAL_StatusTypeDef STP_SetMotorPins(STP_MotorHandleTypeDef *motor, GPIO_TypeDef *gpiox, 
																			uint32_t pin_APlus, uint32_t pin_AMinus, uint32_t pin_BPlus, uint32_t pin_BMinus)
{
	if(motor == NULL)
		return HAL_ERROR;
	if(gpiox == NULL)
		return HAL_ERROR;
	
	if(	(IS_GPIO_PIN(pin_APlus) == 0) &&
			(IS_GPIO_PIN(pin_AMinus) == 0) &&
			(IS_GPIO_PIN(pin_BPlus) == 0) &&
			(IS_GPIO_PIN(pin_BMinus) == 0))
		return HAL_ERROR;
	
	
	motor->Outputs.GPIOx = gpiox;
	motor->Outputs.GPIO_PIN_X_APlus = pin_APlus;
	motor->Outputs.GPIO_PIN_X_AMinus = pin_AMinus;
	motor->Outputs.GPIO_PIN_X_BPlus = pin_BPlus;
	motor->Outputs.GPIO_PIN_X_BMinus = pin_BMinus;
	
	return HAL_OK;
}
#endif //USE_GPIO_PORTS

#ifdef USE_PWM_TIM
HAL_StatusTypeDef STP_SetMotorPins(STP_MotorHandleTypeDef *motor, TIM_HandleTypeDef *htim, 
																			uint32_t channel_APlus, uint32_t channel_AMinus, uint32_t channel_BPlus, uint32_t channel_BMinus)
{
  
	if(motor == NULL)
		return HAL_ERROR;
	if(htim == NULL)
		return HAL_ERROR;
  
	if(	(IS_TIM_CHANNELS(channel_APlus) == 0) &&
			(IS_TIM_CHANNELS(channel_AMinus) == 0) &&
			(IS_TIM_CHANNELS(channel_BPlus) == 0) &&
			(IS_TIM_CHANNELS(channel_BMinus) == 0))
		return HAL_ERROR;
  
  motor->Outputs.Channel_APlus = channel_APlus;
  motor->Outputs.Channel_AMinus = channel_AMinus;
  motor->Outputs.Channel_BPlus = channel_BPlus;
  motor->Outputs.Channel_BMinus = channel_BMinus;
  
  return HAL_OK;
}
#endif //USE_PWM_TIM
HAL_StatusTypeDef STP_MotorStart(STP_MotorHandleTypeDef *motor)
{
#ifdef USE_PWM_TIM
  HAL_TIM_PWM_Start(motor->htim, motor->Outputs.Channel_APlus);
  HAL_TIM_PWM_Start(motor->htim, motor->Outputs.Channel_AMinus);
  HAL_TIM_PWM_Start(motor->htim, motor->Outputs.Channel_BPlus);
  HAL_TIM_PWM_Start(motor->htim, motor->Outputs.Channel_BMinus);
#endif
	return HAL_TIM_Base_Start_IT(motor->htim);
}

HAL_StatusTypeDef STP_SetMotorFrequency(STP_MotorHandleTypeDef *motor, float Speed)
{
	
	if(Speed == 0)
	{
		motor->hramp.setpoint = Speed;
		return HAL_OK;
	}
		
	uint32_t period = STP_GET_TIM_CLK_FREQ(motor->htim) / fabs(Speed); 
	period = period/motor->rhandle.totalSteps/motor->mode;
	
	if(period > 0xFFFF)
		return HAL_ERROR;
	
	motor->hramp.setpoint = Speed;
		
	return HAL_OK;
}
	

#ifdef USE_GPIO_PORTS
HAL_StatusTypeDef STP_MotorMicroStep(STP_RotationHandleTypeDef *hrotor, GPIO_TypeDef *gpiox, uint32_t gpio_pin)
{
	if(gpiox == NULL)
		return HAL_ERROR;
	
  uint32_t microperiod = *hrotor->timarr / hrotor->microStepping;
  uint32_t high_time;
  uint32_t prevperiodtick = 0;
  uint32_t prev_tick = *hrotor->timcnt;
  for(int i = 1; i <= hrotor->microStepping; i++)
  {
    high_time = (uint32_t)(sin_table[i-1] * microperiod);
    
    
    // Включение сигнала
    HAL_GPIO_WritePin(gpiox, gpio_pin, GPIO_PIN_SET);
    
    // Ожидание конца высокого уровня
    while(*hrotor->timcnt < high_time + prevperiodtick)
		{
			if (*hrotor->timcnt < prev_tick) // Проверка на переполнение таймера
				break;
			prev_tick = *hrotor->timcnt;
		}
    
    // Выключение сигнала
    HAL_GPIO_WritePin(gpiox, gpio_pin, GPIO_PIN_RESET);
    
    // ожидание окончания периода
    while(*hrotor->timcnt < i*microperiod)
		{
			if (*hrotor->timarr <= i*microperiod) // Проверка на переполнение таймера
				return HAL_OK;
			if (*hrotor->timcnt < prev_tick) // Проверка на переполнение таймера
				break;
			prev_tick = *hrotor->timcnt;
		}
    
    prevperiodtick = *hrotor->timcnt;
  }
  return HAL_OK;
}

HAL_StatusTypeDef STP_MotorStep(STP_RotationHandleTypeDef *hrotor, STP_OutputsTypeDef *gpios)
{
	if(hrotor == NULL)
		return HAL_ERROR;	
	if(gpios == NULL)
		return HAL_ERROR;
	
	uint16_t step = hrotor->stepSequence[hrotor->stepCount];

	if(hrotor->stopFlag == 0)
	{
    if(hrotor->microStepping)
    {
      if(step&(1<<0))
        STP_MotorMicroStep(hrotor, gpios->GPIOx, gpios->GPIO_PIN_X_APlus);
      else
        HAL_GPIO_WritePin(gpios->GPIOx, gpios->GPIO_PIN_X_APlus, GPIO_PIN_RESET);
        
      if(step&(1<<1))
        STP_MotorMicroStep(hrotor, gpios->GPIOx, gpios->GPIO_PIN_X_AMinus);
      else
        HAL_GPIO_WritePin(gpios->GPIOx, gpios->GPIO_PIN_X_AMinus, GPIO_PIN_RESET);
      
      if(step&(1<<2))
        STP_MotorMicroStep(hrotor, gpios->GPIOx, gpios->GPIO_PIN_X_BPlus);
      else
        HAL_GPIO_WritePin(gpios->GPIOx, gpios->GPIO_PIN_X_BPlus, GPIO_PIN_RESET);
      
      if(step&(1<<3))
        STP_MotorMicroStep(hrotor, gpios->GPIOx, gpios->GPIO_PIN_X_BMinus);
      else
        HAL_GPIO_WritePin(gpios->GPIOx, gpios->GPIO_PIN_X_BMinus, GPIO_PIN_RESET);
    }
    else
    {
      HAL_GPIO_WritePin(gpios->GPIOx, gpios->GPIO_PIN_X_APlus, (step & (1<<0)) ? GPIO_PIN_SET : GPIO_PIN_RESET);		
      HAL_GPIO_WritePin(gpios->GPIOx, gpios->GPIO_PIN_X_AMinus, (step & (1<<1)) ? GPIO_PIN_SET : GPIO_PIN_RESET);	
      HAL_GPIO_WritePin(gpios->GPIOx, gpios->GPIO_PIN_X_BPlus, (step & (1<<2)) ? GPIO_PIN_SET : GPIO_PIN_RESET);	
      HAL_GPIO_WritePin(gpios->GPIOx, gpios->GPIO_PIN_X_BMinus, (step & (1<<3)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }

	
    if(hrotor->direction)
    {
      hrotor->stepCount--;
    }
    else
    {
      hrotor->stepCount++;
    }
    
    hrotor->stepCount %= hrotor->sequenceSize;
	}
	return HAL_OK;
}
#endif //USE_GPIO_PORTS

#ifdef USE_PWM_TIM
HAL_StatusTypeDef STP_MotorMicroStep(STP_RotationHandleTypeDef *hrotor, TIM_HandleTypeDef *htim, uint32_t tim_channel, uint8_t *microstep_cnt)
{
  __HAL_TIM_SET_COMPARE(htim, tim_channel, sin_table[*microstep_cnt]*__HAL_TIM_GET_AUTORELOAD(htim));
  
  (*microstep_cnt)++;
  if(*microstep_cnt>=hrotor->microStepping)
  {
    *microstep_cnt = 0;
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

static uint8_t microstep_cnt1 = 0;
static uint8_t microstep_cnt2 = 0;
static uint8_t microstep_cnt3 = 0;
static uint8_t microstep_cnt4 = 0;
static int increment_cnt = 0;
HAL_StatusTypeDef STP_MotorStep(STP_RotationHandleTypeDef *hrotor, TIM_HandleTypeDef *htim, STP_OutputsTypeDef *channels, STP_MotorModeTypeDef mode)
{
	if(hrotor == NULL)
		return HAL_ERROR;
	if(htim == NULL)
		return HAL_ERROR;
  HAL_StatusTypeDef result = HAL_OK;
  
	
	uint16_t step = hrotor->stepSequence[hrotor->stepCount];

	if(hrotor->stopFlag == 0)
	{
    if(hrotor->microStepping)
    {
      if(step&(1<<0))
        result = STP_MotorMicroStep(hrotor, htim, channels->Channel_APlus, &microstep_cnt1);
      else
        __HAL_TIM_SET_COMPARE(htim, channels->Channel_APlus, 0);
        
      if(step&(1<<1))
        result = STP_MotorMicroStep(hrotor, htim, channels->Channel_AMinus, &microstep_cnt2);
      else
        __HAL_TIM_SET_COMPARE(htim, channels->Channel_AMinus, 0);
      
      if(step&(1<<2))
        result = STP_MotorMicroStep(hrotor, htim, channels->Channel_BPlus, &microstep_cnt3);
      else
        __HAL_TIM_SET_COMPARE(htim, channels->Channel_BPlus, 0);
      
      if(step&(1<<3))
        result = STP_MotorMicroStep(hrotor, htim, channels->Channel_BMinus, &microstep_cnt4);
      else
        __HAL_TIM_SET_COMPARE(htim, channels->Channel_BMinus, 0);
    }
    else
    {
      __HAL_TIM_SET_COMPARE(htim, channels->Channel_APlus, (step & (1<<0)) ? __HAL_TIM_GET_AUTORELOAD(htim) : 0);
      __HAL_TIM_SET_COMPARE(htim, channels->Channel_AMinus, (step & (1<<1)) ? __HAL_TIM_GET_AUTORELOAD(htim) : 0);
      __HAL_TIM_SET_COMPARE(htim, channels->Channel_BPlus, (step & (1<<2)) ? __HAL_TIM_GET_AUTORELOAD(htim) : 0);
      __HAL_TIM_SET_COMPARE(htim, channels->Channel_BMinus, (step & (1<<3)) ? __HAL_TIM_GET_AUTORELOAD(htim) : 0);
    }
    
    
    if(hrotor->microStepping == 0)
    {
      if(hrotor->direction)
      {
        hrotor->stepCount--;
      }
      else
      {
        hrotor->stepCount++;
      }
    }
    else
    {
      increment_cnt = (increment_cnt + 1) % (hrotor->microStepping/mode);
      
      if(increment_cnt == 0)
      {      
        if(hrotor->direction)
        {
          hrotor->stepCount--;
        }
        else
        {
          hrotor->stepCount++;
        }
      }
    }
    hrotor->stepCount %= hrotor->sequenceSize;
  }
	return HAL_OK;
}
#endif //USE_PWM_TIM

HAL_StatusTypeDef STP_MotorHandleTIM(STP_MotorHandleTypeDef *motor)
{
  static uint8_t prev_stepcount = 0;
	if(motor == NULL)
		return HAL_ERROR;
	
	Service_SetMotorMode(motor);	
	
	Service_IsPIDInitialized(&motor->hramp);
  motor->hramp.SampleT = (float)(motor->htim->Instance->PSC+1)*(motor->htim->Instance->ARR+1)/SystemCoreClock;
	if(motor->hramp.pid_initialized && motor->hramp.pid_disable == 0)
		Service_Ramp_ControlValue(&motor->hramp, &motor->frequency);
	else
  {
		motor->frequency = motor->hramp.setpoint;    
    motor->hramp.pid.state[2] = motor->frequency;
  }
	
	Service_SetMotorFrequency(motor, motor->frequency);
	
	if(motor->frequency != 0)
  {
#ifdef USE_GPIO_PORTS
		STP_MotorStep(&motor->rhandle, &motor->Outputs);
#endif
#ifdef USE_PWM_TIM
		STP_MotorStep(&motor->rhandle, motor->htim, &motor->Outputs, motor->mode);
#endif
  }
  if(motor->rhandle.stopFlag == 1)
  {
    Service_StopMotor(motor);
  }
  
  if(prev_stepcount != motor->rhandle.stepCount)
  {
    float step = 1.0/motor->mode;
    
    if(motor->rhandle.microStepping != 0)
      step /= motor->rhandle.microStepping;    
    
    if(motor->rhandle.direction == 0)
      motor->rhandle.currentStep += step;
    else
      motor->rhandle.currentStep -= step;
    
    
    
    if(motor->rhandle.currentStep > motor->rhandle.totalSteps)
      motor->rhandle.currentStep = 0;
    else if(motor->rhandle.currentStep < 0)
      motor->rhandle.currentStep = motor->rhandle.totalSteps;
    
    motor->rhandle.goFlag = 1;  
  }
  else
  {
    motor->rhandle.goFlag = 0;  
  }
  prev_stepcount = motor->rhandle.stepCount;
	
	return HAL_OK;
}


static void Service_StopMotor(STP_MotorHandleTypeDef *motor)
{
#ifdef USE_GPIO_PORTS
		HAL_GPIO_WritePin(motor->Outputs.GPIOx, motor->Outputs.GPIO_PIN_X_APlus, GPIO_PIN_RESET);	
		HAL_GPIO_WritePin(motor->Outputs.GPIOx, motor->Outputs.GPIO_PIN_X_AMinus, GPIO_PIN_RESET);	
		HAL_GPIO_WritePin(motor->Outputs.GPIOx, motor->Outputs.GPIO_PIN_X_BPlus, GPIO_PIN_RESET);		
		HAL_GPIO_WritePin(motor->Outputs.GPIOx, motor->Outputs.GPIO_PIN_X_BMinus, GPIO_PIN_RESET);
#endif
#ifdef USE_PWM_TIM
    __HAL_TIM_SET_COMPARE(motor->htim, motor->Outputs.Channel_APlus, 0);
    __HAL_TIM_SET_COMPARE(motor->htim, motor->Outputs.Channel_AMinus, 0);
    __HAL_TIM_SET_COMPARE(motor->htim, motor->Outputs.Channel_BPlus, 0);
    __HAL_TIM_SET_COMPARE(motor->htim, motor->Outputs.Channel_BMinus, 0);
#endif
}

static HAL_StatusTypeDef Service_SetMotorFrequency(STP_MotorHandleTypeDef *motor, float Speed)
{
	static float speed_prev = 0;
	
	if(speed_prev == Speed)
		return HAL_OK;  
	speed_prev = Speed;
  
	if(Speed > 0)
		motor->rhandle.direction = 0;
	else
		motor->rhandle.direction = 1;
	
	if(Speed !=0)
	{		
		// calc the period to get setted speed
    uint32_t period = STP_GET_TIM_CLK_FREQ(motor->htim) / fabs(Speed); 
    period = period/motor->rhandle.totalSteps/motor->mode;
		
#ifdef USE_PWM_TIM
    period /= 2;
    if(IS_MICROSTEP_VALID(motor->rhandle.microStepping))
    {
      period /= (motor->rhandle.microStepping);      
      if(motor->mode == STP_MODE_HALF_STEP)
        period *= 4;
    }
#endif
    
		if(period <= 0xFFFF)
		{
			// set ARR to corresponding speed 1 ms
			__HAL_TIM_SET_AUTORELOAD(motor->htim, period);
			// run motor
			motor->rhandle.stopFlag = 0;
		}
		else
		{
			// set ARR to interrupt 1 ms
			period = STP_GET_TIM_CLK_FREQ(motor->htim)/1000; 
			if(period <= 0xFFFF)
				__HAL_TIM_SET_AUTORELOAD(motor->htim, period);
			else
			{
				__HAL_TIM_SET_AUTORELOAD(motor->htim, 0xFFFF);
			}
			// stop motor
			motor->rhandle.stopFlag = 1;
		}
	}
	else
	{
		motor->rhandle.stopFlag = 1;
	}
	return HAL_OK;
}


static void Service_SetMotorMode(STP_MotorHandleTypeDef *motor)
{
  static STP_MotorModeTypeDef mode_shdw;
	switch(motor->mode)
	{
		case STP_MODE_FULL_STEP:
			motor->rhandle.sequenceSize = 4;
			motor->rhandle.stepSequence = sequenceFullStep;
      if(motor->mode != mode_shdw)
      {
        if(motor->rhandle.microStepping)
        {
          increment_cnt = 0;
          microstep_cnt1 = 0;
          microstep_cnt2 = 0;
          microstep_cnt3 = 0;
          microstep_cnt4 = 0;
          motor->rhandle.stepCount = 0;
        }
      }
			break;
	
		case STP_MODE_HALF_STEP:
      if(motor->rhandle.microStepping == 0)
      {
        motor->rhandle.sequenceSize = 8;
        motor->rhandle.stepSequence = sequenceHalfStep;
      }
      else
      {
        motor->rhandle.sequenceSize = 4;
        motor->rhandle.stepSequence = sequenceHalfStepSine;
        if(motor->mode != mode_shdw)
        {
          increment_cnt = 0;
          microstep_cnt1 = motor->rhandle.microStepping/2;
          microstep_cnt2 = 0;
          microstep_cnt3 = 0;
          microstep_cnt4 = 0;
          motor->rhandle.stepCount = 0;
        }
      }
			break;
		
		default:
			motor->mode = STP_MODE_UNKNOWN;
			motor->rhandle.sequenceSize = 0;
			motor->rhandle.stepSequence = sequenceFullStep;
	}
  mode_shdw = motor->mode;
}


static void Service_Ramp_ControlValue(STP_PIDHandleTypeDef *hramp, float *PID_Output) 
{
	float out;
	float err = hramp->setpoint-*PID_Output;
	
	/* Limiting the accelerate */
	if (err > hramp->limMaxInt)
		err = hramp->limMaxInt;
	else if (err < hramp->limMinInt)
		err = hramp->limMinInt;
	
	out = arm_pid_f32(&hramp->pid, err);
	
	/* Limiting the accelerate */
	if (out > hramp->limMax)
		out = hramp->limMax;
	else if (out < hramp->limMin)
		out = hramp->limMin;
	
	/* Apply the PID-regulating */
	if(hramp->setpoint != 0)
	{
		arm_pid_init_f32(&hramp->pid, 0);
		*PID_Output = out;
	}
	else // if input = 0 - disable pid and set 0
	{
		arm_pid_reset_f32(&hramp->pid);
		*PID_Output = 0;
	}
}


static void Service_IsPIDInitialized(STP_PIDHandleTypeDef *hramp)
{
	hramp->pid_initialized = 1;
	
	if((hramp->pid.Kp == 0) && (hramp->pid.Ki == 0) && (hramp->pid.Kd == 0)) 
	{
		hramp->pid_initialized = 0;
	}
	if((hramp->limMax == 0) && (hramp->limMin == 0)) 
	{
		hramp->pid_initialized = 0;
	}
	if((hramp->limMinInt == 0) && (hramp->limMaxInt == 0)) 
	{
		hramp->pid_initialized = 0;
	}
}
