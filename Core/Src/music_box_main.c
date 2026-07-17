#include "music_box_main.h"

MusicBoxHandleTypeDef musicbox;

void SaveCurrenttime(int currtime);
void ReadCurrenttime(float *currtime);


void music_box_init(void)
{
  musicbox.rotationFullNumb = ROTATIONS_FULL_MELODY;
  
  musicbox.rotationStep = (float)1/musicbox.motor.rhandle.totalSteps/musicbox.rotationFullNumb;
  
	Menu_Control_Init(&musicbox.player);
//  oled_demo_run();
//	HAL_TIM_PWM_Start(&hpwmtim, TIM_CHANNEL_1);
//	HAL_TIM_PWM_Start(&hpwmtim, TIM_CHANNEL_2);
	STP_SetMotorPIDParams(&musicbox.motor, 0, 0.002, 0);
	STP_SetMotorLimits(&musicbox.motor, 1000, -1000, 1, -1);
#ifdef USE_GPIO_PORTS
	STP_SetMotorPins(&musicbox.motor, GPIOA, GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11);	
#endif
#ifdef USE_PWM_TIM
	STP_SetMotorPins(&musicbox.motor, &htim1, TIM_CHANNEL_1, TIM_CHANNEL_2, TIM_CHANNEL_3, TIM_CHANNEL_4);	
#endif
	STP_SetMotorMode(&musicbox.motor, STP_MODE_HALF_STEP, 0); 
	STP_MotorInit(&musicbox.motor, &htim1, 200);
	STP_MotorStart(&musicbox.motor);
	oled_init();
  musicbox.player.loop = 1;
  musicbox.player.play = 0;
	
	ReadCurrenttime(&musicbox.player.currenttime);
	musicbox.initialized = 1;
}

uint8_t superfinesteps = 64;
uint8_t finesteps = 16;
uint8_t regularsteps = 0;
STP_MotorModeTypeDef motormode_superfinesteps = STP_MODE_FULL_STEP;
STP_MotorModeTypeDef motormode_finesteps = STP_MODE_FULL_STEP;
STP_MotorModeTypeDef motormode_regularsteps = STP_MODE_HALF_STEP;
static float prevstep = 0;
void music_box_main(void)
{	
  musicbox.rotationStep = (float)1/musicbox.rotationFullNumb/musicbox.motor.rhandle.totalSteps;
  
	static int prev_currtime = 0;
	static int intcurrtime = 0;
	
	intcurrtime = (int)(musicbox.player.currenttime*CURRTIME_PRECISE);
	if(intcurrtime != prev_currtime)
	{
		SaveCurrenttime(intcurrtime);
		prev_currtime = intcurrtime;
	}
	
  
  if(musicbox.motor.rhandle.currentStep != prevstep)
  {
		float diff;
		if(musicbox.motor.rhandle.currentStep > prevstep)
			diff = musicbox.motor.rhandle.currentStep - prevstep;
		else
			diff = prevstep - musicbox.motor.rhandle.currentStep;
		
		if(diff > musicbox.motor.rhandle.totalSteps/2)
			diff = musicbox.motor.rhandle.totalSteps - diff;
		
    if(musicbox.motor.rhandle.direction == 0)
      musicbox.player.currenttime += musicbox.rotationStep*diff;
    else
      musicbox.player.currenttime -= musicbox.rotationStep*diff;
    
    prevstep = musicbox.motor.rhandle.currentStep;
  }
	
	
  
  
  if(musicbox.player.currenttime > 1)
  {
    musicbox.player.currenttime = 0;
    if(musicbox.player.loop == 0)
      musicbox.player.play = 0;
  }
  else if(musicbox.player.currenttime < 0)
  {
    if(musicbox.player.loop)
      musicbox.player.currenttime = 1;
    else
      musicbox.player.currenttime = 0;
  }
  
	// если шкатулка запущена
	if(musicbox.player.play)
	{
		// если кнопки перемотки не нажаты, то подтягивает заданную скорость воспроизведения
		if((musicbox.player.pressed_backward == 0) && (musicbox.player.pressed_forward == 0) )
		{
			musicbox.freq = SPEED_1_MOTOR_FREQ*musicbox.player.speed;
			musicbox.motor.hramp.pid_disable = 1;
		}
	}
	else
	{
		musicbox.freq = 0;
	}
  
  
  // перемотка назад
  if((musicbox.player.pressed_backward == 1) && (musicbox.player.pressed_forward == 0) )
  {
    if(musicbox.player.currenttime > 0)
    {
			musicbox.freq = 0;
//      musicbox.freq = -SPEED_1_MOTOR_FREQ;
//      musicbox.motor.hramp.pid_disable = 0;
//      musicbox.motor.hramp.pid.Ki = 0.02;
    }
    else
    {
      musicbox.freq = 0;
    }
  }
  
  // перемотка вперед
  if((musicbox.player.pressed_forward == 1) && (musicbox.player.pressed_backward == 0) )
  {
    musicbox.freq = SPEED_MAX_MOTOR_FREQ;
    musicbox.motor.hramp.pid_disable = 0;
    musicbox.motor.hramp.pid.Ki = 0.02;
  }
	
  // выставление микрошага режима если требуется
  if(musicbox.freq != 0)
  {
    if(fabs(musicbox.freq) <= FREQ_SUPERMICROSTEP_THRESHOLD)
      STP_SetMotorMode(&musicbox.motor, motormode_superfinesteps, superfinesteps);
    else if(fabs(musicbox.freq) <= FREQ_MICROSTEP_THRESHOLD)
      STP_SetMotorMode(&musicbox.motor, motormode_finesteps, finesteps);
    else
      STP_SetMotorMode(&musicbox.motor, motormode_regularsteps, regularsteps);    
  }
  
  STP_SetMotorFrequency(&musicbox.motor, musicbox.freq);
  
  
  
	static uint32_t oled_prevtick = 0;
	uint32_t oled_period = 50;
	if(uwTick - oled_prevtick > oled_period)
	{
		oled_prevtick = uwTick;
		LCD_CreateFrame(&musicbox.player);
    oled_refresh();
	}
  
	if((musicbox.player.pressed_forward == 1) && (musicbox.player.pressed_backward == 1) && (musicbox.player.pressed_start == 1) )
	{
		musicbox.player.currenttime = 0;
		musicbox.player.play = 0;
	}
}

