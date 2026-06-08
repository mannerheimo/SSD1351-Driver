#include "SSD1351.h"



int main()
{   
    stdio_init_all();
    sleep_ms(1000);

    initHardware(&display);
    initDisplay(&display);
    setDisplaySize(&display);
    while (true)
    {
        sleep_ms(500);
        fillColor(&display, RED);
        sleep_ms(500);
        fillColor(&display, BLUE);
    }
    


}