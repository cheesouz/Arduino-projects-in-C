#include <usart.h>
#include <stdio.h>
#include <string.h>

#define MAX 7
void printFirstLetters(char arr[MAX][10]){
    printf("First: ");
    for (int i = 0; i < 7; i++){
        printf("%c", arr[i][0]);
    }
    printf("\n");
}
void printLastLetters(char arr[MAX][10]){
    printf("Last: ");
    for (int i = 0; i < 7; i++){
        int last = strlen(arr[i])-1;
        printf("%c", arr[i][last]);
    }
    printf("\n");
}
void searchShortestName(char arr[MAX][10]){
    int shortestIdx = 0;
    int minCount = 10;
    for (int i = 0; i < MAX; i++){
        if (strlen(arr[i]) < minCount){
            shortestIdx = i;
            minCount = strlen(arr[i]);
        }
    }
    printf("Shortest = %s", arr[shortestIdx]);

}

int main() {
    initUSART();
    char names[MAX][10] = {"Doc", "Grumpy", "Happy", "Sleepy", "Bashful", "Sneezy", "Dopey"};
    printFirstLetters(names);
    printLastLetters(names);
    searchShortestName(names);

return 0;
}