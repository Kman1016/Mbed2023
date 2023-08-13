#include "mbed.h"

// main() runs in its own thread in the OS

AnalogOut Aout(D7);
AnalogIn Ain(A0);

int main()
{
    while (true) {
        Aout = Ain;
    }
}

