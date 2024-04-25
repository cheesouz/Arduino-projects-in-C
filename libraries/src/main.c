#include <led.h>
#include <util/delay.h>
#include <avr/io.h> 

int main()
{
    enableAllLeds();
    _delay_ms(300);
    lightDownAllLeds();
    _delay_ms(300);

    // while (1)
    // {
    //     for (int i = 0; i < 4; i++){
    //         lightToggleOneLed(i);
    //         _delay_ms(200);

    //     }
    // }
    while (1)
    {
        // dimLed(0, 20, 200);
        // dimLed(1, 40, 200);
        // dimLed(2, 60, 200);
        // dimLed(3, 80, 200);
        fadeInLed(0, 300);
        lightUpOneLed(1);
        _delay_ms(300);
        lightDownOneLed(1);
    
        
    }
    
    return 0;
    
}