#include "SSD1351.h"
//datasheet from below
//https://www.waveshare.com/wiki/1.27inch_RGB_OLED_Module#Pinout

SSD1351 display = {
    .spi = spi0,
    .mosi_pin = 19,
    .sck_pin = 18,
    .cs_pin = 17,
    .rst_pin = 15,
    .dc_pin = 14
};

static SSD1351 *d = NULL;

void setDisplaySize(SSD1351 *d) {
    writeCommand()
    /*writecommand set colums address
    write data 0x00
    write data 0x7f
    sama row

    sit clear
    */
}

void clearDisplay(SSD1351 *d) {
    
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