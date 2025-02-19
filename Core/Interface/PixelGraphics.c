/*
 * PixelGraphics.c
 *
 * Библиотека для заполнения буфера дисплея
 *
 * Для разработки библиотеки иcпользовались материалы:
 * https://www.youtube.com/watch?v=ajEqZN5s5xc
 * https://narodstream.ru/stm-urok-37-displej-tft-240x320-8bit-chast-1/
 * https://hubstub.ru/display/126-vyvod-simvolov-i-strok-na-tft-displey-na-primere-ili9341.html
 */
#include "PixelGraphics.h"
#include "font_tahoma_8_prop.h"

/* переменные */

//uint8_t Buffer_Frame[128 * 32] 	= {0,0};	//буфер кадра
uint8_t chSpacing			= 0;		//межсимвольный интервал в px

/* функция очистки буфера кадра */
void Graph_Clean_Buffer_Frame(uint8_t *Buffer_Frame, uint32_t Buffer_Frame_Size)
{
	memset(Buffer_Frame, 0x00, Buffer_Frame_Size);
}

/* функция прорисовки пикселя */
void Graph_Draw_Pixel(uint8_t *Buffer_Frame, uint8_t xPos, uint8_t yPos, uint8_t pxColor)
{
	if ((xPos >= Graph_Width)||(xPos < 0)||(yPos >= Graph_Height)||(yPos < 0))
	{
		//если значения по x и y больше пределов то выходим из функции
		return;
	}
	else
	{
		uint16_t arrayPos = xPos + ((yPos/8)*Graph_Width);

		//заполняем буфер кадра
		if (pxColor)
		{
			Buffer_Frame[arrayPos] |= 1 << (yPos % 8);
		}
		else
		{
			Buffer_Frame[arrayPos] &= 0xFF ^ 1 << (yPos % 8);
		}
	}
}

/* функция инверсии любой точки в буфере кадра */
void Graph_Inversion_Pos_Buffer_Frame(uint8_t *Buffer_Frame, uint16_t xPos_Start, uint16_t xPos_End)
{
	for (; xPos_Start < xPos_End; xPos_Start++)
	{
		Buffer_Frame[xPos_Start] = ~(Buffer_Frame[xPos_Start]);
	}
}

/* работа со шрифтами */

/* функция прорисовки однобайтового символа шрифта */
void Graph_Draw_Char_1_Byte(uint8_t *Buffer_Frame, uint8_t xPos, uint8_t yPos, char Symbol, uint8_t Inversion)
{
	/*
	 * выгрузка кода шрифта в "h" файл из программы matrixFont
	 * https://habr.com/ru/articles/575332/
	 * параметры выгрузки:
	 * сначала строки, справа на лево, сверху вниз, пропорциональный, HEX, нули, 8(uint8_t, unsigned char), С99
	 * ниже циклы для однобайтового символа шрифта, т.е. ширина символа не более 8 пикселей
	 */

	/* если ASCII номер символа в диапазоне 32...255, то рисуем символы шрифта */
	if ((Symbol >= 32) && (Symbol <= 255))
	{
		uint8_t pxChView;
		uint8_t pxBkView;

		//включение/выключении инверсии символа
		if (!(Inversion & ChInvers))
		{
			pxChView = pxView_On;
			pxBkView = pxView_Off;
		}
		else
		{
			pxChView = pxView_Off;
			pxBkView = pxView_On;
		}

		//извлекаем ширину символа + 1px для минимального межсимвольного интервала
		uint8_t chWidth = font_tahoma_8[(Symbol - 0x20) * (1 + FONT_TAHOMA_8_CHAR_HEIGHT)] + 1;
		//запоминаем ширину данного символа
		chSpacing = chWidth;

		//рисуем заданный символ шрифта
		for (uint8_t y = 1; y <= FONT_TAHOMA_8_CHAR_HEIGHT; y++) //высота холста шрифта в пикселях
		{
			for (uint8_t x = 0; x < chWidth; x++) //ширина холста символа шрифта
			{
				/*
				 * извлекаем из массива шрифта байт строки пикселей символа
				 * и с помощью операции побитового И определяем где 1 и где 0
				 * 0x20 на 32
				 */

				if (font_tahoma_8[((Symbol - 0x20) * (1 + FONT_TAHOMA_8_CHAR_HEIGHT)) + y] >> (7 - x) & 0x01)
				{
					Graph_Draw_Pixel(Buffer_Frame, xPos + x, yPos + y, pxChView);
				}
				else
				{
					Graph_Draw_Pixel(Buffer_Frame, xPos + x, yPos + y, pxBkView);
				}
			}

			/*
			 * заполняем фоном справа от символа если ширина символа меньше ширины холста шрифта
			 * с учетом заданного минимального межсимвольного интервала в 1px
			 */
			if (chWidth < FONT_TAHOMA_8_CHAR_WIDTH)
			{
				for (uint8_t n = chWidth; n < (FONT_TAHOMA_8_CHAR_WIDTH + 1); n++)
				{
					if (!(Inversion & ChInvers))
					{
						Graph_Draw_Pixel(Buffer_Frame, xPos + n, yPos + y, pxBkView);
					}
					else
					{
						Graph_Draw_Pixel(Buffer_Frame, xPos + n, yPos + y, pxChView);
					}
				}
			}
		}
	}
}

