#include "mbed.h"
#include "bbcar.h"

Ticker servo_ticker;
Ticker servo_feedback_ticker;

PwmIn servo0_f(D9), servo1_f(D10);
PwmOut servo0_c(D11), servo1_c(D12);
BBCar car(servo0_c, servo0_f, servo1_c, servo1_f, servo_ticker, servo_feedback_ticker);

BusInOut qti_pin(D4,D5,D6,D7);
Thread t1;

int pattern;

void DeterDist() {
    int angles0[2] = {0};
    int angles1[2] = {0};
    int cnt = 0;
    int ang_temp0, ang_temp1;
    float dist1, dist0;
    while (1) {
        ang_temp0 = car.servo0.angle;
        ang_temp1 = car.servo1.angle;
        if (pattern == 0) {
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
            ThisThread::sleep_for(250ms);
        }
        ThisThread::sleep_for(1ms);
    }
}

int main() {

    t1.start(DeterDist);
    parallax_qti qti1(qti_pin);
    
    int angles[2] = {0};
    car.goStraight(50);
    while(1) {

        pattern = (int)qti1;
        //if (pattern == 0)
        //    printf("%d\n",pattern);

        
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
        ThisThread::sleep_for(1ms);
    }
}
