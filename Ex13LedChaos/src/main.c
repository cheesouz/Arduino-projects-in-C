#define __DELAY_BACKWARD_COMPATIBLE__  
#include <Arduino.h>
#include <util/delay.h> 
#include <led.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
  srand(0);
  enableAllLeds();
  lightDownAllLeds();

while (1)
{
  lightToggleOneLed(rand()%4);
  _delay_ms(rand() % 1000);
}


  return 0;
}
