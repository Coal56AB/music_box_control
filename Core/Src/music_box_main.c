#include "music_box_main.h"

MusicBoxHandleTypeDef musicbox;



void music_box_init(void)
{
	Menu_Control_Init(&musicbox.player);
//  oled_demo_run();
//	HAL_TIM_PWM_Start(&hpwmtim, TIM_CHANNEL_1);
//	HAL_TIM_PWM_Start(&hpwmtim, TIM_CHANNEL_2);
	STP_SetMotorPIDParams(&musicbox.motor, 0, 0.002, 0);
	STP_SetMotorLimits(&musicbox.motor, 1000, -1000, 0.5, -0.5);
#ifdef USE_GPIO_PORTS
	STP_SetMotorPins(&musicbox.motor, GPIOA, GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11);	
#endif
#ifdef USE_PWM_TIM
	STP_SetMotorPins(&musicbox.motor, &htim1, TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3, TIM_CHANNEL_4);	
#endif
	STP_SetMotorMode(&musicbox.motor, STP_MODE_HALF_STEP, 16);
	STP_MotorInit(&musicbox.motor, &htim1, 200);
	STP_MotorStart(&musicbox.motor);
	
}

uint8_t microsteps = 128;
STP_MotorModeTypeDef motormode = STP_MODE_FULL_STEP;
void music_box_main(void)
{	
	// если шкатулка запущена
	if(musicbox.player.play)
	{
		// если кнопки перемотки не нажаты, то подтягивает заданную скорость воспроизведения
		if((musicbox.player.press_backward == 0) && (musicbox.player.press_forward == 0) )
		{
			musicbox.freq = SPEED_1_MOTOR_FREQ*musicbox.player.speed;
			musicbox.motor.hramp.pid_disable = 1;
			
			if(musicbox.freq <= 1.5)
				STP_SetMotorMode(&musicbox.motor, motormode, microsteps);
			else
				STP_SetMotorMode(&musicbox.motor, STP_MODE_HALF_STEP, 0);
		}
		else
		{
			// перемотка назад
			if((musicbox.player.press_backward == 1) && (musicbox.player.press_forward == 0) )
			{
				musicbox.freq = -SPEED_1_MOTOR_FREQ;
				musicbox.motor.hramp.pid_disable = 0;
				musicbox.motor.hramp.pid.Ki = 0.05;
				STP_SetMotorMode(&musicbox.motor, STP_MODE_HALF_STEP, 0);
			}
			
			// перемотка вперед
			if((musicbox.player.press_forward == 1) && (musicbox.player.press_backward == 0) )
			{
				musicbox.freq = SPEED_MAX_MOTOR_FREQ;
				musicbox.motor.hramp.pid_disable = 0;
				musicbox.motor.hramp.pid.Ki = 0.005;
				STP_SetMotorMode(&musicbox.motor, STP_MODE_HALF_STEP, 0);
			}
		}
	}
	else
	{
		musicbox.freq = 0;
	}
	
	STP_SetMotorFrequency(&musicbox.motor, musicbox.freq);
}
