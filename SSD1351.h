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

#define COLUMN_START_ADDRESS 0x00
#define COLUMN_END_ADDRESS 0x7F
#define ROW_START_ADDRESS 0x00
#define ROW_END_ADDRESS 0x7F

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

extern SSD1351 display;


void initHardware(SSD1351 *d);
void initDisplay(SSD1351 *d);
void writeCommand(SSD1351 *d, uint8_t cmd);
void writeData(SSD1351 *d, uint8_t data);
void setDisplaySize(SSD1351 *d);
void clearDisplay(SSD1351 *d);
void writeRAM(SSD1351 *d);