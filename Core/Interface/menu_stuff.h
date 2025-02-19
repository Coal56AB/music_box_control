#ifndef MENU_STUFF_H
#define MENU_STUFF_H

#include "stm32f1xx_hal.h"
#include <PixelGraphics.h>
#include "general_gpio.h"
#include "main.h"

//#define GPIO_CONTROL


#define font_size 				10 //refer to font_tahoma_8_prop

#define displaycenter_x		(64)
#define displaycenter_y		(18)

#define circle_stopplay_x_shift	(0)
#define circle_stopplay_y_shift	(2)

#define  circle_stopplay_x		(displaycenter_x + circle_stopplay_x_shift)
#define  circle_stopplay_y		(displaycenter_y + circle_stopplay_y_shift)
#define  circle_stopplay_r		(9)

#define	 play_icon_x_shift		(3)
#define	 play_icon_y_size			(8)

#define	 play_icon_x_left			(circle_stopplay_x - play_icon_x_shift)
#define	 play_icon_x_rigth		(circle_stopplay_x + play_icon_x_shift)
#define	 play_icon_y_up				(circle_stopplay_y - (play_icon_y_size/2))
#define	 play_icon_y_down			(circle_stopplay_y + (play_icon_y_size/2))
#define	 play_icon_y_mid			(circle_stopplay_y)


#define	 forward_backward_icon_x_shift	(15)


#define	 forward_icon_x_left	(play_icon_x_left + forward_backward_icon_x_shift)
#define	 forward_icon_x_rigth	(play_icon_x_rigth + forward_backward_icon_x_shift+3)
#define	 forward_icon_y_up		(play_icon_y_up)
#define	 forward_icon_y_down	(play_icon_y_down)
#define	 forward_icon_y_mid		(play_icon_y_mid)

#define	 backward_icon_x_left	(play_icon_x_left - forward_backward_icon_x_shift-3)
#define	 backward_icon_x_rigth (play_icon_x_rigth - forward_backward_icon_x_shift)
#define	 backward_icon_y_up		(play_icon_y_up)
#define	 backward_icon_y_down	(play_icon_y_down)
#define	 backward_icon_y_mid	(play_icon_y_mid)


#define  speed_x_shift			(39)
#define  speed_y_shift			(-3)
#define  speed_x_width			(24)

#define  speed_x_cursore					(displaycenter_x+speed_x_shift)
#define  speed_y_cursore					(displaycenter_y+speed_y_shift)
#define  speed_underline_x_left		(speed_x_cursore-1)
#define  speed_underline_x_rigth	(speed_underline_x_left+speed_x_width)
#define  speed_underline_y				(speed_y_cursore+font_size)

#define  loop_icon_x_shift	(speed_x_shift+2)
#define  loop_icon_y_shift	(-2)

#define  loop_icon_x_start				(displaycenter_x-loop_icon_x_shift-loop_icon_width)
#define  loop_icon_y_start				(displaycenter_y+loop_icon_y_shift)
#define  loop_icon_width					(10)
#define  loop_icon_height					8

#define  selected_width           (1)



#define GPIO_SwSpeed						SW_SPEED_GPIO_Port
#define GPIO_Pin_SwSpeed				SW_SPEED_Pin

#define GPIO_SwForward					SW_FORWARD_GPIO_Port
#define GPIO_Pin_SwForward			SW_FORWARD_Pin

#define GPIO_SwPlay							SW_PLAY_GPIO_Port
#define GPIO_Pin_SwPlay					SW_PLAY_Pin

#define GPIO_SwBackward					SW_BACKWARD_GPIO_Port
#define GPIO_Pin_SwBackward			SW_BACKWARD_Pin

#define GPIO_SwLoop							SW_LOOP_GPIO_Port
#define GPIO_Pin_SwLoop					SW_LOOP_Pin

typedef struct
{
	unsigned 	play:1;
	unsigned 	loop:1;
	unsigned 	pressed_start:1;
	unsigned 	press_forward:1;
	unsigned 	press_backward:1;
	float			currenttime;
	float			speed;
	
	
	GPIO_SwitchTypeDef	SwPlay;
	GPIO_SwitchTypeDef	SwForward;
	GPIO_SwitchTypeDef	SwBackward;
	GPIO_SwitchTypeDef	SwSpeed;
	GPIO_SwitchTypeDef	SwLoop;
}PlayerTypeDef;
extern PlayerTypeDef Player;


void LCD_CreateFrame(PlayerTypeDef *player);

void Menu_Control_Init(PlayerTypeDef *player);
void Menu_Control(PlayerTypeDef *player);

#endif //MENU_LCD_H
