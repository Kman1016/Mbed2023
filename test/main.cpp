#include "mbed.h"
#include <cstdio>

void add(int a[5]){
    int i = 0;
    for (i = 0; i < 5; i++) {
        a[i] = a[i] + 2;
    }
}
// main() runs in its own thread in the OS
int main()
{
    int a[5] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");
    add(a);
    for (int i = 0; i < 5; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");
}

