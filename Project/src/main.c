#include <Arduino.h>
#include <usart.h>
#include <display.h>
#include <avr/io.h>
#include <util/delay.h>
#include <button.h>
#include <stdlib.h>
#include <led.h>

#define BUTTON_PORT PORTC
#define BUTTON_PIN PINC
#define BUTTON_DDR DDRC
#define BUTTON1 PC1
#define BUTTON3 PC3


static int counter = 0;
static int dinoPos = 1;
static int dinoMoveFlag = 1;
const int gameSpeed = 400;

/* Segment byte maps for dino's positions: up, normal, down */
const uint8_t DINO_POS[] =  {0x9C, 0xA3, 0xF7};

/* Segment byte maps for arrow's positions */
const uint8_t ARROW_HEIGHT[] =  {0xFE, 0xBF, 0xF7};

/* Byte maps to select digit 1 to 4 */
const uint8_t SEGMENTS[] = {0xF1, 0xF2, 0xF4, 0xF8};

void initTimer0()
{
    // STEP 1: choose the WAVE FORM and by consequence the Mode of Operation
    // We choose FAST PWM; TCNT0 counts to 255
    TCCR0A |= _BV(WGM00) | _BV(WGM01); // WGM00 = 1 and WGM01 = 1 --> Fast PWM Mode

    // STEP 2: *always* set a PRESCALER, otherwise the timer won't count
    // The counting speed is determined by the CPU clock (16 Mhz) divided by this factor
    TCCR0B |= _BV(CS01) | _BV(CS00); // CS00 and CS01 = 1 --> prescaler, factor is now 64
    // STEP 3: enable INTERRUPTS
    // Enable interrupts for overflow: TCNT0 == TOP
    TIMSK0 |= _BV(TOIE0); // enable overflow interrupt
}
void clearDisplay(){
    cbi(PORTD, LATCH_DIO);
    shift(0x00, MSBFIRST); 
    shift(0x00, MSBFIRST); 
    sbi(PORTD, LATCH_DIO);
}

void displayArrow(uint8_t segment, uint8_t height) {
  clearDisplay();
  cbi(PORTD, LATCH_DIO);
  shift(height, MSBFIRST);
  shift(SEGMENTS[segment-1], MSBFIRST);
  sbi(PORTD, LATCH_DIO);
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

void displayDino(uint8_t value) {
  clearDisplay();
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
      if (dinoMoveFlag == 1){ dinoMoveFlag = 0; }
  }
  }
  if (bit_is_clear(BUTTON_PIN, BUTTON3)){
    //debounce
    _delay_ms( 50 );
    if (bit_is_clear(BUTTON_PIN, BUTTON3)){
      printf("Button 2 pressed in interrupt\n");
      if (dinoMoveFlag == 1){ dinoMoveFlag = 2; }
  }

  }

}

ISR(TIMER0_OVF_vect) {
    static uint8_t currentSegment = 4;
    static uint8_t arrowHeight = 1; // Position index
    static uint8_t arrowPosition = 4;
    static int savedArrowCounter = gameSpeed;
    static int savedDinoCounter = 0;

    counter++;

    currentSegment = currentSegment % 4 + 1;

    displayArrow(arrowPosition, ARROW_HEIGHT[arrowHeight]);
    displayDino(dinoPos);

    //dino recieves the information that it's supposed to move
    if (dinoMoveFlag != 1){
      //dino is in the neutral position and wants to jump
      if (dinoMoveFlag == 0 && dinoPos == 1){
        savedDinoCounter = counter;
        dinoPos = 0;
      }
      //dino is in the neutral position and wants to duck
      if (dinoMoveFlag == 2 && dinoPos == 1){
        savedDinoCounter = counter;
        dinoPos = 2;
      }
      //dino is not in the default pos. check if enough time has passed for it to return to default position
      if (dinoMoveFlag == dinoPos ) {
        if (counter - savedDinoCounter == gameSpeed * 2) {
          dinoPos = 1;
          dinoMoveFlag = 1;
          savedDinoCounter = 0; //redundant?
        }
      }

    }
    
    //update arrow
    if (counter - savedArrowCounter == gameSpeed){
      printf("Arrow: ");
      //this is where we shoot the new arrow
      if (arrowPosition == 1){
        arrowHeight = rand()%3;
        arrowPosition = 4;
        printf("new arrow on %d\n", arrowPosition);
      } else {
        arrowPosition--;
        printf("arrow moved at %d\n", arrowPosition);
      }
      //relative time of when was the last time the arrow was moved
      savedArrowCounter = counter;
    }
    
}


int main(){
  initUSART();
  initDisplay();
  printf("START\n");
  initTimer0();
  setupButtons();
  srand(0);
  enableAllLeds();


  int lives = 4;
  while (lives > 0){
    sei();
    lightUpMultipleLeds(lives);

  }
  
  return 0;
}