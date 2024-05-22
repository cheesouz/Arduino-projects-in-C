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
#define INITIAL_GAME_SPEED 400


static int counter = 0;
static int dinoPos = 1;
static int dinoMoveFlag = 1;
static int gameSpeed = INITIAL_GAME_SPEED;
static int lives = 4;
static int points = 0;
static uint8_t arrowPosition = 4;
static uint8_t arrowHeight = 1;

/* Segment byte maps for dino's positions: up, normal, down */
const uint8_t DINO_POS[] =  {0x9C, 0xA3, 0xF7};

/* Segment byte maps for arrow's positions: up, normal, down */
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

void shootNewArrow(){
  arrowHeight = rand()%3;
  arrowPosition = 4;
}

void loseLife(){
  lives--;
  gameSpeed = INITIAL_GAME_SPEED;
  printf("You lost a life!\n");
  //to ensure that the correct number of lives is displayed
  lightDownAllLeds();
  //ToDo: play sound
}

void handleCollision() {
  bool collision = 0;
  switch (arrowHeight) {
      case 0:
          collision = (dinoPos == 0);
          break;
      case 1:
          collision = (dinoPos != 2);
          break;
      case 2:
          collision = (dinoPos != 0);
          break;
  }
  if (collision) {
      loseLife();
  } else {
      points++;
      printf("Points: %d\n", points);
  }
}

ISR( PCINT1_vect ){
  if (bit_is_clear(BUTTON_PIN, BUTTON1)){
    //debounce
    _delay_ms( 50 );
    if (bit_is_clear(BUTTON_PIN, BUTTON1)){
      //ToDo: Check if dino is in a neutral position
      if (dinoMoveFlag == 1){ dinoMoveFlag = 0; }
  }
  }
  if (bit_is_clear(BUTTON_PIN, BUTTON3)){
    //debounce
    _delay_ms( 50 );
    if (bit_is_clear(BUTTON_PIN, BUTTON3)){
      if (dinoMoveFlag == 1){ dinoMoveFlag = 2; }
  }
  }
}

ISR(TIMER0_OVF_vect) {
    // static uint8_t currentSegment = 4;
    // currentSegment = currentSegment % 4 + 1;

    static int savedArrowCounter = INITIAL_GAME_SPEED;
    static int savedDinoCounter = 0;

    lightUpMultipleLeds(lives);

    counter++;

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
        //this is where we shoot the new arrow
        if (arrowPosition == 1){
            handleCollision();
            shootNewArrow();
        } else {
            arrowPosition--;
        }
        //relative time of when was the last time the arrow was moved
        savedArrowCounter = counter;
    }

    //lazy way...
    if (points > 0 && points % 10 == 0) {
        gameSpeed = (int)(gameSpeed * 0.8 + 0.5);
        printf("Time to speed up! Game Speed: %d\n", gameSpeed);
        points++;  // Increment points to avoid repeated speeding up on the same point threshold
    }

}


int main(){
  initUSART();
  initDisplay();
  printf("START\n");
  initTimer0();
  setupButtons();
  //ToDo: add potentiometer as a seed for random
  srand(0);
  enableAllLeds();
  lightDownAllLeds();

  while (lives > 0){
     sei();

  }
  cli();
  printf("Game lost\n");
  printf("Points: %d\n", points);
  writeStringAndWait("GAME", 800);
  writeStringAndWait("DONE", 800);
  writeString("    ");

  //ToDo: add sound at the end of the game
  return 0;
}