#include "SSD1351.h"
#include "Fonts/font8x8_basic.h"

//datasheet from below
//https://www.waveshare.com/wiki/1.27inch_RGB_OLED_Module#Pinout

static SSD1351 display = {
    .spi = spi0,
    .mosi_pin = 19,
    .sck_pin = 18,
    .cs_pin = 17,
    .rst_pin = 15,
    .dc_pin = 14,
    .width = 128,
    .height = 96
};

uint16_t framebuf[DISPLAY_SIZE];

void writeChar(int x, int y, uint16_t color, char chr) { //implicit type conversion
    if (chr < 0 || chr > 127) return; // prevent font8x8 out of bound 
    for (int row = 0; row < 8; row++) { // loop trough each of the 8 rows of char bitmap

        for (int col = 0; col < 8; col++){ // loop trough the 8 pixels/columns of the curr row
            
            if ((font8x8_basic[chr][row] >> col) & 1) { // check each bit, if 1 then draw  
                setPixel(x + col, y + row, color);
            }
        }
    }
}

void writeString(int x, int y, uint16_t color, char *str) {
    int curr_x = x;
    for (int i = 0; str[i] != '\0'; i++) {
        writeChar(x, y, color, str[i]);
        x += 8; // move next char 8 pixels right
        if (x >= display.width) { // change to next row if going out of bounds of display.width
            y += 8;
            x = curr_x;
        }
    }
    writeRAM();
}

void setPixel(int x, int y, uint16_t color) {
    if (x < 0 || x > display.width - 1 || y < 0 || y > display.height) return;
    framebuf[y * display.width + x] = color;
}

void fillColor(uint16_t color) {
    for (int i = 0; i < DISPLAY_SIZE; i++) {
        framebuf[i] = color;
    }
    writeRAM();
}

void setStartLine() {
    writeCommand(DISPLAY_START_LINE);
    writeData(0X00);
}

void setDisplaySize() {
    writeCommand(SET_COLUMN_ADDRESS);
    writeData(COLUMN_START_ADDRESS);
    writeData(COLUMN_END_ADDRESS);

    writeCommand(SET_ROW_ADDRESS);
    writeData(ROW_START_ADDRESS);
    writeData(ROW_END_ADDRESS);
    clearDisplay();
    setStartLine();
}

void clearDisplay() {
    memset(framebuf, 0, sizeof(framebuf));
    writeRAM();
}


void writeCommand(uint8_t cmd) {
    gpio_put(display.cs_pin, 0);
    gpio_put(display.dc_pin, 0);
    spi_write_blocking(display.spi, &cmd, 1);
    
    gpio_put(display.cs_pin, 1);
}


void writeData(uint8_t data) {
    gpio_put(display.cs_pin, 0);
    gpio_put(display.dc_pin, 1);
    spi_write_blocking(display.spi, &data, 1);
    
    gpio_put(display.cs_pin, 1);
}

void writeRAM(void) {
    writeCommand(WRITE_RAM_COMMAND);

    gpio_put(display.cs_pin, 0);
    gpio_put(display.dc_pin, 1);

    for (int i = 0; i < DISPLAY_SIZE; i++) {
        uint16_t pixel = framebuf[i];

        uint8_t msb = (pixel >> 8) & 0xFF;  
        uint8_t lsb = pixel & 0xFF;
        
        spi_write_blocking(display.spi, &msb, 1);
        spi_write_blocking(display.spi, &lsb, 1);
    }

    gpio_put(display.cs_pin, 1);
}


void initHardware() {

    spi_init(display.spi, DISPLAY_SPI_SPEED); // could prob push higher speed
    gpio_set_function(display.mosi_pin, GPIO_FUNC_SPI);
    gpio_set_function(display.sck_pin, GPIO_FUNC_SPI);

    gpio_init(display.rst_pin);
    gpio_init(display.dc_pin);
    gpio_init(display.cs_pin);

    gpio_set_dir(display.rst_pin, GPIO_OUT);
    gpio_set_dir(display.dc_pin, GPIO_OUT);
    gpio_set_dir(display.cs_pin, GPIO_OUT);
}


void initDisplay() {
    gpio_put(display.cs_pin, 1); //default low -> must pull always high if not writing, can cause corruption

    //pulling rst low, initializes the chip
    sleep_ms(5);
    gpio_put(display.rst_pin, 0);
    sleep_us(5);
    gpio_put(display.rst_pin, 1);
    sleep_us(5);

    //command lock, can be left open, its only off to minimize noice during startup interfiering with disp
    
    writeCommand(SET_COMMAND_LOCK);
    writeData(UNLOCK_DRIVER);

    writeCommand(REMAP); //each bit represent a setting check datasheet
    //0010 1110
    writeData(0x74);



    

    writeCommand(SET_MULTIPLEX_RATIO);
    writeData(127); //96 rows 0-95

    setDisplaySize();
    setStartLine();

    writeCommand(DISPLAY_ON);
    sleep_ms(200);
    
}