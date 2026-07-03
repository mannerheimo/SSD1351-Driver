
# C library for SSD1351 1.27" OLED display

Optimized bare-metal C driver for the SSD1351 1.27" (Configured for 128x96 resolution) RGB OLED Display. Using Raspberry Pi Pico C SDK.

NOTE: This project was purely for fun and learning. There is better alternatives out there.

## Highlights
- Asynchronous DMA transfers 
  - Frees up the CPU by offloading framebuffer transfers to dedicated hardware DMA channel.
- Double buffering 
  - Utilizes two framebuffers (displaybuf and drawbuf) in RAM to maximise the FPS and eliminate screen tearing. While DMA pushes    displaybuf to display, the CPU renders the next frame to drawbuf safely.

- Custom graphics and Font Engine:      
  - Draws characters and strings in 8x8 resolution to display.
  - Handles custom bitmap rendering using VLSB (Vertical least significant bit) decoding.

- Solid SPI Management
  - Implemented strict hardware timing controls to protect against signal noice and data corruption during fast RAM writes.


## Pin layout
add
## Example code 
``` 

    // initializes the GPIOS and DMA
    initHardware();
    initDMA();

    // Runs SSD1351 initialization sequence and sets display settings
    initDisplay(); 

    // Render graphics to draw buffer
    fillColor(0x0000); 
    writeString(0, 0, 0xFFFF, "Blinky");

    // Swap buffers and push to display via DMA
    displayUpdate();
```

## API reference
```
initDMA(); // Initializes the hardware DMA channel configuration.
initHardware(); // Configures the hardware SPI peripheral and GPIO directions.
initDisplay(); // Runs the startup sequence, sets column/row boundaries, and powers on the panel.

fillColor(uint16_t color); // Fills the entire active drawbuf with a specific 16-bit color.
clearDisplay(); // Flushes the active drawbuf back to raw zeros (black).
setPixel(int x, int y, uint16_t color); // Sets a specific pixel in the drawbuf.
writeChar(int x, int y, uint16_t color, char chr); // Renders a single 8x8 character into the drawbuf using data from font8x8_basic.h.
writeString(int x, int y, uint16_t color, char *str); // Renders an entire string, spacing letters by 8 pixels and wrapping lines automatically.
writeBITMAP(int x, int y, const unsigned char *bitmap, int width, int height, uint16_t color); // Decodes and draws a custom VLSB bitmap into the drawbuf.

displayUpdate(); // Swaps the drawing and displaying buffers, commands the display to receive data, and blocks until the DMA/SPI pipelines empty to guarantee a safe transfer.
```   
    
    
    
    
    
    
    





