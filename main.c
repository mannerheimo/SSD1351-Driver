#include "SSD1351.h"



int main()
{   
    stdio_init_all();
    sleep_ms(1000);

    initHardware(&display);
    initDisplay(&display);

    while (true)
    {
        sleep_ms(1000);
        writeCommand(&display, 0XA5);
    }
    


}