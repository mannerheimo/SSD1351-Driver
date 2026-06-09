#include "SSD1351.h"



int main()
{   
    stdio_init_all();
    sleep_ms(1000);

    initHardware();
    initDisplay();
    setDisplaySize();
    
    fillColor(0xFF);
    setPixel(0, 0, 0x0000);
    setPixel(1, 0, 0x0000);
    setPixel(2, 0, 0x0000);


    while (true) {
        sleep_ms(1000);
    }
    


}