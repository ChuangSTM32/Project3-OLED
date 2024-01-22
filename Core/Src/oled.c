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
#include <oled.h>
#include <stdio.h>

extern I2C_HandleTypeDef hi2c1;
/* Write command */
#define OLED_WRITECOMMAND(command)      OLED_I2C_Write(OLED_I2C_ADDR, 0x00, (command))
/* Write data */
#define OLED_WRITEDATA(data)            OLED_I2C_Write(OLED_I2C_ADDR, 0x40, (data))
/* Absolute value */
#define ABS(x)   ((x) > 0 ? (x) : -(x))

/* OLED data buffer */
static uint8_t OLED_Buffer[OLED_WIDTH * OLED_HEIGHT / 8];

/* Private OLED structure */
typedef struct {
	uint16_t CurrentX;
	uint16_t CurrentY;
	uint8_t Inverted;
	uint8_t Initialized;
} OLED_t;

/* Private variable */
static OLED_t OLED;

#define OLED_RIGHT_HORIZONTAL_SCROLL              0x26
#define OLED_LEFT_HORIZONTAL_SCROLL               0x27
#define OLED_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define OLED_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  0x2A
#define OLED_DEACTIVATE_SCROLL                    0x2E // Stop scroll
#define OLED_ACTIVATE_SCROLL                      0x2F // Start scroll
#define OLED_SET_VERTICAL_SCROLL_AREA             0xA3 // Set scroll range

#define OLED_NORMALDISPLAY       0xA6
#define OLED_INVERTDISPLAY       0xA7


void OLED_ScrollRight(uint8_t start_row, uint8_t end_row)
{
	OLED_WRITECOMMAND (OLED_RIGHT_HORIZONTAL_SCROLL);  // send 0x26
	OLED_WRITECOMMAND (0x00);  // send dummy
	OLED_WRITECOMMAND(start_row);  // start page address
	OLED_WRITECOMMAND(0X00);  // time interval 5 frames
	OLED_WRITECOMMAND(end_row);  // end page address
	OLED_WRITECOMMAND(0X00);
	OLED_WRITECOMMAND(0XFF);
	OLED_WRITECOMMAND (OLED_ACTIVATE_SCROLL); // start scroll
}


void OLED_ScrollLeft(uint8_t start_row, uint8_t end_row)
{
	OLED_WRITECOMMAND (OLED_LEFT_HORIZONTAL_SCROLL);  // send 0x26
	OLED_WRITECOMMAND (0x00);  // send dummy
	OLED_WRITECOMMAND(start_row);  // start page address
	OLED_WRITECOMMAND(0X00);  // time interval 5 frames
	OLED_WRITECOMMAND(end_row);  // end page address
	OLED_WRITECOMMAND(0X00);
	OLED_WRITECOMMAND(0XFF);
	OLED_WRITECOMMAND (OLED_ACTIVATE_SCROLL); // start scroll
}


