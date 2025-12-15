#include <stdio.h>

int main() {
    int i;
    int sum = 0;

    for (i = 1; i <= 5; i++) {
        sum = sum + i;
        printf("i = %d, sum = %d\n", i, sum);
    }
    return 0;
}
