#include "menu_stuff.h"

static int menu_white_theme = 0;

void Graph_Draw_LoopIcon(uint8_t *Buffer_Frame, uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t pxColor);

void LCD_CreateFrame(PlayerTypeDef *player)
{
	Graph_Clean_Buffer_Frame(oled_buf, sizeof(oled_buf));
	
	// название песни
	Graph_Output_String(oled_buf, 0, 0, "Harry Potter Theme", 0, 0);
	
	// кнопка стоп/плей
//	Graph_Draw_Circle(oled_buf, circle_stopplay_x, circle_stopplay_y, circle_stopplay_r, 1);
	
	// иконка "плей"
	if(player->play)
	{ 
		Graph_Draw_Line(oled_buf, play_icon_x_left+1, play_icon_y_up, play_icon_x_left+1, play_icon_y_down, 1);
		Graph_Draw_Line(oled_buf, play_icon_x_rigth, play_icon_y_up, play_icon_x_rigth, play_icon_y_down, 1);
	}
	// иконка "стоп"
	else
		{ 
		Graph_Draw_Triangle(oled_buf, play_icon_x_left-1, play_icon_y_up, play_icon_x_left-1, play_icon_y_down, play_icon_x_rigth+1, play_icon_y_mid, 1);
	}
	if(player->pressed_start)
		Graph_Invertsion_RectangleArea(oled_buf, play_icon_x_left-1-selected_width, play_icon_y_up-selected_width, play_icon_x_rigth+2-(play_icon_x_left-1)+2*selected_width+1, play_icon_y_down-play_icon_y_up+2*selected_width+1);
		
	
	// дорожка песни
	Graph_Draw_Line(oled_buf, 0, Graph_Height-2, player->currenttime*Graph_Width, Graph_Height-2, 1);
	
	
	// перемотка вперед
  Graph_Draw_Triangle(oled_buf, forward_icon_x_left, forward_icon_y_up, forward_icon_x_left, forward_icon_y_down, forward_icon_x_rigth-1, forward_icon_y_mid, 1);
  Graph_Draw_Line(oled_buf, forward_icon_x_rigth, forward_icon_y_up, forward_icon_x_rigth, forward_icon_y_down, 1);
	if(player->press_forward)
		Graph_Invertsion_RectangleArea(oled_buf, forward_icon_x_left-selected_width, forward_icon_y_up-selected_width, forward_icon_x_rigth-forward_icon_x_left+2*selected_width+1, forward_icon_y_down-forward_icon_y_up+2*selected_width+1);
		
		
	// перемотка назад
	Graph_Draw_Triangle(oled_buf, backward_icon_x_rigth, backward_icon_y_up, backward_icon_x_rigth, backward_icon_y_down, backward_icon_x_left+1, backward_icon_y_mid, 1);
	Graph_Draw_Line(oled_buf, backward_icon_x_left, backward_icon_y_up, backward_icon_x_left, backward_icon_y_down, 1);
	if(player->press_backward)
		Graph_Invertsion_RectangleArea(oled_buf, backward_icon_x_left-selected_width, backward_icon_y_up-selected_width, backward_icon_x_rigth-(backward_icon_x_left)+2*selected_width+1, backward_icon_y_down-backward_icon_y_up+2*selected_width+1);
	
	// индикация скорости
	char buff_speed[5];
	sprintf(buff_speed, "x%.1f", player->speed);
	if((player->speed < 1.05) && player->speed > 0.95)
		Graph_Output_String(oled_buf, speed_x_cursore, speed_y_cursore, buff_speed, 0, 0);
	else
	{		
		Graph_Output_String(oled_buf, speed_x_cursore, speed_y_cursore, buff_speed, 0, 1);
	}
//	Graph_Draw_Line(oled_buf, speed_underline_x_left, speed_underline_y, speed_underline_x_rigth, speed_underline_y, 1);
	
	// иконка зацикливания
	Graph_Draw_LoopIcon(oled_buf, loop_icon_x_start, loop_icon_y_start, loop_icon_width, loop_icon_height, 1);
	if(player->loop)
		Graph_Invertsion_RectangleArea(oled_buf, loop_icon_x_start+3, loop_icon_y_start+3, loop_icon_width-5, loop_icon_height-5);
//		Graph_Invertsion_RectangleArea(oled_buf, loop_icon_x_start-selected_width, loop_icon_y_start-selected_width-2, loop_icon_width+2*selected_width+1, loop_icon_height+2*selected_width+2*2);

  
  
  if(menu_white_theme)
  {
		Graph_Invertsion_RectangleArea(oled_buf, 0, 0, 128, 32);
  }


}



/* Функция рисования значка "зациклить песню" */
void Graph_Draw_LoopIcon(uint8_t *Buffer_Frame, uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t pxColor)
{
  #define arr_size 2
  #define spot_width 2
    uint8_t r = (width < height ? width : height) / 4; // Радиус закруглений ограничен минимальным размером

    // Четыре дуги по углам
    Graph_Draw_Arc(Buffer_Frame, x + width - r, y + r, r, 270, 360, pxColor);   // Верхний правый угол
    Graph_Draw_Arc(Buffer_Frame, x + r, y + r, r, 180, 270, pxColor);           // Верхний левый угол
    Graph_Draw_Arc(Buffer_Frame, x + r, y + height - r, r, 90, 180, pxColor);   // Нижний левый угол
    Graph_Draw_Arc(Buffer_Frame, x + width - r, y + height - r, r, 0, 90, pxColor); // Нижний правый угол

    // Прямые линии между дугами
    Graph_Draw_Line(Buffer_Frame, x + r, y, x + width - r-spot_width, y, pxColor);           // Верхняя горизонталь
    Graph_Draw_Line(Buffer_Frame, x, y + r, x, y + height - r, pxColor);          // Левая вертикаль
    Graph_Draw_Line(Buffer_Frame, x + r+spot_width, y + height, x + width - r, y + height, pxColor); // Нижняя горизонталь
    Graph_Draw_Line(Buffer_Frame, x + width, y + r, x + width, y + height - r, pxColor); // Правая вертикаль

    // Стрелки
    Graph_Draw_Line(Buffer_Frame, x + width - r-spot_width, y, x + width - r - arr_size-spot_width, y - arr_size, pxColor); // Верхняя стрелка
    Graph_Draw_Line(Buffer_Frame, x + width - r-spot_width, y, x + width - r - arr_size-spot_width, y + arr_size, pxColor);

    Graph_Draw_Line(Buffer_Frame, x + r+spot_width, y + height, x + r + arr_size+spot_width, y + height - arr_size, pxColor); // Нижняя стрелка
    Graph_Draw_Line(Buffer_Frame, x + r+spot_width, y + height, x + r + arr_size+spot_width, y + height + arr_size, pxColor);
}
