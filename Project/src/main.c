#include <Arduino.h>
#include <usart.h>
#include <display.h>
#include <avr/io.h>
#include <util/delay.h>
#include <button.h>
#include <stdlib.h>

#define BUTTON_PORT PORTC
#define BUTTON_PIN PINC
#define BUTTON_DDR DDRC
#define BUTTON1 PC1
#define BUTTON3 PC3

//how long it takes the arrow to reach the dinosaur
#define ARROW_SPEED 9

/* Segment byte maps for dino's positions: up, normal, down */
const uint8_t DINO_POS[] =  {0x9C, 0xA3, 0xF7};

/* Segment byte maps for arrow's positions */
const uint8_t ARROW_POS[] =  {0xFE, 0xBF, 0xF7};

/* Byte maps to select digit 1 to 4 */
const uint8_t SEGMENTS[] = {0xF1, 0xF2, 0xF4, 0xF8};

void moveArrow(uint8_t segment, uint8_t height) {
  cbi(PORTD, LATCH_DIO);
  shift(height, MSBFIRST);
  shift(SEGMENTS[segment], MSBFIRST);
  sbi(PORTD, LATCH_DIO);
}

void shootArrow(){ 
  srand(0);
  uint8_t arrowHeight = ARROW_POS[rand() %3];
  for (int i = 3; i > 0; i--){
    moveArrow(i, arrowHeight);
    delay(ARROW_SPEED/3);
  }

}

void setupButtons(){
  BUTTON_DDR &= ~_BV( BUTTON1 );          
  BUTTON_DDR &= ~_BV( BUTTON3 );
  BUTTON_PORT |= _BV( BUTTON1 );  
  BUTTON_PORT |= _BV( BUTTON3 );
  //in Pin Change Interrupt Control Register activate port C
  PCICR |= _BV( PCIE1 );
  PCMSK1 |= _BV( BUTTON1 );
  PCMSK1 |= _BV( BUTTON3 );
}

void changeDinoPos(uint8_t value) {
  cbi(PORTD, LATCH_DIO);
  shift(DINO_POS[value], MSBFIRST);
  shift(0xF1, MSBFIRST);
  sbi(PORTD, LATCH_DIO);
}

ISR( PCINT1_vect ){
  if (bit_is_clear(BUTTON_PIN, BUTTON1)){
    //debounce
    _delay_ms( 50 );
    if (bit_is_clear(BUTTON_PIN, BUTTON1)){
      printf("Button 1 pressed in interrupt\n");
      //ToDo: Check if dino is in a neutral position
      changeDinoPos(0);
      _delay_ms(400);
      changeDinoPos(1);
  }
  }
  if (bit_is_clear(BUTTON_PIN, BUTTON3)){
    //debounce
    _delay_ms( 50 );
    if (bit_is_clear(BUTTON_PIN, BUTTON3)){
      printf("Button 2 pressed in interrupt\n");
      //ToDo: Check if dino is in a neutral position
      changeDinoPos(2);
      _delay_ms(400);
      changeDinoPos(1);
  }

  }

}

int main(){
  initUSART();
  initDisplay();
  printf("START\n");
  
  setupButtons();
  sei();

  while (1){
    shootArrow();
    
  }
  
  return 0;
}