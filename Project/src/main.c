#include <usart.h>
#include <display.h>
#include <avr/io.h>
#include <util/delay.h>
#include <button.h>
#include <stdlib.h>
#include <led.h>
#include <buzzer.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define INITIAL_GAME_SPEED 500
#define INITIAL_NUMBER_OF_LIVES 4

/* Segment byte maps for dino's positions: up, normal, down */
const uint8_t DINO_POS[] =  {0x9C, 0xA3, 0xF7};

/* Segment byte maps for arrow's positions: up, normal, down */
const uint8_t ARROW_HEIGHT[] =  {0xFE, 0xBF, 0xF7};

static int counter = 0;
static int dinoPos = 1;
static int dinoMoveFlag = 1;
static int gameSpeed = INITIAL_GAME_SPEED;
static int currentScore = 0;
static int lives = INITIAL_NUMBER_OF_LIVES;
static int updateArrayOfScoresFlag = 0;
static int lastSpeedIncreaseScore = 0;

// initiating arrowPosition and arrowHeight using pointers
  static int arrowPosition = 4;
  int* pArrowPosition = &arrowPosition;

  static int arrowHeight = 1;
  int* pArrowHeight = &arrowHeight;

void initADC()
{
    ADMUX |= (1 << REFS0);                                // Set up the reference voltage. We choose 5V as the reference.
    ADMUX &= ~(1 << MUX3  ) & ~(1 << MUX2  ) & ~(1 << MUX1 ) & ~(1 << MUX0 );
                                                          //Set MUX0-3 to zero to read analog input from PC0
                                                          //Default is 0000 so this setting is not really necessary     
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Determine the sample rate by setting the division factor to 128.
    ADCSRA |= (1 << ADEN);                                // Enable the ADC
    ADCSRA |= (1 << ADATE);                               // Enable ADC Auto Triggering
    ADCSRB = 0;                                                // Set ADC Auto Trigger Source to Free Running Mode (default)
    ADCSRA |= (1 << ADSC);                                // Start the analog-to-digital conversion
}

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

void displayArrow(int segment, int height) {
  cbi(PORTD, LATCH_DIO);
  shift(ARROW_HEIGHT[height], MSBFIRST);
  shift(SEGMENT_SELECT[segment-1], MSBFIRST);
  sbi(PORTD, LATCH_DIO);
}

void displayDino(uint8_t value) {
  cbi(PORTD, LATCH_DIO);
  shift(DINO_POS[value], MSBFIRST);
  shift(0xF1, MSBFIRST);
  sbi(PORTD, LATCH_DIO);
}

void shootNewArrow(){
  *pArrowHeight = rand()%3;
  *pArrowPosition = 4;
}

void loseLife(){
  updateArrayOfScoresFlag = 1;
  lives--;
  gameSpeed = INITIAL_GAME_SPEED;
  lastSpeedIncreaseScore = 0;
  printf("You lost a life!\n");
  dinoPos = 1;
  dinoMoveFlag = 1;
  //to ensure that the correct number of lives is displayed
  lightDownAllLeds();
  playTone(A5, 800);
}

void handleCollision(int *arrowHeight) {
  int collision = 0;
  switch (*arrowHeight) {
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
      currentScore++;
      //printf("Current score: %d\n", currentScore);
  }
}

void increaseSpeed() {
    if (currentScore > lastSpeedIncreaseScore && (currentScore - lastSpeedIncreaseScore) >= 6) {
        gameSpeed = (int)(gameSpeed * 0.8 + 0.5);
        printf("Time to speed up! Game Speed: %d\n", gameSpeed);
        lastSpeedIncreaseScore = currentScore;
    }
}

ISR( PCINT1_vect ){
  if (bit_is_clear(BUTTON_PIN, BUTTON1)){
    //debounce
    _delay_ms( 50 );
    if (bit_is_clear(BUTTON_PIN, BUTTON1)){
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
    static int savedArrowCounter = INITIAL_GAME_SPEED;
    static int savedDinoCounter = 0;

    counter++;

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
    displayDino(dinoPos);

    //update arrow
    if (counter - savedArrowCounter >= gameSpeed) {
        displayArrow(*pArrowPosition, *pArrowHeight);
        if (*pArrowPosition == 1) {
            handleCollision(pArrowHeight);
            shootNewArrow();
        } else {
            (*pArrowPosition)--;
        }
        //relative time of when was the last time the arrow was moved
        savedArrowCounter = counter;
    }
}

int main(){
  initUSART();
  initDisplay();
  initTimer0();
  setupButtons();
  clearDisplay();
  printf("\n\n\nSTART\n");

  enableAllLeds();
  lightDownAllLeds();

  initADC();
  printf("Rotate the potentiometer to generate a seed.\n");
  niceLedLightUp(200);
  uint16_t value = ADC;
  printf("Seed value: %d\n", value);
  srand(value);

  int* arrayOfScores = calloc(INITIAL_NUMBER_OF_LIVES, sizeof(int));

  writeStringAndWait("GO  ", 800);
  sei();

  while (lives > 0){
    lightUpMultipleLeds(lives);
    displayArrow(*pArrowPosition, *pArrowHeight);

    increaseSpeed();

    if (updateArrayOfScoresFlag){
      updateArrayOfScoresFlag = 0;
      int idx = INITIAL_NUMBER_OF_LIVES - lives - 1;
      printf("\nScore from round %d: %d\n\n", idx, currentScore);
      arrayOfScores[idx] = currentScore;
      currentScore = 0;
    }
  }
  cli();

  printf("\nGame lost\n\n");
  playTone(C6, 1500);

  int totalScore = 0;

  for (int i = 0; i < INITIAL_NUMBER_OF_LIVES; i++)
  {
    totalScore += arrayOfScores[i];
    printf("Points from round %d: %d\n", i+1, arrayOfScores[i]);
  }
  printf("Total score: %d\n\n", totalScore);
  free(arrayOfScores);

  writeStringAndWait("GAME", 800);
  writeStringAndWait("OVER", 800);
  writeNumberAndWait(totalScore, 800);
  writeString("    ");

  //ToDo: add sound at the end of the game
  
  return 0;
}