/* функция прорисовки двухбайтового символа шрифта */
void Graph_Draw_Char_2_Byte(uint8_t *Buffer_Frame, uint8_t xPos, uint8_t yPos, uint8_t Symbol, uint8_t Inversion)
{
//	/*
//	 * выгрузка кода шрифта в "h" файл из программы matrixFont
//	 * https://habr.com/ru/articles/575332/
//	 * параметры выгрузки:
//	 * сначала строки, справа на лево, сверху вниз, моноширный, HEX, нули, 8(uint8_t, unsigned char), С99
//	 * ниже циклы для двухбайтового символа шрифта, т.е. ширина символа более 8 пикселей
//	 */

//	/* если ASCII номер символа в диапазоне 48...57, то рисуем символы цифр шрифта */

//	if ((Symbol >= 48) && (Symbol <= 57))
//	{
//		Symbol = (Symbol - 48) * 15;

//		uint8_t pxChView;
//		uint8_t pxBkView;

//		//включение/выключении инверсии символа
//		if (!(Inversion & ChInvers))
//		{
//			pxChView = pxView_On;
//			pxBkView = pxView_Off;
//		}
//		else
//		{
//			pxChView = pxView_Off;
//			pxBkView = pxView_On;
//		}

//		//рисуем заданный символ шрифта
//		for (uint8_t y = 0; y < FONT_TERMINUS_10X15__CHAR_HEIGHT; y++) //высота холста шрифта в пикселях
//		{
//			for (uint8_t x = 0; x < 8; x++) //8 - один байт
//			{
//				/*
//				 * извлекаем из массива шрифта правый байт строки пикселей символа
//				 * и с помощью операции побитового И определяем где 1 а где 0
//				 */
//				if (font_terminus_10x15_[(Symbol + y) * 2 + 1] >> (7 - x) & 0x01)
//				{
//					/*
//					 * рисуем только нужную нам часть из правого байта
//					 * пикселя символа, -6 смещаем картинку на заданную позицию
//					 */
//					if (x > 5)
//					{
//						Graph_Draw_Pixel(Buffer_Frame, xPos + x - 6, yPos + y, pxChView);
//					}
//				}
//				else
//				{
//					/*
//					 * рисуем только нужную нам часть из правого байта
//					 * пикселя фона символа, -6 смещаем картинку на заданную позицию
//					 */
//					if (x > 5)
//					{
//						Graph_Draw_Pixel(Buffer_Frame, xPos + x - 6, yPos + y, pxBkView);
//					}
//				}

//				/*
//				 * извлекаем из массива шрифта левый байт строки пикселей символа
//				 * и с помощью операции побитового И определяем где 1 а где 0
//				 */
//				if (font_terminus_10x15_[(Symbol + y) * 2] >> (7 - x) & 0x01)
//				{
//					/*
//					 * смещаем картинку на 8 пикселей вправо
//					 * т.е. "склеиваем" картинку символа и рисуем пиксель символа,
//					 * -6 смещаем картинку на заданную позицию
//					 */
//					Graph_Draw_Pixel(Buffer_Frame, xPos + x + 8  - 6, yPos + y, pxChView);
//				}
//				else
//				{
//					/*
//					 * смещаем картинку на 8 пикселей вправо
//					 * т.е. "склеиваем" картинку символа и рисуем пиксель фона символа,
//					 * -6 смещаем картинку на заданную позицию
//					 */
//					Graph_Draw_Pixel(Buffer_Frame, xPos + x + 8  - 6, yPos + y, pxBkView);
//				}
//			}
//		}
//	}
}

