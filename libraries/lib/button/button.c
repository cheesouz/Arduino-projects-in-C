#include <util/delay.h>
#include <avr/io.h>
#include <button.h>

#define NB_BUTTONS 3
#define DEBOUNCING 5000
 
void enableOneButton (int buttonNumber){
    if (buttonNumber<1||buttonNumber>NB_BUTTONS) return;
    DDRC &= ~(1 << (PC1 + buttonNumber-1));
    PORTC |= (1 << (PC1 + buttonNumber-1));//Pull up on
}
//for programs without interrupts
void enableAllButtons(){
  for (int i = 0; i < 3; i++){
        enableOneButton(i+1);
    }
}

//for programs with interrupts
void setupButtons(){
  BUTTON_DDR &= ~_BV( BUTTON1 ); 
  BUTTON_DDR &= ~_BV( BUTTON2 );             
  BUTTON_DDR &= ~_BV( BUTTON3 );
  BUTTON_PORT |= _BV( BUTTON1 );  
  BUTTON_PORT |= _BV( BUTTON2 );  
  BUTTON_PORT |= _BV( BUTTON3 );
  PCICR |= _BV( PCIE1 );
  PCMSK1 |= _BV( BUTTON1 );
  PCMSK1 |= _BV( BUTTON2 );
  PCMSK1 |= _BV( BUTTON3 );
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

int buttonPushedWithDebounce(int button) {
    if (buttonPushed(button)) {
        _delay_ms(100);
        if (buttonPushed(button)) {
            return 1;
        }
    }
    return 0;
}
 
int buttonReleased(int buttonNumber) {
    return 1 - buttonPushed(buttonNumber);
}