void Flash_ErasePage(void)
{
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
    while (FLASH->SR & FLASH_SR_BSY);
    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR = CURRENTSTEP_ADDRESS;
    FLASH->CR |= FLASH_CR_STRT;
    while (FLASH->SR & FLASH_SR_BSY);
    FLASH->CR &= ~FLASH_CR_PER;
    FLASH->CR |= FLASH_CR_LOCK;
}
void Flash_WriteByte(uint8_t data)
{
    uint32_t addr = CURRENTSTEP_ADDRESS;
    while (*(uint8_t*)addr != 0xFF && addr < CURRENTSTEP_ADDRESS + FLASH_PAGE_SIZE-2)
    {
        addr++;
    }
    
    if (addr >= CURRENTSTEP_ADDRESS + FLASH_PAGE_SIZE-2) 
    {
        Flash_ErasePage();
        addr = CURRENTSTEP_ADDRESS;
    }
    
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
    while (FLASH->SR & FLASH_SR_BSY);
    FLASH->CR |= FLASH_CR_PG;
    *(__IO uint16_t*)addr = (uint16_t)data;
    while (FLASH->SR & FLASH_SR_BSY);
    FLASH->CR &= ~FLASH_CR_PG;
    FLASH->CR |= FLASH_CR_LOCK;
}

void SaveCurrenttime(int currtime)
{
	__disable_irq();
	Flash_WriteByte(currtime);
	__enable_irq();

}

uint8_t Flash_ReadLastByte(void)
{
    uint32_t addr = CURRENTSTEP_ADDRESS;
    uint32_t lastValidAddr = CURRENTSTEP_ADDRESS;
    while (*(uint8_t*)addr != 0xFF && addr < CURRENTSTEP_ADDRESS + FLASH_PAGE_SIZE-2)
    {
        lastValidAddr = addr;
        addr++;
    }
    return *(uint16_t*)(lastValidAddr-1);
}

void ReadCurrenttime(float *currtime)
{
	uint8_t uintcurrtime = 0;
	
	__disable_irq();
	uintcurrtime = Flash_ReadLastByte();
	
	*currtime = (float)uintcurrtime/CURRTIME_PRECISE;
	__enable_irq();

}