/* функция вывода строки на дисплей */
void Graph_Output_String(uint8_t *Buffer_Frame, uint8_t xPos, uint8_t yPos, char *String, uint8_t setChSpacing, uint8_t Inversion)
{
	/*
	 * вывод строки шрифтом font_tahoma_8
	 * параметры функции:
	 * xPos			- позиция по X
	 * yPos			- позиция по Y
	 * *String		- строка для вывода
	 * setChSpacing	- межсимвольный интервал в px
	 * Inversion	- включение/выключени инверсии
	 */

	//uint8_t xPos_Start = xPos; //px начала вывода строки по X

	//посимвольный вывод строки
	while(*String)
	{
		//проверяем не вылезем ли мы за пределы экрана при отрисовке следующего символа,
		// если да, то переходим на следующую строчку
		/*if((xPos + 8) > Graph_Width)
		{
			xPos = xPos_Start;
			yPos = yPos + 10;
		}*/

		//вывод текущего символа строки
		Graph_Draw_Char_1_Byte(Buffer_Frame, xPos, yPos, *String, Inversion);

		//изменяем координату для отрисовки следующего символа
		xPos += chSpacing - 1 + setChSpacing;

		//инкремент указателя на следующий символ в строке
		String++;
	}
}

/* геометрические примитивы */

/* функция рисования линии */
void Graph_Draw_Line(uint8_t *Buffer_Frame, uint8_t xPos_Start, uint8_t yPos_Start, uint8_t xPos_End, uint8_t yPos_End, uint8_t pxColor)
{
	int dx = (xPos_End >= xPos_Start) ? xPos_End - xPos_Start : xPos_Start - xPos_End;
	int dy = (yPos_End >= yPos_Start) ? yPos_End - yPos_Start : yPos_Start - yPos_End;
	int sx = (xPos_Start < xPos_End) ? 1 : -1;
	int sy = (yPos_Start < yPos_End) ? 1 : -1;
	int err = dx - dy;

	for (;;)
	{
		Graph_Draw_Pixel(Buffer_Frame, xPos_Start, yPos_Start, pxColor);
		if (xPos_Start == xPos_End && yPos_Start == yPos_End)
			break;
		int e2 = err + err;
		if (e2 > -dy)
		{
			err -= dy;
			xPos_Start += sx;
		}
		if (e2 < dx)
		{
			err += dx;
			yPos_Start += sy;
		}
	}
}

/* функция рисования пустотелого прямоугольника */
void Graph_Draw_Rectangle(uint8_t *Buffer_Frame, uint8_t xPos_Start, uint8_t yPos_Start, uint8_t rectangle_Width, uint8_t rectangle_Height, uint8_t pxColor)
{
	/* рисуем стороны прямоугольника */
	//левая сторона прямоугольника
	Graph_Draw_Line(Buffer_Frame, xPos_Start, yPos_Start, xPos_Start, yPos_Start + rectangle_Height, pxColor);
	//верх прямоугольника
	Graph_Draw_Line(Buffer_Frame, xPos_Start, yPos_Start, xPos_Start + rectangle_Width, yPos_Start, pxColor);
	//правая сторона прямоугольника
	Graph_Draw_Line(Buffer_Frame, xPos_Start + rectangle_Width, yPos_Start, xPos_Start + rectangle_Width, yPos_Start + rectangle_Height, pxColor);
	//низ прямоугольника
	Graph_Draw_Line(Buffer_Frame, xPos_Start, yPos_Start + rectangle_Height, xPos_Start + rectangle_Width, yPos_Start + rectangle_Height, pxColor);
}

/* функция рисования закрашенного прямоугольника */
void Graph_Draw_Rectangle_Filled(uint8_t *Buffer_Frame, uint8_t xPos_Start, uint8_t yPos_Start, uint8_t rectangle_Width, uint8_t rectangle_Height, uint8_t pxColor)
{
	for (uint8_t i = 0; i <= rectangle_Height; i++)
	{
		Graph_Draw_Line(Buffer_Frame, xPos_Start, yPos_Start + i, xPos_Start + rectangle_Width, yPos_Start + i, pxColor);
	}
}

/* функция рисования пустотелой окружности */
void Graph_Draw_Circle(uint8_t *Buffer_Frame, uint8_t xPos, uint8_t yPos, uint8_t circle_Radius, uint8_t pxColor)
{
	int f = 1 - (int)circle_Radius;
	int ddF_x = 1;
	int ddF_y = -2 * (int)circle_Radius;
	int x_0 = 0;

	Graph_Draw_Pixel(Buffer_Frame, xPos, yPos + circle_Radius, pxColor);
	Graph_Draw_Pixel(Buffer_Frame, xPos, yPos - circle_Radius, pxColor);
	Graph_Draw_Pixel(Buffer_Frame, xPos + circle_Radius, yPos, pxColor);
	Graph_Draw_Pixel(Buffer_Frame, xPos - circle_Radius, yPos, pxColor);

	int y_0 = circle_Radius;
	while (x_0 < y_0)
	{
		if (f >= 0)
		{
			y_0--;
			ddF_y += 2;
			f += ddF_y;
		}

		x_0++;
		ddF_x += 2;
		f += ddF_x;

		Graph_Draw_Pixel(Buffer_Frame, xPos + x_0, yPos + y_0, pxColor);
		Graph_Draw_Pixel(Buffer_Frame, xPos - x_0, yPos + y_0, pxColor);
		Graph_Draw_Pixel(Buffer_Frame, xPos + x_0, yPos - y_0, pxColor);
		Graph_Draw_Pixel(Buffer_Frame, xPos - x_0, yPos - y_0, pxColor);
		Graph_Draw_Pixel(Buffer_Frame, xPos + y_0, yPos + x_0, pxColor);
		Graph_Draw_Pixel(Buffer_Frame, xPos - y_0, yPos + x_0, pxColor);
		Graph_Draw_Pixel(Buffer_Frame, xPos + y_0, yPos - x_0, pxColor);
		Graph_Draw_Pixel(Buffer_Frame, xPos - y_0, yPos - x_0, pxColor);
	}
}

