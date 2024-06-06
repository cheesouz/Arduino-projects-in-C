#define __DELAY_BACKWARD_COMPATIBLE__  
#include <util/delay.h>
#include <avr/io.h>

#define NUMBER_OF_LEDS 4 //Define is a "preprocessor directive". It ensures that every NUMBER_OF_LEDS will be replaced by 4 in the following code

void enableOneLed ( int lednumber ) //C has no classes; functions can be included directly in the .c file.
{
    if ( lednumber < 0 || lednumber > NUMBER_OF_LEDS-1 ) return;
    DDRB |= ( 1 << ( PB2 + lednumber ));    //Check the tutorial "Writing to a Pin". We know from the documentation on
                                            //the multifunctional shield that the LEDs start at PB2
}

void lightUpOneLed ( int lednumber )    //Note: enabled LEDs light up immediately ( 0 = on )
{
    if ( lednumber < 0 || lednumber > NUMBER_OF_LEDS-1 ) return;
    PORTB &= ~( 1 << ( PB2 + lednumber ));  //Check the tutorial on "Bit Operations" to know what happens in this line.
}

void lightDownOneLed ( int lednumber )
{
    if ( lednumber < 0 || lednumber > 3 ) return;
    PORTB |= ( 1 << ( PB2 + lednumber ));   //Make sure you understand this line as well!
}

void enableMultipleLeds(uint8_t numberOfLeds){
    for (int i = 0; i < numberOfLeds; i++){
        enableOneLed(i);
    }
}

void enableAllLeds (){
    DDRB |= 0b00111100;
}

void lightUpMultipleLeds (uint8_t numberOfLeds){
    for (int i = 0; i < numberOfLeds; i++){
        lightUpOneLed(i);
    }
}; 

void lightUpAllLeds (){
    PORTB = 0b00000000;
};


void lightDownMultipleLeds (uint8_t numberOfLeds){
    for (int i = 0; i < numberOfLeds; i++){
        lightDownOneLed(i);
    }
}; 

void lightDownAllLeds (){
    PORTB = 0b00111100;
};

void lightToggleOneLed(int lednumber){
    if ( lednumber < 0 || lednumber > 3 ) return;
    PORTB ^= (1 << ( PB2 + lednumber ));
};

void dimLed (int lednumber, int percentage, int duration){
    if (lednumber < 0 || lednumber > 3 || percentage > 100 || percentage < 0) return;

    int timeOn = percentage/10;
    int timeOff = 10 - timeOn;
    for (int i = 0; i< (duration); i++){
        lightUpOneLed(lednumber);
        _delay_ms(timeOn);
        lightDownOneLed(lednumber);
        _delay_ms(timeOff);
        };

};

void fadeInLed (int led, int duration){
for (int i = 1; i < duration; i++){
    int currentDimLevel = i*100/duration;
    dimLed(led, currentDimLevel, duration/100);
};
};

void niceLedLightUp(int duration){
    fadeInLed(0, duration);
    int ledsOn = 1;
    while (ledsOn < NUMBER_OF_LEDS)
    {
        lightUpMultipleLeds(ledsOn);
        fadeInLed(ledsOn, duration);
        ledsOn++;
    }
    
    
}