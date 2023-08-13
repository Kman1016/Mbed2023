#include "mbed.h"

AnalogOut Aout(PA_4);
// main() runs in its own thread in the OS
int main()
{
    while (true) {
        Aout = 0.25;
        ThisThread::sleep_for(2s);
        Aout = 0.5;
        ThisThread::sleep_for(2s);
        Aout = 0.75;
        ThisThread::sleep_for(2s);
    }
}

