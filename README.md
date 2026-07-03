
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
initDMA(); // initializes DMA
initHardware(); // initializes GPIOS
initDisplay(); // initializes display sequence and sets the screen size and other required settings



```   
    
    
    
    
    
    
    





