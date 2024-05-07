#include <usart.h>
#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>
#include <display.h>
#include <stdlib.h>
#include <button.h>

//function from canvas for converting input from the potentiometer into a number
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
void displayNumberOfMatches(){

}
void displayPlayersWishes(){}
void displayPC{
  
}

int main(){
initDisplay();
initADC();
initUSART();
printf("Turn the potentiometer to generate a seed.\n");
_delay_ms(500);
int seed = ADC;
srand(seed);
enableOneButton(1);
while (1)
{
writeNumberAndWait(seed, 1000);
int gameOn = buttonPushed(1); // C doesn't hava a built-in bool data type so i'm using this varible as a flag

while (gameOn)
{
  printf("test\n");
  if (buttonReleased(1)==0){gameOn = 0;}
}

}
}