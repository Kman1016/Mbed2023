#include "mbed.h"
#include "bbcar.h"
#include "TextLCD.h"

Ticker servo_ticker;
Ticker servo_feedback_ticker;
Thread t_DeterDist, t_prepare;

PwmIn servo0_f(D9), servo1_f(D10);
PwmOut servo0_c(D11), servo1_c(D12);
BBCar car(servo0_c, servo0_f, servo1_c, servo1_f, servo_ticker, servo_feedback_ticker);

BusInOut qti_pin(D4,D5,D6,D7);
parallax_qti qti1(qti_pin);
I2C i2c_lcd(D14, D15);// SDA, SCL
//TextLCD_SPI lcd(&spi_lcd, p8, TextLCD::LCD40x4);   // SPI bus, 74595 expander, CS pin, LCD Type
TextLCD_I2C lcd(&i2c_lcd, 0x4E, TextLCD::LCD16x2);   // I2C bus, PCF8574 Slaveaddress, LCD Type
                                                     //TextLCD_I2C lcd(&i2c_lcd, 0x42, TextLCD::LCD16x2, TextLCD::WS0010);
                                                     // I2C bus, PCF8574 Slaveaddress, LCD Type, Device Type
                                                     //TextLCD_SPI_N lcd(&spi_lcd, p8, p9);
                                                     // SPI bus, CS pin, RS pin, LCDType=LCD16x2, BL=NC, LCDTCtrl=ST7032_3V3
//TextLCD_I2C_N lcd(&i2c_lcd, ST7032_SA, TextLCD::LCD16x2, NC, TextLCD::ST7032_3V3);
// I2C bus, Slaveaddress, LCD Type, BL=NC, LCDTCtrl=ST7032_3V3
// main() runs in its own thread in the OS


// global variable
int npattern, pattern;
double nspeed, speed = 20, MaxSpeed = 40, minSpeed = -30;
// function
void DeterDist();
int check(int errorDistance_Range);
void spin(float distance);
void Prepare();

void eRPCmain();
void stop(uint8_t cars);
void goStraight(uint8_t cars, int32_t  speed);
void turn(uint8_t cars, int32_t speed, double factor);
void RemoteControlAction(int mode, int value);
void RemoteTuneSpeed(double max, double min);
void RemoteShowSpeed();
void RemoteShowPattern();


// main() runs in its own thread in the OS
int main()
{
    t_DeterDist.start(DeterDist);
    t_prepare.start(Prepare);
    car.goStraight(50);
    while (1) {
        pattern = npattern;
        speed = nspeed;
        // bright 0
        //printf("%f  %d\n",speed, pattern);
        
            switch (pattern) {
                // turn right
                case 0b0111: car.turn(speed, -0.001); break;
                case 0b0011: car.turn(speed, -0.08); break;
                case 0b0010: car.turn(speed, -0.05); break;
                case 0b0001: car.turn(speed, -0.05); break;

                case 0b0110: car.goStraight(speed); break;
                // turn left
                case 0b1000: car.turn(speed, 0.05); break;
                case 0b0100: car.turn(speed, 0.05); break;
                case 0b1100: car.turn(speed, 0.08); break;
                case 0b1110: car.turn(speed, 0.001); break;
                case 0b0000: car.goCertainDistance(3); ThisThread::sleep_for(100ms);break;

                case 0b1101:    
                        car.goCertainDistance(3); ThisThread::sleep_for(500ms);
                        while((int)qti1 != 0b0110) {
                            car.turn(speed, -0.08);
                            ThisThread::sleep_for(100ms);
                        }
                        break;
                case 0b1011: 
                        car.goCertainDistance(3); ThisThread::sleep_for(500ms);
                        while((int)qti1 != 0b0110) {
                            car.turn(speed, 0.08);
                            ThisThread::sleep_for(100ms);
                        }
                        break;
                //case 0b1101:
                //case 0b1011:
                // slit 
                case 0b1001: 
                    car.stop();


                default: car.goStraight(40);
            }
        ThisThread::sleep_for(3ms);
    }
}

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
            case 0b0011: delta = npattern == pattern? delta + 5 : delta - 3; break;
            case 0b0010: delta = npattern == pattern? delta + 3 : delta - 3; break;
            case 0b0001: delta = npattern == pattern? delta + 5 : delta - 3; break;

            case 0b0110: delta = npattern == pattern? delta + 1 : delta - 3; break;
            // turn left
            case 0b1000: delta = npattern == pattern? delta + 5 : delta - 3; break;
            case 0b0100: delta = npattern == pattern? delta + 3 : delta - 3; break;
            case 0b1100: delta = npattern == pattern? delta + 5 : delta - 3; break;
            case 0b1110: delta = npattern == pattern? delta + 1 : delta - 1; break;

            case 0b0000: delta = npattern == pattern? -10 : 0; break;
            default: delta = 1;
        }
        delta = (delta) > 10? 10 : delta;
        delta = (delta) < -10? -10 : delta;
        nspeed = speed + delta;
        nspeed = nspeed > MaxSpeed? MaxSpeed : nspeed;
        nspeed = nspeed < minSpeed? minSpeed : nspeed;
        delta = delta < 0? 0 : delta-1;
        ThisThread::sleep_for(1ms);
        //printf("%f\n",nspeed);
    }
}

