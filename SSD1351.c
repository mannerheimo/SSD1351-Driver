#include "SSD1351.h"
//datasheet from below
//https://www.waveshare.com/wiki/1.27inch_RGB_OLED_Module#Pinout

SSD1351 display = {
    .spi = spi0,
    .mosi_pin = 19,
    .sck_pin = 18,
    .cs_pin = 17,
    .rst_pin = 15,
    .dc_pin = 14,
    .width = 128,
    .height = 128
};

static SSD1351 *d = NULL;
uint16_t framebuf[128][128];

void setDisplaySize(SSD1351 *d) {
    writeCommand(d, SET_COLUMN_ADDRESS);
    writeData(d, COLUMN_START_ADDRESS);
    writeData(d, COLUMN_END_ADDRESS);

    writeCommand(d, SET_ROW_ADDRESS);
    writeData(d, ROW_START_ADDRESS);
    writeData(d, ROW_END_ADDRESS);
    clearDisplay(d);
}

void clearDisplay(SSD1351 *d) {
    memset(framebuf, 0, 128*128);
    writeRAM(d);
}


void writeCommand(SSD1351 *d, uint8_t cmd) {
    gpio_put(d->cs_pin, 0);
    gpio_put(d->dc_pin, 0);
    spi_write_blocking(d->spi, &cmd, 1);
    
    gpio_put(d->cs_pin, 1);
}


void writeData(SSD1351 *d, uint8_t data) {
    gpio_put(d->cs_pin, 0);
    gpio_put(d->dc_pin, 1);
    spi_write_blocking(d->spi, &data, 1);
    
    gpio_put(d->cs_pin, 1);
}

void writeRAM(SSD1351 *d) {
    writeCommand(d, WRITE_RAM_COMMAND);
    writeData(d, framebuf);

    for (int y = 0; y < d->height; y++) {
        for (int x = 0; x < d->width; x++) {
            uint8_t ls = framebuf[y][x];  
            uint8_t msb = framebuf[y][x]; 
        }
    }
}


void initHardware(SSD1351 *d) {

    spi_init(d->spi, 1000*1000);
    gpio_set_function(d->mosi_pin, GPIO_FUNC_SPI);
    gpio_set_function(d->sck_pin, GPIO_FUNC_SPI);

    gpio_init(d->rst_pin);
    gpio_init(d->dc_pin);
    gpio_init(d->cs_pin);

    gpio_set_dir(d->rst_pin, GPIO_OUT);
    gpio_set_dir(d->dc_pin, GPIO_OUT);
    gpio_set_dir(d->cs_pin, GPIO_OUT);

    setDisplaySize(d);
}


void initDisplay(SSD1351 *d) {
    gpio_put(d->cs_pin, 1); //default low -> must pull always high if not writing, can cause corruption

    //pulling rst low, initializes the chip
    sleep_ms(5);
    gpio_put(d->rst_pin, 0);
    sleep_us(5);
    gpio_put(d->rst_pin, 1);
    sleep_us(5);

    //command lock, can be left open, its only off to minimize noice during startup interfiering with disp
    
    writeCommand(d, SET_COMMAND_LOCK);
    writeData(d, UNLOCK_DRIVER);
    writeCommand(d, DISPLAY_ON);
    sleep_ms(200);

}
// init framebuffer, like 128x96xbits per pixel, this way we can blast full image immedietly
// higher level commands for easy use