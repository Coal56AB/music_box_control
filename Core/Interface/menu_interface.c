#include "menu_stuff.h"


#define SPEED_SWITCH
//#define SPEED_ANALOG


void Menu_Control_Init(PlayerTypeDef *player)
{
	GPIO_Switch_Init(&player->SwPlay, GPIO_SwPlay, GPIO_Pin_SwPlay, SW_ON);
	GPIO_Switch_Init(&player->SwForward, GPIO_SwForward, GPIO_Pin_SwForward, SW_ON);
	GPIO_Switch_Init(&player->SwBackward, GPIO_SwBackward, GPIO_Pin_SwBackward, SW_ON);
	GPIO_Switch_Init(&player->SwSpeed, GPIO_SwSpeed, GPIO_Pin_SwSpeed, SW_ON);
	GPIO_Switch_Init(&player->SwLoop, GPIO_SwLoop, GPIO_Pin_SwLoop, SW_ON);	
	
	player->speed = 1;
}

void Menu_Control(PlayerTypeDef *player)
{
#ifdef GPIO_CONTROL
	/* Обработка кнопки плей */
	if(GPIO_Read_Swich(&player->SwPlay))
	{
		if(player->SwPlay.Sw_GrandPrevState == 0)
			player->play ^= 1;
			
		player->pressed_start = 1;
	}
	else
	{
		player->pressed_start = 0;
	}

	/* Обработка кнопки прокрутки вперед */
	if(GPIO_Read_Swich(&player->SwForward))
	{
		player->press_forward = 1;
	}
	else
	{
		player->press_forward = 0;
	}
	
	/* Обработка кнопки прокрутки назад */
	if(GPIO_Read_Swich(&player->SwBackward))
	{
		player->press_backward = 1;
	}
	else
	{
		player->press_backward = 0;
	}
	
	/* Обработка кнопки зацикливания */
	if(GPIO_Read_Swich(&player->SwLoop))
	{
		if(player->SwLoop.Sw_GrandPrevState == 0)
			player->loop ^= 1;
	}
	
	/* Обработка кнопки скорости */
	if(GPIO_Read_Swich(&player->SwSpeed))
	{
		if(player->SwSpeed.Sw_GrandPrevState == 0)			
			player->speed += 0.25;
		
		
		if(player->speed > 1.55)
			player->speed = 0.75;
		
	}
  
#endif
}