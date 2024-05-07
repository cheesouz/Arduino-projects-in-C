#include <led.h>
#include <button.h>
#include <util/delay.h>
#include <avr/io.h> 
#include <display.h>

int main()
{
    initDisplay();
    writeStringAndWait("sisa", 1000);
    writeStringAndWait("go  ", 1000);
    writeStringAndWait("wash", 1000);
    writeStringAndWait("ur  ", 1000);
    writeStringAndWait("ass ", 1000);
   
}