#include <led.h>
#include <util/delay.h>
#include <avr/io.h> 

int main()
{
    enableAllLeds();
    _delay_ms(300);
    lightDownAllLeds();
    _delay_ms(300);

    while (1)
    {
        for (int i = 0; i < 4; i++){
            lightToggleOneLed(i);
            _delay_ms(200);

        }
    }
    return 0;
    
}