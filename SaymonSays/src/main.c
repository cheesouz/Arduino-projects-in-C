#include <usart.h>
#include <stdio.h>
#include <led.h>
#include <stdlib.h>
#include <button.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

#define ARRAY_LEN 10
#define DELAY 1600

int button_pushed = 0;
int counter = 0;
uint8_t sequence[ARRAY_LEN];
uint8_t playerSequence[ARRAY_LEN];
int currentButton = 10;

void generatePuzzle(uint8_t arr[], int arrLen){
    for (int i = 0; i<arrLen; i++){
       arr[i] = rand() % 3;
    }
}
void printPuzzle(uint8_t arr[], int arrLen){
    printf("[");
    for (int i = 0; i<arrLen; i++){
        printf("%d ", arr[i] + 1);
    }
    printf("]\n");
}

void playPuzzle(uint8_t arr[], int num){
    for (size_t i = 0; i < num; i++)
    {
        lightUpOneLed(arr[i]);
        _delay_ms(DELAY);
        lightDownOneLed(arr[i]);
        _delay_ms(DELAY/2);
    }
    printf("Now it's your turn!\n");
}

int readInput(uint8_t arr[], int num){
    int i = 0;
    //wait for the player to press a button
    
    while (i < num)
    {
            while (currentButton == 10)
        {
            _delay_ms(50);
        }

        if (currentButton != 10){
            if (sequence[i] == currentButton){
                printf("You pressed button %d, correct!\n", currentButton + 1);
            } else {
            printf("You pressed button %d, incorrect!\n", currentButton + 1);
            return 0;
            }
        }
        currentButton = 10;
        i++;
    }
    printf("Correct, we go to the next level\n");
    for (size_t i = 0; i < 8; i++)
    {
        lightToggleOneLed(3);
        _delay_ms(200);
    }
    lightDownAllLeds();
    
    return 1;
    
}

ISR( PCINT1_vect ){
  if (bit_is_clear(BUTTON_PIN, BUTTON1)){
    //debounce
    _delay_ms( 50 );
    if (bit_is_clear(BUTTON_PIN, BUTTON1)){
      button_pushed = 1;
      currentButton = 0;
      }
  }
  if (bit_is_clear(BUTTON_PIN, BUTTON2)){
    //debounce
    _delay_ms( 50 );
    if (bit_is_clear(BUTTON_PIN, BUTTON2)){
      currentButton = 1;
      }
  }
  if (bit_is_clear(BUTTON_PIN, BUTTON3)){
    //debounce
    _delay_ms( 50 );
    if (bit_is_clear(BUTTON_PIN, BUTTON3)){
        currentButton = 2;
  }
  }
}

int main() {
    sei();
    initUSART();
    enableAllLeds();
    lightDownAllLeds();
    setupButtons();
    printf("Welcome to Simon Says! Press button 1 to start the game.\n");

    while (!button_pushed){
        counter++;
        _delay_ms(100);
        lightToggleOneLed(3);
    }
    currentButton = 10;
    lightDownOneLed(3);
    srand(counter);
    generatePuzzle(sequence, ARRAY_LEN);

    int gameOn = 1;
    int i = 0;
    while (gameOn && i < 10) {
        printf("\nLevel %d\n", i + 1);
        playPuzzle(sequence, i + 1);
        gameOn = readInput(sequence, i + 1);
        i++;
    }
    if (i == 10){
        printf("Congratulations, you are the Simon Master!\n");
    } else {
        printf("You lost, the correct sequence was: ");
        printPuzzle(sequence, ARRAY_LEN);
    }

    return 0;
}