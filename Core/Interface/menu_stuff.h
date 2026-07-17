#ifndef MENU_STUFF_H
#define MENU_STUFF_H

#include "stm32f1xx_hal.h"
#include <PixelGraphics.h>
#include "general_gpio.h"
#include "main.h"

//#define GPIO_CONTROL


#define font_size 				10 //refer to font_tahoma_8_prop

#define displaycenter_x		(62)
#define displaycenter_y		(18)


#define control_panel_y_shift   (2)
#define control_panel_y_height  (8)
#define control_panel_y_mid     (displaycenter_y + control_panel_y_shift)
#define control_panel_y_up      (control_panel_y_mid - (control_panel_y_height/2))
#define control_panel_y_down    (control_panel_y_mid + (control_panel_y_height/2))

#define	 play_icon_x_widht		(3)
#define	 play_icon_y_size			control_panel_y_height//(8)

#define	 play_icon_x_left			(displaycenter_x - play_icon_x_widht)
#define	 play_icon_x_rigth		(displaycenter_x + play_icon_x_widht)
#define	 play_icon_y_up				(control_panel_y_up)
#define	 play_icon_y_down			(control_panel_y_down)
#define	 play_icon_y_mid			(control_panel_y_mid)


#define	 forward_backward_icon_x_shift	(18)


#define	 forward_icon_x_left	(play_icon_x_left + forward_backward_icon_x_shift)
#define	 forward_icon_x_rigth	(play_icon_x_rigth + forward_backward_icon_x_shift+3)
#define	 forward_icon_y_up		(control_panel_y_up)
#define	 forward_icon_y_down	(control_panel_y_down)
#define	 forward_icon_y_mid		(control_panel_y_mid)

#define	 backward_icon_x_left	(play_icon_x_left - forward_backward_icon_x_shift-3)
#define	 backward_icon_x_rigth (play_icon_x_rigth - forward_backward_icon_x_shift)
#define	 backward_icon_y_up		(control_panel_y_up)
#define	 backward_icon_y_down	(control_panel_y_down)
#define	 backward_icon_y_mid	(control_panel_y_mid)


#define  speed_x_shift			(39)
#define  speed_y_shift			(-3)

#define  speed_x_cursore					(displaycenter_x+speed_x_shift)
#define  speed_y_cursore					(displaycenter_y+speed_y_shift)

#define  loop_icon_x_shift	(speed_x_shift+3)
#define  loop_icon_y_shift	(-2)

#define  loop_icon_x_start				(displaycenter_x-loop_icon_x_shift-loop_icon_width)
#define  loop_icon_y_start				(displaycenter_y+loop_icon_y_shift)
#define  loop_icon_width					(11)
#define  loop_icon_height					8

#define  selected_width           (1)

#define GPIO_SwTheme						SW_THEME_GPIO_Port
#define GPIO_Pin_SwTheme				SW_THEME_Pin

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
	unsigned 	pressed_forward:1;
	unsigned 	pressed_backward:1;
	unsigned 	pressed_loop:1;
	unsigned 	pressed_speed:1;
	float			currenttime;
	float			speed;
	
	
	GPIO_SwitchTypeDef	SwTheme;
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
