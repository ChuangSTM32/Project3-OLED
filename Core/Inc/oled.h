/**
 * original author:  Tilen Majerle<tilen@majerle.eu>
 * modification for STM32f10x: Alexander Lutsai<s.lyra@ya.ru>

   ----------------------------------------------------------------------
   	Copyright (C) Alexander Lutsai, 2016
    Copyright (C) Tilen Majerle, 2015

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
   ----------------------------------------------------------------------
 */
#ifndef OLED_H
#define OLED_H 100

/* C++ detection */
#ifdef __cplusplus
extern C {
#endif

/**
 * This OLED LCD uses I2C for communication
 *
 * Library features functions for drawing lines, rectangles and circles.
 *
 * It also allows you to draw texts and characters using appropriate functions provided in library.
 *
 * Default pinout
 *
**/

#include "stm32f4xx_hal.h"

#include <fonts.h>

#include "stdlib.h"
#include "string.h"


// I2C address
#ifndef OLED_I2C_ADDR
#define OLED_I2C_ADDR         0x78
//#define OLED_I2C_ADDR       0x7A
#endif

/* OLED settings */
/* OLED width in pixels */
#ifndef OLED_WIDTH
#define OLED_WIDTH            128
#endif
/* OLED LCD height in pixels */
#ifndef OLED_HEIGHT
#define OLED_HEIGHT           64
#endif

/**
 * @brief  OLED color enumeration
 */
typedef enum {
	OLED_COLOR_BLACK = 0x00, /*!< Black color, no pixel */
	OLED_COLOR_WHITE = 0x01  /*!< Pixel is set. Color depends on LCD */
} OLED_COLOR_t;



/**
 * @brief  Initializes OLED LCD
 * @param  None
 * @retval Initialization status:
 *           - 0: LCD was not detected on I2C port
 *           - > 0: LCD initialized OK and ready to use
 */
uint8_t OLED_Init(void);

/**
 * @brief  Updates buffer from internal RAM to LCD
 * @note   This function must be called each time you do some changes to LCD, to update buffer from RAM to LCD
 * @param  None
 * @retval None
 */
void OLED_UpdateScreen(void);

/**
 * @brief  Toggles pixels invertion inside internal RAM
 * @note   @ref OLED_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  None
 * @retval None
 */
void OLED_ToggleInvert(void);

/**
 * @brief  Fills entire LCD with desired color
 * @note   @ref OLED_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  Color: Color to be used for screen fill. This parameter can be a value of @ref OLED_COLOR_t enumeration
 * @retval None
 */
void OLED_Fill(OLED_COLOR_t Color);

/**
 * @brief  Draws pixel at desired location
 * @note   @ref OLED_UpdateScreen() must called after that in order to see updated LCD screen
 * @param  x: X location. This parameter can be a value between 0 and OLED_WIDTH - 1
 * @param  y: Y location. This parameter can be a value between 0 and OLED_HEIGHT - 1
 * @param  color: Color to be used for screen fill. This parameter can be a value of @ref OLED_COLOR_t enumeration
 * @retval None
 */
void OLED_DrawPixel(uint16_t x, uint16_t y, OLED_COLOR_t color);

/**
 * @brief  Sets cursor pointer to desired location for strings
 * @param  x: X location. This parameter can be a value between 0 and OLED_WIDTH - 1
 * @param  y: Y location. This parameter can be a value between 0 and OLED_HEIGHT - 1
 * @retval None
 */
void OLED_SetCursor(uint16_t x, uint16_t y);

/**
 * @brief  Puts character to internal RAM
 * @note   @ref OLED_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  ch: Character to be written
 * @param  color: Color used for drawing. This parameter can be a value of @ref OLED_COLOR_t enumeration
 * @retval Character written
 */
char OLED_ShowChar(char ch, FontDef_t* Font, OLED_COLOR_t color);

/**
 * @brief  Puts string to internal RAM
 * @param  Column: start column. Valid input is 0 to 11
 * @param  Line: start row. Valid input is 0 to 2
 * @param  *str: String to be written
 * @retval Zero on success or character value when function failed
 */
char OLED_ShowString(uint16_t Column, uint16_t Line, char* str, FontDef_t* Font);

/**
 * @brief  Draws line on LCD
 * @note   @ref OLED_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x0: Line X start point. Valid input is 0 to OLED_WIDTH - 1
 * @param  y0: Line Y start point. Valid input is 0 to OLED_HEIGHT - 1
 * @param  x1: Line X end point. Valid input is 0 to OLED_WIDTH - 1
 * @param  y1: Line Y end point. Valid input is 0 to OLED_HEIGHT - 1
 * @param  c: Color to be used. This parameter can be a value of @ref OLED_COLOR_t enumeration
 * @retval None
 */
void OLED_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, OLED_COLOR_t c);

/**
 * @brief  Puts number to internal RAM
 * @param  x: start x. Valid input is 0 to 127
 * @param  y: start y. Valid input is 0 to 63
 * @param  Number: number to be written.
 * @retval None
 */
void OLED_ShowNum(uint16_t x, uint16_t y, uint32_t Number, FontDef_t* Font);

/**
 * @brief  Puts number to internal RAM
 * @param  x: start x. Valid input is 0 to 127
 * @param  y: start y. Valid input is 0 to 63
 * @param  Number: number to be written.
 * @retval None
 */
void OLED_ShowSignedNum(uint16_t x, uint16_t y, int32_t Number, FontDef_t* Font);

#ifndef OLED_I2C_TIMEOUT
#define OLED_I2C_TIMEOUT					20000
#endif

/**
 * @brief  Initializes OLED LCD
 * @param  None
 * @retval Initialization status:
 *           - 0: LCD was not detected on I2C port
 *           - > 0: LCD initialized OK and ready to use
 */
void OLED_I2C_Init();

/**
 * @brief  Writes single byte to slave
 * @param  *I2Cx: I2C used
 * @param  address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used
 * @param  reg: register to write to
 * @param  data: data to be written
 * @retval None
 */
void OLED_I2C_Write(uint8_t address, uint8_t reg, uint8_t data);

/**
 * @brief  Writes multi bytes to slave
 * @param  *I2Cx: I2C used
 * @param  address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used
 * @param  reg: register to write to
 * @param  *data: pointer to data array to write it to slave
 * @param  count: how many bytes will be written
 * @retval None
 */
void OLED_I2C_WriteMulti(uint8_t address, uint8_t reg, uint8_t *data, uint16_t count);

/**
 * @brief  Draws the Bitmap
 * @param  X:  X location to start the Drawing
 * @param  Y:  Y location to start the Drawing
 * @param  *bitmap : Pointer to the bitmap
 * @param  W : width of the image
 * @param  H : Height of the image
 * @param  color : 1-> white/blue, 0-> black
 */
void OLED_DrawBitmap(int16_t x, int16_t y, const unsigned char* bitmap, int16_t w, int16_t h, uint16_t color);

// scroll the screen for fixed rows

void OLED_ScrollRight(uint8_t start_row, uint8_t end_row);


void OLED_ScrollLeft(uint8_t start_row, uint8_t end_row);


void OLED_Scrolldiagright(uint8_t start_row, uint8_t end_row);


void OLED_Scrolldiagleft(uint8_t start_row, uint8_t end_row);


void OLED_Stopscroll(void);


// inverts the display i = 1->inverted, i = 0->normal

void OLED_InvertDisplay (int i);






// clear the display

void OLED_Clear (void);


/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
