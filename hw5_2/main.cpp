#include "mbed.h"
#include "bbcar.h"
#include <cstdio>

Ticker servo_ticker;
Ticker servo_feedback_ticker;

PwmIn servo0_f(D9), servo1_f(D10);
PwmOut servo0_c(D11), servo1_c(D12);
BBCar car(servo0_c, servo0_f, servo1_c, servo1_f, servo_ticker, servo_feedback_ticker);

BusInOut qti_pin(D4,D5,D6,D7);
parallax_qti qti1(qti_pin);
Thread t1, t2;

int npattern, pattern;
double nspeed, speed = 20;

void DeterDist() {
    int angles0[2] = {0};
    int angles1[2] = {0};
    int cnt = 0;
    int ang_temp0, ang_temp1;
    float dist1, dist0;
    while (1) {
        ang_temp0 = car.servo0.angle;
        ang_temp1 = car.servo1.angle;
        if (pattern == 0b1111) {
            printf("%d: %d\n",cnt++ ,pattern);
            angles0[1] = angles0[0];
            angles0[0] = ang_temp0;
            angles1[1] = angles1[0];
            angles1[0] = ang_temp1;
            if ((angles0[1] != 0 && angles0[0] != 0) || (angles1[1] != 0 && angles1[0] != 0)) {
                dist0 = abs(angles0[0] - angles0[1])*6.5*3.14159/360;
                dist1 = abs(angles1[0] - angles1[1])*6.5*3.14159/360;
                if (dist0>dist1) 
                    printf("%f\n",dist0);
                else
                    printf("%f\n",dist1);
            }
            ThisThread::sleep_for(1s);
        }
        ThisThread::sleep_for(1ms);
    }
}

void Prepare() {
    int delta = 0;
    while (1) {
        npattern = (int)qti1;
        switch (npattern) {
            // turn right
            case 0b0111: delta = npattern == pattern? delta + 1 : delta - 1; break;
            case 0b0011: delta = npattern == pattern? delta + 2 : delta - 1; break;
            case 0b0001: delta = npattern == pattern? delta + 3 : delta - 3; break;
            case 0b0110: delta = npattern == pattern? delta + 1 : delta - 3; break;
            // turn left
            case 0b1000: delta = npattern == pattern? delta + 3 : delta - 3; break;
            case 0b1100: delta = npattern == pattern? delta + 2 : delta - 1; break;
            case 0b1110: delta = npattern == pattern? delta + 1 : delta - 1; break;
            case 0b0000: delta = npattern == pattern? -10 : 0; break;
            default: delta = 0;
        }
        delta = (delta) > 10? 10 : delta;
        delta = (delta) < -10? -10 : delta;
        nspeed = speed + delta;
        nspeed = nspeed > 40? 40 : nspeed;
        nspeed = nspeed < -30? -30 : nspeed;
        delta = delta < 0? 0 : delta-1;
        ThisThread::sleep_for(1ms);
        //printf("%f\n",nspeed);
    }
}

int main() {

    t1.start(DeterDist);
    
    
    int angles[2] = {0};
    car.goStraight(50);
    t2.start(Prepare);
    while(1) {

        //pattern = (int)qti1;
        //if (pattern == 0)
            //printf("%d\n",pattern);

        /*
        // bright 0
        switch (pattern) {
            // turn right
            case 0b1000: car.turn(55, -0.04); break;
            case 0b1100: car.turn(55, -0.03); break;
            case 0b1110: car.turn(55, -0.01); break;
            case 0b0110: car.goStraight(55); break;
            // turn left
            case 0b0111: car.turn(55, 0.01); break;
            case 0b0011: car.turn(55, 0.03); break;
            case 0b0001: car.turn(55, 0.04); break;
            case 0b1111: car.stop(); break;
            default: car.goStraight(50);
        }
        */


        pattern = npattern;
        speed = nspeed;
        // bright 0

        switch (pattern) {
            // turn right
            case 0b0111: car.turn(speed, -0.1); break;
            case 0b0011: car.turn(speed, -0.01); break;
            case 0b0010: car.turn(speed, -0.05); break;
            case 0b0001: car.turn(speed, -0.001); break;
            case 0b0110: car.goStraight(speed); break;
            // turn left
            case 0b1000: car.turn(speed, 0.001); break;
            case 0b0100: car.turn(speed, 0.05); break;
            case 0b1100: car.turn(speed, 0.01); break;
            case 0b1110: car.turn(speed, 0.1); break;
            case 0b0000: car.goStraight(speed); ThisThread::sleep_for(15ms); break;
            default: car.goStraight(30);
        }
        ThisThread::sleep_for(3ms);
       
    }
}