void OLED_Scrolldiagright(uint8_t start_row, uint8_t end_row)
{
	OLED_WRITECOMMAND(OLED_SET_VERTICAL_SCROLL_AREA);  // sect the area
	OLED_WRITECOMMAND (0x00);   // write dummy
	OLED_WRITECOMMAND(OLED_HEIGHT);

	OLED_WRITECOMMAND(OLED_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
	OLED_WRITECOMMAND (0x00);
	OLED_WRITECOMMAND(start_row);
	OLED_WRITECOMMAND(0X00);
	OLED_WRITECOMMAND(end_row);
	OLED_WRITECOMMAND (0x01);
	OLED_WRITECOMMAND (OLED_ACTIVATE_SCROLL);
}


void OLED_Scrolldiagleft(uint8_t start_row, uint8_t end_row)
{
	OLED_WRITECOMMAND(OLED_SET_VERTICAL_SCROLL_AREA);  // sect the area
	OLED_WRITECOMMAND (0x00);   // write dummy
	OLED_WRITECOMMAND(OLED_HEIGHT);

	OLED_WRITECOMMAND(OLED_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
	OLED_WRITECOMMAND (0x00);
	OLED_WRITECOMMAND(start_row);
	OLED_WRITECOMMAND(0X00);
	OLED_WRITECOMMAND(end_row);
	OLED_WRITECOMMAND (0x01);
	OLED_WRITECOMMAND (OLED_ACTIVATE_SCROLL);
}


void OLED_Stopscroll(void)
{
	OLED_WRITECOMMAND(OLED_DEACTIVATE_SCROLL);
}


void OLED_InvertDisplay (int i)
{
  if (i) OLED_WRITECOMMAND (OLED_INVERTDISPLAY);

  else OLED_WRITECOMMAND (OLED_NORMALDISPLAY);

}


void OLED_DrawBitmap(int16_t x, int16_t y, const unsigned char* bitmap, int16_t w, int16_t h, uint16_t color)
{

    int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    for(int16_t j=0; j<h; j++, y++)
    {
        for(int16_t i=0; i<w; i++)
        {
            if(i & 7)
            {
               byte <<= 1;
            }
            else
            {
               byte = (*(const unsigned char *)(&bitmap[j * byteWidth + i / 8]));
            }
            if(byte & 0x80) OLED_DrawPixel(x+i, y, color);
        }
    }
}


uint8_t OLED_Init(void) {

	/* Init I2C */
	OLED_I2C_Init();

	/* Check if LCD connected to I2C */
	if (HAL_I2C_IsDeviceReady(&hi2c1, OLED_I2C_ADDR, 1, 20000) != HAL_OK) {
		/* Return false */
		return 0;
	}

	/* A little delay */
	uint32_t p = 2500;
	while(p>0)
		p--;

	/* Init LCD */
	OLED_WRITECOMMAND(0xAE); //display off
	OLED_WRITECOMMAND(0x20); //Set Memory Addressing Mode
	OLED_WRITECOMMAND(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	OLED_WRITECOMMAND(0xB0); //Set Page Start Address for Page Addressing Mode,0-7
	OLED_WRITECOMMAND(0xC8); //Set COM Output Scan Direction
	OLED_WRITECOMMAND(0x00); //---set low column address
	OLED_WRITECOMMAND(0x10); //---set high column address
	OLED_WRITECOMMAND(0x40); //--set start line address
	OLED_WRITECOMMAND(0x81); //--set contrast control register
	OLED_WRITECOMMAND(0xFF);
	OLED_WRITECOMMAND(0xA1); //--set segment re-map 0 to 127
	OLED_WRITECOMMAND(0xA6); //--set normal display
	OLED_WRITECOMMAND(0xA8); //--set multiplex ratio(1 to 64)
	OLED_WRITECOMMAND(0x3F); //
	OLED_WRITECOMMAND(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	OLED_WRITECOMMAND(0xD3); //-set display offset
	OLED_WRITECOMMAND(0x00); //-not offset
	OLED_WRITECOMMAND(0xD5); //--set display clock divide ratio/oscillator frequency
	OLED_WRITECOMMAND(0xF0); //--set divide ratio
	OLED_WRITECOMMAND(0xD9); //--set pre-charge period
	OLED_WRITECOMMAND(0x22); //
	OLED_WRITECOMMAND(0xDA); //--set com pins hardware configuration
	OLED_WRITECOMMAND(0x12);
	OLED_WRITECOMMAND(0xDB); //--set vcomh
	OLED_WRITECOMMAND(0x20); //0x20,0.77xVcc
	OLED_WRITECOMMAND(0x8D); //--set DC-DC enable
	OLED_WRITECOMMAND(0x14); //
	OLED_WRITECOMMAND(0xAF); //--turn on OLED panel


	OLED_WRITECOMMAND(OLED_DEACTIVATE_SCROLL);

	/* Clear screen */
	OLED_Fill(OLED_COLOR_BLACK);

	/* Update screen */
	OLED_UpdateScreen();

	/* Set default values */
	OLED.CurrentX = 0;
	OLED.CurrentY = 0;

	/* Initialized OK */
	OLED.Initialized = 1;

	/* Return OK */
	return 1;
}

void OLED_UpdateScreen(void) {
	uint8_t m;

	for (m = 0; m < 8; m++) {
		OLED_WRITECOMMAND(0xB0 + m);
		OLED_WRITECOMMAND(0x00);
		OLED_WRITECOMMAND(0x10);

		/* Write multi data */
		OLED_I2C_WriteMulti(OLED_I2C_ADDR, 0x40, &OLED_Buffer[OLED_WIDTH * m], OLED_WIDTH);
	}
}

void OLED_ToggleInvert(void) {
	uint16_t i;

	/* Toggle invert */
	OLED.Inverted = !OLED.Inverted;

	/* Do memory toggle */
	for (i = 0; i < sizeof(OLED_Buffer); i++) {
		OLED_Buffer[i] = ~OLED_Buffer[i];
	}
}

void OLED_Fill(OLED_COLOR_t color) {
	/* Set memory */
	memset(OLED_Buffer, (color == OLED_COLOR_BLACK) ? 0x00 : 0xFF, sizeof(OLED_Buffer));
}

void OLED_DrawPixel(uint16_t x, uint16_t y, OLED_COLOR_t color) {
	if (
		x >= OLED_WIDTH ||
		y >= OLED_HEIGHT
	) {
		/* Error */
		return;
	}

	/* Check if pixels are inverted */
	if (OLED.Inverted) {
		color = (OLED_COLOR_t)!color;
	}

	/* Set color */
	if (color == OLED_COLOR_WHITE) {
		OLED_Buffer[x + (y / 8) * OLED_WIDTH] |= 1 << (y % 8);
	} else {
		OLED_Buffer[x + (y / 8) * OLED_WIDTH] &= ~(1 << (y % 8));
	}
}

void OLED_SetCursor(uint16_t x, uint16_t y) {
	/* Set write pointers */
	OLED.CurrentX = x;
	OLED.CurrentY = y;
}

char OLED_ShowChar(char ch, FontDef_t* Font, OLED_COLOR_t color) {
	uint32_t i, b, j;

	/* Check available space in LCD */
	if (
			OLED_WIDTH <= (OLED.CurrentX + Font->FontWidth) ||
			OLED_HEIGHT <= (OLED.CurrentY + Font->FontHeight)
	) {
		/* Error */
		return 0;
	}

	/* Go through font */
	for (i = 0; i < Font->FontHeight; i++) {
		b = Font->data[(ch - 32) * Font->FontHeight + i];
		for (j = 0; j < Font->FontWidth; j++) {
			if ((b << j) & 0x8000) {
				OLED_DrawPixel(OLED.CurrentX + j, (OLED.CurrentY + i), (OLED_COLOR_t) color);
			} else {
				OLED_DrawPixel(OLED.CurrentX + j, (OLED.CurrentY + i), (OLED_COLOR_t)!color);
			}
		}
	}

	/* Increase pointer */
	OLED.CurrentX += Font->FontWidth;

	/* Return character written */
	return ch;
}

char OLED_ShowString(uint16_t x, uint16_t y, char* str, FontDef_t* Font) {
	OLED_SetCursor(x, y);

	/* Write characters */
	while (*str) {
		/* Write character by character */
		if (OLED_ShowChar(*str, Font, OLED_COLOR_WHITE) != *str) {
			/* Return error */
			return *str;
		}

		/* Increase string pointer */
		str++;
	}

	OLED_UpdateScreen();

	/* Everything OK, zero should be returned */
	return *str;
}

void OLED_ShowNum(uint16_t x, uint16_t y, uint32_t Number, FontDef_t* Font)
{
	char str[12];
	snprintf(str, sizeof(str), "%lu", Number);
	OLED_ShowString(x, y, str, Font);
}

void OLED_ShowSignedNum(uint16_t x, uint16_t y, int32_t Number, FontDef_t* Font)
{
	char str[12];
	snprintf(str, sizeof(str), "%+ld", Number);
	OLED_ShowString(x, y, str, Font);
}

void OLED_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, OLED_COLOR_t c) {
	int16_t dx, dy, sx, sy, err, e2, i, tmp;

	/* Check for overflow */
	if (x0 >= OLED_WIDTH) {
		x0 = OLED_WIDTH - 1;
	}
	if (x1 >= OLED_WIDTH) {
		x1 = OLED_WIDTH - 1;
	}
	if (y0 >= OLED_HEIGHT) {
		y0 = OLED_HEIGHT - 1;
	}
	if (y1 >= OLED_HEIGHT) {
		y1 = OLED_HEIGHT - 1;
	}

	dx = (x0 < x1) ? (x1 - x0) : (x0 - x1);
	dy = (y0 < y1) ? (y1 - y0) : (y0 - y1);
	sx = (x0 < x1) ? 1 : -1;
	sy = (y0 < y1) ? 1 : -1;
	err = ((dx > dy) ? dx : -dy) / 2;

	if (dx == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}

		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}

		/* Vertical line */
		for (i = y0; i <= y1; i++) {
			OLED_DrawPixel(x0, i, c);
		}

		/* Return from function */
		return;
	}

	if (dy == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}

		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}

		/* Horizontal line */
		for (i = x0; i <= x1; i++) {
			OLED_DrawPixel(i, y0, c);
		}

		/* Return from function */
		return;
	}

	while (1) {
		OLED_DrawPixel(x0, y0, c);
		if (x0 == x1 && y0 == y1) {
			break;
		}
		e2 = err;
		if (e2 > -dx) {
			err -= dy;
			x0 += sx;
		}
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		}
	}

	OLED_UpdateScreen();
}

