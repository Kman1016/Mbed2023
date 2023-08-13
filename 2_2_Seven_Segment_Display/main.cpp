#include "mbed.h"

BusOut display(D6, D7, D9, D10, D11, D5, D4, D8);
char table[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
// main() runs in its own thread in the OS
int main()
{
    while (true) {
        for (int i = 0; i < 10; i++) {
            display = table[i];
            ThisThread::sleep_for(1s);
        }
    }
}