void spin(float distance) {
    car.servo0.targetAngle = (int)(distance*360/(6.5*3.14)) + car.servo0.angle;
    car.servo1.targetAngle = (int)(distance*360/(6.5*3.14)) + car.servo1.angle;
    while (check(1)) {
        ThisThread::sleep_for(1ms);
    }
    car.stop();
}

int check(int errorDistance_Range){
    int speed, offset;                                                              // Control system variables
    float errorDistance, factor=1;                 

    errorDistance = (car.servo0.targetAngle - car.servo0.angle)*6.5*3.14/360;       // Calculate error
    
    speed = int(errorDistance);

    if(errorDistance > 0)                        // Add offset
        offset = 40;
    else if(errorDistance < 0)
        offset = -40;
    else
        offset = 0;    

    car.servo0.set_factor(factor);
    car.servo1.set_factor(factor);
    car.servo0.set_speed(speed + offset);  
    car.servo1.set_speed(speed + offset);

    if ( abs(errorDistance) > (errorDistance_Range) )
        return 1;   
    else return 0;
}

void RemoteShowPattern() {
    printf("pattern: %d\n",pattern);
    lcd.printf("pattern: %d\n", int(pattern));
    ThisThread::sleep_for(1s);
    lcd.cls();
}

void RemoteShowSpeed() {
    printf("speed: %f\n",speed);
    lcd.printf("speed: %d\n", int(speed));
    ThisThread::sleep_for(1s);
    lcd.cls();
}

void RemoteTuneSpeed(double max, double min) {
    MaxSpeed = max;
    minSpeed = min;
}

void RemoteControlAction(uint8_t mode, int32_t value, double factor) {
    switch (mode) {
        case 1:
            car.goCertainDistance(value);
            break;
        case 2:
            spin(10*3.14159*value/360);
            break;
        case 3:
            car.turn(value, factor);
            break;
        case 4:
            car.goStraight(speed);
        case 5:
            car.stop();
    }
    ThisThread::sleep_for(100ms);
    
}
void stop(uint8_t cars){
    car.stop();
    printf("Car %d stop.\n", cars);
}

void goStraight(uint8_t cars, int32_t  speed){
    car.goStraight(speed);
    printf("Car %d go straight at speed %d.\n", cars, speed);

}

void turn(uint8_t cars, int32_t speed, double factor){
    car.turn(speed, factor);
    printf("Car %d turn at speed %d with a factor of %f.\n", cars, speed, factor);
}