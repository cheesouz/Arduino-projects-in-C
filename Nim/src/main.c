#include <usart.h>
#include <stdio.h>
#include <stdlib.h>
#include <button.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include <display.h>


#define START_NUMBER 21
#define MAX_NUMBER 3

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

void display(int pick, int playerTurn, int numberOfMatches) {
    char letter = playerTurn == 1 ? 'P' : 'C';
    writeNumberToSegment(0, pick);
    writeCharToSegment(1, letter);
    writeNumberToSegment(2, numberOfMatches / 10);
    writeNumberToSegment(3, numberOfMatches % 10);
}

int main() {
    enableAllButtons();
    initDisplay();
    initUSART();
    sei();

    initADC();
    printf("Rotate the potentiometer to generate a seed.\n");
    _delay_ms(5000);
    int seed = ADC;
    printf("Seed value: %d\n", seed);
    srand(seed);

    // Show the seed as long as the player doesn't start the game
    while (!buttonPushedWithDebounce(1)) {
        writeNumber(seed);
    }

    printf("START\n");
    int playerTurn = rand() % 2;
    if (playerTurn){
        printf("Pick an appropiate number of matches and press button number 2 to continue\n");
    }
    int matchesLeft = START_NUMBER;
    int gameOn = 1;
    int pick = 2;

    int *matchesHistory = (int *)malloc(START_NUMBER * sizeof(int));
    int turnCount = 0;

    while (gameOn) {
        display(pick, playerTurn, matchesLeft);

        if (playerTurn) {
            if (buttonPushedWithDebounce(1) && pick > 1) {
                pick--;
            }
            if (buttonPushedWithDebounce(3) && pick < MAX_NUMBER) {
                pick++;
            }
            if (buttonPushedWithDebounce(2)) {
                matchesLeft -= pick;
                playerTurn = 0;
                matchesHistory[turnCount] = matchesLeft;
                turnCount++;
            }
        } else {
            pick = (matchesLeft - 1) % (MAX_NUMBER + 1);
            if (pick == 0){
                pick = (rand() % MAX_NUMBER) + 1;
            }
            printf("Computer picked %d matches, press button number 2 to continue\n", pick);
            while (!buttonPushedWithDebounce(2)){
                display(pick, playerTurn, matchesLeft);
            }
            matchesLeft -= pick;
            printf("Pick an appropiate number of matches and press button number 2 to continue\n");
            pick = 2; // Reset to default pick
            playerTurn = 1;
            matchesHistory[turnCount] = matchesLeft;
            turnCount++;
        }
        printf("\n");
        // Check if the game should end
        if (matchesLeft <= 0){
            gameOn = 0;
        }
    }
    printf("The game has ended, %s has won!\n\n", playerTurn ? "Player" : "Computer");

    printf("Matches history:\n");
    for (int i = 0; i < turnCount; i++) {
        printf("Turn %d: %d matches left\n", i + 1, matchesHistory[i]);
    }

    free(matchesHistory);

    writeStringAndWait(playerTurn ? "WON " : "LOST", 1000);
    clearDisplay();

    return 0;
}