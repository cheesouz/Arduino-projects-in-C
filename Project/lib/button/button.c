#include <util/delay.h>
#include <avr/io.h>
 
#define NB_BUTTONS 3
#define DEBOUNCING 5000
 
void enableOneButton (int buttonNumber){
    if (buttonNumber<1||buttonNumber>NB_BUTTONS) return;
    DDRC &= ~(1 << (PC1 + buttonNumber-1));
    PORTC |= (1 << (PC1 + buttonNumber-1));//Pull up on
}
 
int buttonPushed(int buttonNumber){
    if (buttonNumber<1||buttonNumber>NB_BUTTONS) return 0;
    if ((PINC & (1 << (PC1 + buttonNumber-1))) == 0){
       _delay_us(DEBOUNCING);
       if ((PINC & (1 << (PC1 + buttonNumber-1))) == 0){
           return 1;
       } else {
         return 0;
       }
    }
    return 0;
}
 
int buttonReleased(int buttonNumber) {
    return 1 - buttonPushed(buttonNumber);
}