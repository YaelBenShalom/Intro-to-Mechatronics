#include <stdio.h>
#include <string.h>
#define MAXLENGTH 100           // max length of string input

void getString(char *str);      // helper prototypes
void printResult(char *str);
int greaterThan(char ch1, char ch2);
void swap(char *str, int index1, int index2);

int main(void) {
    int len;                    // length of the entered string
    char str[MAXLENGTH];        // input should be no longer than MAXLENGTH
    int i = 0;
    int swap_count = 1;
    
    getString(str);
    len = strlen(str);          // get length of the string, from string.h

    while (swap_count != 0) {   // nested loops to put the string in sorted order
        swap_count = 0;
        for(i = 0; i<len-1; i++) {
            if (greaterThan(str[i], str[i+1])) {
                swap(str, i, i+1);
                swap_count += 1;
                // printf("%s\n", str);
            }
        }
    }

    printResult(str);
    return(0);
}

// helper functions

void getString(char *str) {
    printf("Enter the string you would like to sort (up to %d character):\t", MAXLENGTH);
    scanf("%s", str);
} 


void printResult(char *str) {
    printf("Here is the sorted string:\t%s\n", str);
}


int greaterThan(char ch1, char ch2) {
    if(ch1 > ch2) {
        return 1;
    }
    return 0;
}


void swap(char *str, int index1, int index2) {
    char str_index1 = str[index1];
    str[index1] = str[index2];
    str[index2] = str_index1;
}