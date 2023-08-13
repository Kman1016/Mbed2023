#include "mbed.h"
const double pi = 3.141592653589793238462;
const double amplitude = 1.0f;
const double offset = 1000;
// main() runs in its own thread in the OS
AnalogOut aout(PA_5);
int main()
{
    double rads = 0.0;
    uint16_t sample = 0;
    while (true) {
        for (int i = 0; i < 360; i++) {
            rads = (pi * i) / 180.0f;
            sample = (uint16_t)(amplitude * (offset * (cos(rads + pi))) + offset);
            aout.write_u16(sample);
        }
    }
}

