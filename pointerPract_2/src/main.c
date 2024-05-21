

#define ON_BOARD 1

#ifdef ON_BOARD
#include <usart.h>
#else
#include <stdio.h>
#endif

char arr[] = { 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!', '\0' } ;

void arrchar1(char *arr, int pos)
{
  printf("[%d]: %c\n", pos, arr[pos] );
}

void arrchar2(char *arr, int pos)
{
  printf("[%d]: %c\n", pos, *(arr + pos));   // complete this line
}

int main()
{
#ifdef ON_BOARD
  initUSART();
#endif

  printf("size of arr: %d\n", (int) sizeof(arr));
  arrchar1(arr, 6);
  arrchar2(arr, 6);
  return 0;
}

