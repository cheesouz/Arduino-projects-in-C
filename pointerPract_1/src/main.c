#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>
#include <display.h>
#include <usart.h>

#define MAX 5
#define SEVENFOLD(x) (x*7)

void printArray(int arr[]){
    printf("Contents of the array:\n");
    for (int i = 0; i < MAX; i++)
    {
        printf("Address: %p has value: %d\n", &arr[i], arr[i]);
    }
}

int *makeArray(){
    int *arr = malloc(MAX);
    for (int i =0; i <MAX; i++){
        arr[i] = SEVENFOLD(i);
    }
    return arr;
}

int main(){
    initUSART();
    int array[MAX];
    for (int i = 0; i < MAX; i++)
    {
        array[i] = 0;
    }
    
    printArray(array);
    int another = makeArray();
    printArray(another);
return 0;
}
