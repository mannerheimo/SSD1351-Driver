#include "SSD1351.h"
#include "Fonts/font8x8_basic.h"

//datasheet from below
//https://www.waveshare.com/wiki/1.27inch_RGB_OLED_Module#Pinout
//https://files.waveshare.com/upload/a/a7/SSD1351-Revision_1.5.pdf

uint16_t framebuf[DISPLAY_SIZE];
uint16_t framebuf2[DISPLAY_SIZE];
// 2 buffers to maximize fps
uint16_t *displaybuf = framebuf;
uint16_t *drawbuf = framebuf2;

static SSD1351 display = {
    .spi = spi0,
    .spi_tx_dma_channel = 0,
    .dma_transfer_count = 0,
    .total_bytes = DISPLAY_SIZE * 2,
    .mosi_pin = 19,
    .sck_pin = 18,
    .cs_pin = 17,
    .rst_pin = 15,
    .dc_pin = 14,
    .width = 128,
    .height = 96
};


void displayUpdate() {
    uint16_t *temp = drawbuf;
    drawbuf = displaybuf;
    displaybuf = temp;
    writeRAM();
}

void initDMA() {
    display.spi_tx_dma_channel = dma_claim_unused_channel(true); 
    dma_channel_config dma_channel_config = dma_channel_get_default_config(display.spi_tx_dma_channel);

    channel_config_set_transfer_data_size(&dma_channel_config, DMA_SIZE_8);
    channel_config_set_dreq(&dma_channel_config, DREQ_SPI0_TX);
    channel_config_set_read_increment(&dma_channel_config, true);
    channel_config_set_write_increment(&dma_channel_config, false);

    dma_channel_set_config(display.spi_tx_dma_channel, &dma_channel_config, false);


    dma_channel_configure(
        display.spi_tx_dma_channel, 
        &dma_channel_config, 
        &spi_get_hw(display.spi)->dr,
        NULL, 
        0, 
        false
    );

    display.dma_transfer_count = dma_encode_transfer_count(display.total_bytes);

}


// this function allows drawing bitmaps to display, vlsb (vertical least significant bit)
void writeBITMAP(int x, int y, const unsigned char *bitmap, int width, int height, uint16_t color) {
    int byte_index = 0;

    for (int i = 0; i < height; i++) { // looping trought the rows horiz
        for (int j = 0; j < width; j++) { // looping each pixel vertically
            uint16_t byte = bitmap[byte_index++];

            for (int bit = 0; bit < 8; bit++) { // looping trough each bit of the byte

                if ((byte >> bit) & 1) { // if bit 1, then turn on pixel
                    if ((i*8) + bit < height) { 
                    setPixel(x + j, (i*8) + y + bit, color); 
                    }
                }
            }
        }
    }
    
    
}

// draws charachter at x,y pos. for supported charachters, see font8x8_basic.h
void writeChar(int x, int y, uint16_t color, char chr) { //implicit type conversion
    if (chr < 0 || chr > 127) return;
    for (int row = 0; row < 8; row++) { // loop trough each of the 8 rows of (char bitmap)

        for (int col = 0; col < 8; col++){ // loop trough the 8 pixels/columns of the curr row
            
            if ((font8x8_basic[chr][row] >> col) & 1) { // check each bit, if 1 then draw  
                setPixel(x + col, y + row, color);
            }
        }
    }
}

//draws string to display, using writeChar function. 
void writeString(int x, int y, uint16_t color, char *str) {
    int curr_x = x;
    for (int i = 0; str[i] != '\0'; i++) {
        writeChar(x, y, color, str[i]);
        x += 8; // move next char 8 pixels right
        if (x >= display.width) { // change to next row if going out of bounds of display.width
            y += 8; // move next char/string 8 pixels down
            x = curr_x; // go back to original x
        }
    }
    
}

// set x,y pixel in framebuf to whatever color. 
// NOTE: if using this as standalone, you need to writeRAM to see changes in display.
void setPixel(int x, int y, uint16_t color) {
    if (x < 0 || x > display.width - 1 || y < 0 || y > display.height) return;
    drawbuf[y * display.width + x] = color;
}

// fill display with color
void fillColor(uint16_t color) {
    for (int i = 0; i < DISPLAY_SIZE; i++) {
        drawbuf[i] = color;
    }
    
}

void setStartLine() {
    writeCommand(DISPLAY_START_LINE);
    writeData(0X00);
}

void setDisplaySize() {
    //sets width 0-127
    writeCommand(SET_COLUMN_ADDRESS);
    writeData(COLUMN_START_ADDRESS);
    writeData(COLUMN_END_ADDRESS);
    //sets height/row 0-95
    writeCommand(SET_ROW_ADDRESS);
    writeData(ROW_START_ADDRESS);
    writeData(ROW_END_ADDRESS);
    clearDisplay();
    setStartLine();
}

void clearDisplay() {
    memset(drawbuf, 0, sizeof(framebuf));
    
}

//writes command to driver
void writeCommand(uint8_t cmd) {
    gpio_put(display.cs_pin, 0);
    gpio_put(display.dc_pin, 0);
    spi_write_blocking(display.spi, &cmd, 1);
    
    gpio_put(display.cs_pin, 1);
}

//write data to driver
void writeData(uint8_t data) {
    gpio_put(display.cs_pin, 0);
    gpio_put(display.dc_pin, 1);
    spi_write_blocking(display.spi, &data, 1);
    
    gpio_put(display.cs_pin, 1);
}

/*
when writing to drivers ram, DMA is utilized to transfer framebuffer to spi.
*/
void writeRAM(void) {
    while (dma_channel_is_busy(display.spi_tx_dma_channel)); // can't have writing happening at the same time
    
    writeCommand(WRITE_RAM_COMMAND);
    gpio_put(display.cs_pin, 0);
    gpio_put(display.dc_pin, 1);
    
    // send framebuf to spi to ram
    dma_channel_transfer_from_buffer_now(display.spi_tx_dma_channel, displaybuf, display.dma_transfer_count); //tähän vaan se encoded transfer count ja sit pitäös olla kunnos, tee returnit docxygen
    dma_channel_wait_for_finish_blocking(display.spi_tx_dma_channel); // wait until dma is done before next ram write
    while(spi_is_busy(display.spi)); // to catch the last bytes moving trough spi before cutting the writing
    gpio_put(display.cs_pin, 1);

    
    /*
    this is alternative non DMA way to write to ram
    loops trough framebuf pixel by pixel, and splits it into lsb, msb, writes
    for (int i = 0; i < DISPLAY_SIZE; i++) {
        uint16_t pixel = framebuf[i];

        uint8_t msb = (pixel >> 8) & 0xFF;  
        uint8_t lsb = pixel & 0xFF;
        
        spi_write_blocking(display.spi, &msb, 1);
        spi_write_blocking(display.spi, &lsb, 1);
    }
        gpio_put(display.cs_pin, 1); 
    */
    
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