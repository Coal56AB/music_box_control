/*
 * PixelGraphics.h
 *
 */

#ifndef INC_PIXEL_GRAPHICS_H_
#define INC_PIXEL_GRAPHICS_H_

/* инклюды */
#include <main.h>
#include <string.h>
#include <stdio.h>
#include "oled.h"

#define Graph_Width 	128		//ширина дисплея в пикселях
#define Graph_Height 	32		//высота дисплея в пикселях

#define pxView_On	1		//закраска пикселя On
#define pxView_Off	0		//закраска пикселя Off

#define ChInvers	1		//инверсия символа шрифта On
#define ChUnInvers	0		//инверсия символа шрифта Off

/* прототипы функций */

void Graph_Clean_Buffer_Frame(uint8_t *Buffer_Frame, uint32_t Buffer_Frame_Size);
void Graph_Draw_Pixel(uint8_t *Buffer_Frame, uint8_t xPos, uint8_t yPos, uint8_t pxColor);
void Graph_Inversion_Pos_Buffer_Frame(uint8_t *Buffer_Frame, uint16_t xPos_Start, uint16_t yPos_Start, uint16_t width, uint16_t height);
void Graph_Invertsion_RectangleArea(uint8_t *Buffer_Frame, uint8_t xPos_Start, uint8_t yPos_Start, uint8_t rectangle_Width, uint8_t rectangle_Height);

void Graph_Draw_Char_1_Byte(uint8_t *Buffer_Frame, uint8_t xPos, uint8_t yPos, char Symbol, uint8_t Inversion);
void Graph_Draw_Char_2_Byte(uint8_t *Buffer_Frame, uint8_t xPos, uint8_t yPos, uint8_t Symbol, uint8_t inversion);
void Graph_Output_String(uint8_t *Buffer_Frame, uint8_t xPos, uint8_t yPos, char *String, uint8_t setChSpacing, uint8_t Inversion);

void Graph_Draw_Line(uint8_t *Buffer_Frame, uint8_t xPos_Start, uint8_t yPos_Start, uint8_t xPos_End, uint8_t yPos_End, uint8_t pxColor);
void Graph_Draw_Rectangle(uint8_t *Buffer_Frame, uint8_t xPos_Start, uint8_t yPos_Start, uint8_t rectangle_Width, uint8_t rectangle_Height, uint8_t pxColor);
void Graph_Draw_Rectangle_Filled(uint8_t *Buffer_Frame, uint8_t xPos_Start, uint8_t yPos_Start, uint8_t rectangle_Width, uint8_t rectangle_Height, uint8_t pxColor);
void Graph_Draw_Circle(uint8_t *Buffer_Frame, uint8_t xPos, uint8_t yPos, uint8_t circle_Radius, uint8_t pxColor);
void Graph_Draw_Circle_Filled(uint8_t *Buffer_Frame, int8_t xPos, int8_t yPos, int8_t circle_Radius, uint8_t pxColor);
void Graph_Draw_Triangle(uint8_t *Buffer_Frame, uint8_t xPos1, uint8_t yPos1, uint8_t xPos2, uint8_t yPos2, uint8_t xPos3, uint8_t yPos3, uint8_t pxColor);
void Graph_Draw_Arc(uint8_t *Buffer_Frame, uint8_t xPos, uint8_t yPos, uint8_t radius, uint16_t startAngle, uint16_t endAngle, uint8_t pxColor);
#endif /* INC_PIXEL_GRAPHICS_H_ */
