#include <stdio.h>

int main() {
    // Dead Code (this variable is declared but never used)
    int deadVariable = 100;

    // Constant Folding and Instruction Simplification
    int a = 10;
    int b = a + 0; // Simplify to: int b = a;
    int c = b * 1; // Simplify to: int c = b;
    int d = 5 * 4; // Constant Fold to: int d = 20;

    // More Dead Code (the computation is done but the result isn't used)
    int e = d + c;

    // The real computation
    int result = a + d;

    printf("The result is: %d\n", result);
    return 0;
}
