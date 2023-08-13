#include "mbed.h"
#include "bbcar.h"

Ticker servo_ticker;
Ticker servo_feedback_ticker;

PwmIn servo0_f(D9), servo1_f(D10);
PwmOut servo0_c(D11), servo1_c(D12);
BBCar car(servo0_c, servo0_f, servo1_c, servo1_f, servo_ticker, servo_feedback_ticker);

BusInOut qti_pin(D4,D5,D6,D7);

int main() {

   parallax_qti qti1(qti_pin);
   int pattern;

   car.goStraight(50);
   while(1) {
      pattern = (int)qti1;
      printf("%d\n",pattern);
        // bright 0
      switch (pattern) {
            // turn right
         case 0b1000: car.turn(50, -0.05); break;
         case 0b1100: car.turn(50, -0.01); break;
         case 0b1110: car.turn(50, -0.001); break;
         case 0b0110: car.goStraight(50); break;
            // turn left
         case 0b0111: car.turn(50, 0.001); break;
         case 0b0011: car.turn(50, 0.01); break;
         case 0b0001: car.turn(50, 0.05); break;
         case 0b1111: car.stop(); break;
         default: car.goStraight(50);
      }
      ThisThread::sleep_for(1ms);
   }
}
