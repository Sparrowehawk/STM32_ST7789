/*
 * gmt020-02.c
 *
 *  Created on: May 24, 2025
 *      Author: Hamid
 */


#include "fonts.h"
#include "gmt020-02.h"
#include "main.h"

void ST7789_init()
{

	uint8_t data[] = {0x55, 0x00};
	uint16_t data_16bit[] = {ST7789_XSTART, 0x00EF, ST7789_YSTART, 0x013F};
	uint8_t addr[] = {ST7789_SWRST, ST7789_SLPOUT, ST7789_COLMOD, ST7789_MADCTL, ST7789_CASET, ST7789_RASET, ST7789_INVON, ST7789_NORON, ST7789_DISPON};


	ST7789_write_command();

	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, 0);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, 1);
	HAL_Delay(1000);

	ST7789_write_command();
	HAL_SPI_Transmit(&hspi1, &addr[0], 1, 500);
	HAL_SPI_Transmit(&hspi1, &addr[1], 1, 500);

	HAL_SPI_Transmit(&hspi1, &addr[2], 1, 500);
	ST7789_write_data();
	HAL_SPI_Transmit(&hspi1, &data[0], 1, 500);

	ST7789_write_command();
	HAL_SPI_Transmit(&hspi1, &addr[3], 1, 500);
	ST7789_write_data();
	HAL_SPI_Transmit(&hspi1, &data[1], 1, 500);


	// Writing 2 x 16 bit data streams as 4 x 8 bits
	// Sending 0x0000 as 0x00 0x00
	// Sending 239, 0x00EF as 0x00 0xEF
	ST7789_write_command();
	HAL_SPI_Transmit(&hspi1, &addr[4], 1, 500);
	ST7789_write_data();
	ST7789_write_16bit(&data_16bit[0]);
	ST7789_write_16bit(&data_16bit[1]);

	// Writing 2 x 16 bit data streams as 4 x 8 bits
	// Sending 0x0000 as 0x00 0x00
	// Sending 319, 0x013F as 0x01 0x3F
	ST7789_write_command();
	HAL_SPI_Transmit(&hspi1, &addr[5], 1, 500);
	ST7789_write_data();
	ST7789_write_16bit(&data_16bit[2]);
	ST7789_write_16bit(&data_16bit[3]);


	ST7789_write_command();
	HAL_SPI_Transmit(&hspi1, &addr[6], 1, 500);
	HAL_SPI_Transmit(&hspi1, &addr[7], 1, 500);

	HAL_SPI_Transmit(&hspi1, &addr[8], 1, 500);
	ST7789_end_write();
}

void ST7789_write_data()
{
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, 0);
	HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, 1);
}

void ST7789_write_command()
{
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, 0);
	HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, 0);
}

void ST7789_end_write()
{
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, 1);
}

void ST7789_write_16bit(uint16_t *data)
{
	uint8_t bufferA = (*data) >> 8;;
	uint8_t bufferB = (*data) & 0xFF;

	HAL_SPI_Transmit(&hspi1, &bufferA, 1, 500);
	HAL_SPI_Transmit(&hspi1, &bufferB, 1, 500);
}

void ST7789_set_addr(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1)
{
	uint16_t data;
	uint8_t addr[] = {ST7789_CASET, ST7789_RASET, ST7789_RAMWR};

	ST7789_write_command();
	HAL_SPI_Transmit(&hspi1, &addr[0], 1, 500);
	ST7789_write_data();
	data = x0 + ST7789_XSTART;
	ST7789_write_16bit(&data);
	data = x1 + ST7789_XSTART;
	ST7789_write_16bit(&data);

	ST7789_write_command();
	HAL_SPI_Transmit(&hspi1, &addr[1], 1, 500);
	ST7789_write_data();
	data = y0 + ST7789_XSTART;
	ST7789_write_16bit(&data);

	data = y1 + ST7789_XSTART;
	ST7789_write_16bit(&data);


	ST7789_write_command();
	HAL_SPI_Transmit(&hspi1, &addr[2], 1, 500);
	ST7789_end_write();
}

void ST7789_draw_pixel(uint16_t x, uint16_t y, uint16_t colour)
{
	uint16_t data;
	ST7789_set_addr(x, x+1, y, y+1);
	ST7789_write_data();

	data = colour;
	ST7789_write_16bit(&data);
	ST7789_end_write();
}

void ST7789_clear_screen(uint16_t colour)
{
	uint32_t i;
	ST7789_set_addr(0, LCD_WIDTH-1, 0, LCD_HEIGHT-1);

	ST7789_write_data();
	for (i = LCD_SIZE; i > 0; i--){
		ST7789_write_16bit(&colour);
	}
	ST7789_end_write();
}

void ST7789_draw_char(uint16_t x, uint16_t y, uint16_t colour, char letter)
{
    if (letter < 32 || letter > 127) return;

    int col, row;

    for (col = 0; col < 5; col++) {
        uint8_t line = ascii[letter - 32][col];

        for (row = 0; row < 8; row++) {
            if (line & (1 << row)) {
                ST7789_draw_pixel(x + col, y + row, colour);
            }
        }
    }
}

void ST7789_draw_string(uint16_t x, uint16_t y, uint16_t colour, char* string)
{
    int i;
    uint16_t x_pos = x;
    uint16_t y_pos = y;

    for (i = 0; string[i] != '\0'; i++) {
        if(x_pos + 6 > LCD_WIDTH){
            y_pos += 8;
            x_pos = 0;

            if(y_pos + 8 > LCD_HEIGHT){
                y_pos = 0;
            }

        }

        ST7789_draw_char(x_pos, y_pos, colour, string[i]);
        x_pos += 6;
    }
}