void OLED_Clear (void)
{
	OLED_Fill (0);
	OLED_UpdateScreen();
}

void OLED_ON(void) {
	OLED_WRITECOMMAND(0x8D);
	OLED_WRITECOMMAND(0x14);
	OLED_WRITECOMMAND(0xAF);
}

void OLED_OFF(void) {
	OLED_WRITECOMMAND(0x8D);
	OLED_WRITECOMMAND(0x10);
	OLED_WRITECOMMAND(0xAE);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//  _____ ___   _____
// |_   _|__ \ / ____|
//   | |    ) | |
//   | |   / /| |
//  _| |_ / /_| |____
// |_____|____|\_____|
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void OLED_I2C_Init() {
	//MX_I2C1_Init();
	uint32_t p = 250000;
	while(p>0)
		p--;
	//HAL_I2C_DeInit(&hi2c1);
	//p = 250000;
	//while(p>0)
	//	p--;
	//MX_I2C1_Init();
}

void OLED_I2C_WriteMulti(uint8_t address, uint8_t reg, uint8_t* data, uint16_t count) {
	uint8_t dt[256];
	dt[0] = reg;
	uint8_t i;
	for(i = 0; i < count; i++)
		dt[i+1] = data[i];
	HAL_I2C_Master_Transmit(&hi2c1, address, dt, count+1, 10);
}


void OLED_I2C_Write(uint8_t address, uint8_t reg, uint8_t data) {
	uint8_t dt[2];
	dt[0] = reg;
	dt[1] = data;
	HAL_I2C_Master_Transmit(&hi2c1, address, dt, 2, 10);
}
