#include <stdio.h>

int main() {
    int a = 0;
    int b = 1;

    if (a == b) {
        printf("Integer a equals to b branch!\n");
    }
    else {
        printf("Integer a not equals to b branch!\n");
    }

    float c = 3.14;
    float d = 2.71;
    
    if (c == d) {
        printf("Float c equals to d branch!\n");
    }
    else {
        printf("Float c not equals to d branch!\n");
    }

    return 0;
}
