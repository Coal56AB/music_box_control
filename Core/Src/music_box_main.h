#include "stm32f1xx_hal.h"
#include "menu_stuff.h"
#include "STPMotor.h"
#include "tim.h"

#define SPEED_1_MOTOR_FREQ			((float)1.8)//1.742)
#define SPEED_MAX_MOTOR_FREQ		((float)3.5)
#define ROTATIONS_FULL_MELODY	  ((float)10)
#define FREQ_MICROSTEP_THRESHOLD ((float)0.5)

typedef struct
{
	float 									freq;
	STP_MotorHandleTypeDef 	motor;
	PlayerTypeDef						player;
  
  float                   rotationFullNumb;
  float                   rotationStep;
}MusicBoxHandleTypeDef;
extern MusicBoxHandleTypeDef musicbox;


void music_box_init(void);
void music_box_main(void);