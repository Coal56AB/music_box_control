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
}

void Menu_Control(PlayerTypeDef *player)
{
	

}