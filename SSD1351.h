#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "stdio.h"
#include "string.h"

#define SET_COLUMN_ADDRESS 0x15
#define SET_ROW_ADDRESS 0x75
#define WRITE_RAM_COMMAND 0x5C
#define READ_RAM_COMMAND 0x5D
#define SET_COMMAND_LOCK 0xFD 
#define UNLOCK_DRIVER 0x12
#define DISPLAY_ON 0xAF
#define SET_DISPLAY_OFFSET 0xA2
#define SET_DISPLAY_OFF 0xA4 //pixels are turned off/black, regardless of ram content
#define SET_DISPLAY_ON 0xA5 //all pixels are turned to white, regardless of ram
#define SET_INVERSE_DISPLAY 0xA7 //inverse everything
#define RESET_DISPLAY_NORMAL 0xA6 //returns to normal operation if the last 3 commands were used
#define SET_MULTIPLEX_RATIO 0xCA

#define SET_SLEEP_ON 0xAE
#define SET_SLEEP_OFF 0xAF
#define DISPLAY_ENHANCEMENT 0xB2

#define COLUMN_START_ADDRESS 0x00
#define COLUMN_END_ADDRESS 0x7F
#define ROW_START_ADDRESS 0x00
#define ROW_END_ADDRESS 0x5F
#define DISPLAY_START_LINE 0x00
#define REMAP 0xA0 // 0x00 = horizontal; 0x01 = vertical;

#define BLUE 0xF800
#define RED 0x001F
#define GREEN 0x07E0
#define DISPLAY_SIZE 128*96
#define DISPLAY_SPI_SPEED 10000*1000

typedef struct {
    uint mosi_pin;
    uint miso_pin; 
    uint cs_pin;
    uint sck_pin;
    uint rst_pin;
    uint dc_pin;
    uint width;
    uint height;
    spi_inst_t* spi;
} SSD1351;

void initHardware();
void initDisplay();
void writeCommand(uint8_t cmd);
void writeData(uint8_t data);
void setDisplaySize();
void clearDisplay();
void writeRAM();
void fillColor(uint16_t color);
void setStartLine();
void setPixel(int x, int y, uint16_t color);
void writeChar(int x, int y, uint16_t color, char chr);
void writeString(int x, int y, uint16_t color, char *str);