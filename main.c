#include "SSD1351.h"



int main()
{   
    stdio_init_all();
    sleep_ms(1000);

    initHardware();
    initDisplay();
    setDisplaySize();
    

    fillColor(0xFFFF);
    while (true) {
        sleep_ms(1000);
        
        writeString(0, 0, 0x0000, "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Etiam vitae augue sem. Phasellus et volutpat metus.");
        
    }    
}