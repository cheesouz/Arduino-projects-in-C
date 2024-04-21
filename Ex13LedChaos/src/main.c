#include <Arduino.h>
#include <util/delay.h> 
#include <led.h>
#include <stdlib.h>
#define __DELAY_BACKWARD_COMPATIBLE__  

int main(int argc, char const *argv[])
{
  srand(0);
  enableAllLeds();
  lightDownAllLeds();

while (1)
{
  lightUpOneLed(rand())
}


  return 0;
}
