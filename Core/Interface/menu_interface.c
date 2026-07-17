#include "menu_stuff.h"

#define THEME_ADDRESS			0x0801F800

#define GPIO_CONTROL
#define SPEED_SWITCH
//#define SPEED_ANALOG
void StoreTheme(uint16_t theme);
uint16_t ReadTheme(void);
extern int menu_white_theme;
typedef enum
{
	SPEED_SLOW,
	SPEED_NORMAL,
	SPEED_FAST,
	SPEED_VERY_FAST
}MotorSpeedTypeDef;

void Menu_Control_Init(PlayerTypeDef *player)
{
	GPIO_Switch_Init(&player->SwPlay, GPIO_SwPlay, GPIO_Pin_SwPlay, SW_ON);
	GPIO_Switch_Init(&player->SwForward, GPIO_SwForward, GPIO_Pin_SwForward, SW_ON);
	GPIO_Switch_Init(&player->SwBackward, GPIO_SwBackward, GPIO_Pin_SwBackward, SW_ON);
	GPIO_Switch_Init(&player->SwSpeed, GPIO_SwSpeed, GPIO_Pin_SwSpeed, SW_ON);
	GPIO_Switch_Init(&player->SwLoop, GPIO_SwLoop, GPIO_Pin_SwLoop, SW_ON);	
	
  player->SwTheme.Sw_FilterDelay = 70;
  player->SwPlay.Sw_FilterDelay = 70;
  player->SwForward.Sw_FilterDelay = 70;
  player->SwBackward.Sw_FilterDelay = 70;
  player->SwSpeed.Sw_FilterDelay = 70;
  player->SwLoop.Sw_FilterDelay = 70;
  
	player->speed = 1;
	
	menu_white_theme = ReadTheme();
}

void Menu_Control(PlayerTypeDef *player)
{
#ifdef GPIO_CONTROL	
	
	/* Обработка кнопки плей */
	if(GPIO_Read_Swich(&player->SwPlay))
	{
		if(player->SwPlay.Sw_PrevState == 0)
		{
			player->play ^= 1;
			player->pressed_start = 1;
		}			
	}
	else
	{
		player->pressed_start = 0;
	}

	/* Обработка кнопки прокрутки вперед */
	if(GPIO_Read_Swich(&player->SwForward))
	{
		player->pressed_forward = 1;
	}
	else
	{
		player->pressed_forward = 0;
	}
	
	/* Обработка кнопки прокрутки назад */
	if(GPIO_Read_Swich(&player->SwBackward))
	{
		player->pressed_backward = 1;
	}
	else
	{
		player->pressed_backward = 0;
	}
	
	/* Обработка кнопки зацикливания */
	if(GPIO_Read_Swich(&player->SwLoop))
	{
		if(player->SwLoop.Sw_PrevState == 0)
		{
			player->loop ^= 1;
			player->pressed_loop = 1;
		}
	}
	else
	{
		player->pressed_loop = 0;
	}
	
	/* Обработка кнопки скорости */
	if(GPIO_Read_Swich(&player->SwSpeed))
	{
		if(player->SwSpeed.Sw_PrevState == 0)
		{
			if(player->speed<1)
				player->speed += 0.2;
			else
				player->speed += 0.25;
						
			player->pressed_speed = 1;
		}
		
		if(player->speed > 1.755)
			player->speed = 1;		
	}
	else
	{
		player->pressed_speed = 0;
	}
  
	/* Обработка кнопки темы меню */
	static int flag_update_menu = 0;
	if(player->pressed_backward && player->pressed_forward && player->pressed_loop && player->pressed_start && player->pressed_speed)
	{
		if(flag_update_menu)
		{
			menu_white_theme ^= 1;
			flag_update_menu = 0;
			StoreTheme(menu_white_theme);
		}
	}
	else
	{		
		if((player->pressed_backward || player->pressed_forward || player->pressed_loop || player->pressed_start || player->pressed_speed) == 0)
			flag_update_menu = 1;
	}
#endif
}
void Flash_EraseTheme(void)
{
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
    while (FLASH->SR & FLASH_SR_BSY);
    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR = THEME_ADDRESS;
    FLASH->CR |= FLASH_CR_STRT;
    while (FLASH->SR & FLASH_SR_BSY);
    FLASH->CR &= ~FLASH_CR_PER;
    FLASH->CR |= FLASH_CR_LOCK;
}
void StoreTheme(uint16_t theme)
{
	uint16_t data;
	if(theme)
		data = 0xAA00;
	else
		data = 0x00AA;
	
	
	__disable_irq();

	Flash_EraseTheme();	
	FLASH->KEYR = FLASH_KEY1;
	FLASH->KEYR = FLASH_KEY2;
	while (FLASH->SR & FLASH_SR_BSY);
	FLASH->CR |= FLASH_CR_PG;
	*(__IO uint16_t*)THEME_ADDRESS = (uint16_t)data;
	while (FLASH->SR & FLASH_SR_BSY);
	FLASH->CR &= ~FLASH_CR_PG;
	FLASH->CR |= FLASH_CR_LOCK;
	
	__enable_irq();
}
	
uint16_t ReadTheme(void)
{
	uint32_t addr = THEME_ADDRESS;
	uint16_t theme = *(uint16_t*)(addr);
	
	if(theme == 0xAA00)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}