/* функция рисования закрашенной окружности */
void Graph_Draw_Circle_Filled(uint8_t *Buffer_Frame, int8_t xPos, int8_t yPos, int8_t circle_Radius, uint8_t pxColor)
{
	int16_t f = 1 - circle_Radius;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * circle_Radius;
	int16_t x_0 = 0;
	int16_t y_0 = circle_Radius;

	Graph_Draw_Pixel(Buffer_Frame, xPos, yPos + circle_Radius, pxColor);
	Graph_Draw_Pixel(Buffer_Frame, xPos, yPos - circle_Radius, pxColor);
	Graph_Draw_Pixel(Buffer_Frame, xPos + circle_Radius, yPos, pxColor);
	Graph_Draw_Pixel(Buffer_Frame, xPos - circle_Radius, yPos, pxColor);
	Graph_Draw_Line(Buffer_Frame, xPos - circle_Radius, yPos, xPos + circle_Radius, yPos, pxColor);

	while (x_0 < y_0)
	{
		if (f >= 0)
		{
			y_0--;
			ddF_y += 2;
			f += ddF_y;
		}

		x_0++;
		ddF_x += 2;
		f += ddF_x;

		Graph_Draw_Line(Buffer_Frame, xPos - x_0, yPos + y_0, xPos + x_0, yPos + y_0, pxColor);
		Graph_Draw_Line(Buffer_Frame, xPos + x_0, yPos - y_0, xPos - x_0, yPos - y_0, pxColor);
		Graph_Draw_Line(Buffer_Frame, xPos + y_0, yPos + x_0, xPos - y_0, yPos + x_0, pxColor);
		Graph_Draw_Line(Buffer_Frame, xPos + y_0, yPos - x_0, xPos - y_0, yPos - x_0, pxColor);
	}
}

/* функция рисования треугольника */
void Graph_Draw_Triangle(uint8_t *Buffer_Frame, 
                         uint8_t xPos1, uint8_t yPos1, 
                         uint8_t xPos2, uint8_t yPos2, 
                         uint8_t xPos3, uint8_t yPos3, 
                         uint8_t pxColor)
{
    Graph_Draw_Line(Buffer_Frame, xPos1, yPos1, xPos2, yPos2, pxColor);
    Graph_Draw_Line(Buffer_Frame, xPos2, yPos2, xPos3, yPos3, pxColor);
    Graph_Draw_Line(Buffer_Frame, xPos3, yPos3, xPos1, yPos1, pxColor);
}

/* Функция рисования дуги (четверти окружности) */
void Graph_Draw_Arc(uint8_t *Buffer_Frame, uint8_t xPos, uint8_t yPos, uint8_t radius, uint16_t startAngle, uint16_t endAngle, uint8_t pxColor)
{
	int xPos_tmp = 0;
	int yPos_tmp = 0;
	for (int angle = startAngle; angle <= endAngle; angle++)
	{
			xPos_tmp = xPos + (radius * cosf(angle * 3.14159 / 180));
			yPos_tmp = yPos + (radius * sinf(angle * 3.14159 / 180));
			Graph_Draw_Pixel(Buffer_Frame, xPos_tmp, yPos_tmp, pxColor);
	}
}

/* Функция инвертирования прямоугольной области */
void Graph_Invertsion_RectangleArea(uint8_t *Buffer_Frame, uint8_t xPos_Start, uint8_t yPos_Start, uint8_t rectangle_Width, uint8_t rectangle_Height)
{
	// Пройдем по всем строкам прямоугольника
	for (uint16_t y = yPos_Start; y < yPos_Start + rectangle_Height; y++)
	{
			// Вычислим начальный и конечный индекс для данной строки
			uint16_t xStartPos = y * Graph_Width/8 + xPos_Start;
			uint16_t xEndPos = y * Graph_Width/8 + xPos_Start + rectangle_Width;

			// Инвертируем пиксели в данной строке
			Graph_Inversion_Pos_Buffer_Frame(Buffer_Frame, xStartPos, xEndPos);
	}
	
}
