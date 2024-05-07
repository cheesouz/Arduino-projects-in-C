#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>
#include <display.h>

int main(){
    int num = 1234;
    initDisplay();
    printf("Aaaaaa");

while (1)
{
    num = (num % 1000) * 10 + (num % 10 + 1)%10;
    printf("%d \n",num);
    writeNumberAndWait(num, 100);
}
printf("Aaaaaa");
}
