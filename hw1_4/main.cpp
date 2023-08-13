#include "mbed.h"
const double pi = 3.141592653589793238462;
const double amplitude = 2.0f;
const double offset = 65535 /2;
// main() runs in its own thread in the OS
AnalogOut aout(PA_5);
int main()
{
    double rads = 0.0;
    uint16_t sample[360];
    for (int i = 0; i < 360; i++) {
        
        rads = (pi * i) / 180.0f;
        if (rads < pi)
            sample[i] = 65535.0f * (tanh(rads));
        else
            sample[i] = 65535.0f * (tanh(2*pi-rads));;
    }
    while (true) {
        for (int i = 0; i < 360; i = i + 5) {
            aout.write_u16(sample[i]);
            //wait_us(1);
            aout.write_u16(sample[i+1]);
            //wait_us(1);
            aout.write_u16(sample[i+2]);
            aout.write_u16(sample[i+3]);
            aout.write_u16(sample[i+4]);
        }
    }
}

