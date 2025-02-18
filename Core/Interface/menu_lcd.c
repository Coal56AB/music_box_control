#include "menu_stuff.h"

void Graph_Draw_LoopIcon(uint8_t *Buffer_Frame, uint8_t xPos, uint8_t yPos, uint8_t size, uint8_t pxColor);

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
		Graph_Draw_Line(oled_buf, play_icon_x_left, play_icon_y_up, play_icon_x_left, play_icon_y_down, 1);
		Graph_Draw_Line(oled_buf, play_icon_x_rigth, play_icon_y_up, play_icon_x_rigth, play_icon_y_down, 1);
	}
	// иконка "стоп"
	else
		{ 
		Graph_Draw_Triangle(oled_buf, play_icon_x_left, play_icon_y_up, play_icon_x_left, play_icon_y_down, play_icon_x_rigth+2, play_icon_y_mid, 1);
	}
	if(player->pressed_start)
		Graph_Invertsion_RectangleArea(oled_buf, play_icon_x_left-1, play_icon_y_up-1, play_icon_x_rigth+2-play_icon_x_left+2, play_icon_y_down-play_icon_y_up+2);
		
	
	// дорожка песни
	Graph_Draw_Line(oled_buf, 0, Graph_Height-1, player->currenttime*Graph_Width, Graph_Height-1, 1);
	
	
	// перемотка вперед
  Graph_Draw_Triangle(oled_buf, forward_icon_x_left, forward_icon_y_up, forward_icon_x_left, forward_icon_y_down, forward_icon_x_rigth+2, forward_icon_y_mid, 1);
  Graph_Draw_Line(oled_buf, forward_icon_x_rigth+3, forward_icon_y_up, forward_icon_x_rigth+3, forward_icon_y_down, 1);
	if(player->press_forward)
		Graph_Invertsion_RectangleArea(oled_buf, forward_icon_x_left-1, forward_icon_y_up-1, forward_icon_x_rigth+3-forward_icon_x_left+2, forward_icon_y_down-forward_icon_y_up+2);
		
		
	// перемотка назад
	Graph_Draw_Triangle(oled_buf, backward_icon_x_left, backward_icon_y_up, backward_icon_x_left, backward_icon_y_down, backward_icon_x_rigth-2, backward_icon_y_mid, 1);
	Graph_Draw_Line(oled_buf, forward_icon_x_rigth-3, backward_icon_y_up, forward_icon_x_rigth-3, backward_icon_y_down, 1);
	if(player->press_backward)
		Graph_Invertsion_RectangleArea(oled_buf, backward_icon_x_left-1, backward_icon_y_up-1, backward_icon_x_rigth+3-backward_icon_x_left+2, backward_icon_y_down-backward_icon_y_up+2);
	
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
	Graph_Draw_LoopIcon(oled_buf, loop_icon_x_start, loop_icon_y_start, loop_icon_size, 1);
	if(player->loop)
		Graph_Invertsion_RectangleArea(oled_buf, loop_icon_x_start-1, loop_icon_y_start-1, loop_icon_size+2, loop_icon_size+2);
}



/* Функция рисования значка "зациклить песню" */
void Graph_Draw_LoopIcon(uint8_t *Buffer_Frame, uint8_t x, uint8_t y, uint8_t size, uint8_t pxColor)
{
    uint8_t r = size / 4; // Радиус закруглений
    uint8_t w = size;     // Ширина значка

    // Верхняя дуга
    Graph_Draw_Arc(Buffer_Frame, x + w - r, y + r, r, 270, 360, pxColor);
    Graph_Draw_Arc(Buffer_Frame, x + r, y + r, r, 180, 270, pxColor);
    
    // Нижняя дуга
    Graph_Draw_Arc(Buffer_Frame, x + r, y + w - r, r, 90, 180, pxColor);
    Graph_Draw_Arc(Buffer_Frame, x + w - r, y + w - r, r, 0, 90, pxColor);

    // Прямые линии между дугами
    Graph_Draw_Line(Buffer_Frame, x + r, y, x + w - r, y, pxColor);       	// Верхняя горизонталь
    Graph_Draw_Line(Buffer_Frame, x, y + r, x, y + w - r, pxColor);       	// Левая вертикаль
    Graph_Draw_Line(Buffer_Frame, x + w - r, y + w, x + r, y + w, pxColor); // Нижняя горизонталь
    Graph_Draw_Line(Buffer_Frame, x + w, y + w - r, x + w, y + r, pxColor); // Правая вертикаль

    // Стрелки
    Graph_Draw_Line(Buffer_Frame, x + w - r, y, x + w - r - 3, y - 3, pxColor); // Верхняя стрелка
    Graph_Draw_Line(Buffer_Frame, x + w - r, y, x + w - r - 3, y + 3, pxColor);
    
    Graph_Draw_Line(Buffer_Frame, x + r, y + w, x + r + 3, y + w - 3, pxColor); // Нижняя стрелка
    Graph_Draw_Line(Buffer_Frame, x + r, y + w, x + r + 3, y + w + 3, pxColor);
}
