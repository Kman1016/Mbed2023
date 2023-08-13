#include "mbed.h"
#include "bbcar.h"


#include "drivers/DigitalOut.h"

#include "erpc_simple_server.hpp"
#include "erpc_basic_codec.hpp"
#include "erpc_crc16.hpp"
#include "UARTTransport.h"
#include "DynamicMessageBufferFactory.h"
#include "bbcar_control_server.h"

#include "TextLCD.h"
#include <cstdio>
#include <memory>

#define SAMPLE 128
#define DIST2PATTERN 8
#define FIRST_ROTATION 70
#define SECONF_ROTATION -110
#define NUMBERofCONTENT 4

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
ep::UARTTransport uart_transport(D1, D0, 9600);
ep::DynamicMessageBufferFactory dynamic_mbf;
erpc::BasicCodecFactory basic_cf;
erpc::Crc16 crc16;
erpc::SimpleServer rpc_server;

Ticker servo_ticker;
Ticker servo_feedback_ticker;

PwmIn servo0_f(D9), servo1_f(D10);
PwmOut servo0_c(D11), servo1_c(D12);
BBCar car(servo0_c, servo0_f, servo1_c, servo1_f, servo_ticker, servo_feedback_ticker);

BusInOut qti_pin(D4,D5,D6,D7);
DigitalInOut pin8(D8);

parallax_qti qti1(qti_pin);
parallax_laserping  ping1(pin8);

BBCarService_service remote_control_service;

Thread t_LCDShow, t_prepare, t_erpc, t_sampling;

//BufferedSerial serdev(D1, D0, 9600);


bool ControlMode = 0, nControlMode = 0;
int npattern, pattern, QTiValue;
double lastspeed = 40, speed = 20, MaxSpeed = 45, minSpeed = -30;
float LaserPingValue;
int angles0[2] = {0};
int angles1[2] = {0};
int content[NUMBERofCONTENT] = {0};
int ncontent[NUMBERofCONTENT] = {0};
int obstacle = 0;




void eRPCmain();
void stop(uint8_t cars);
void goStraight(uint8_t cars, int32_t  speed);
void turn(uint8_t cars, int32_t speed, double factor);
void RemoteControlAction(int mode, int value);
void RemoteTuneSpeed(double max, double min);
void RemoteShowSpeed();
void RemoteShowPattern();
void ctl_mode();

void sampling();

int DeterDist(int pat);
int check(int errorDistance_Range);
void spin(float distance);
void DeterSlitPattern(float distance, float original_rot, int original_ang, int angles[SAMPLE], float dist[SAMPLE]);
int  SlitPattern();
void Prepare();
void AutoMode();
void LCDshowing();


int main() {

    t_LCDShow.start(LCDshowing);
    t_sampling.start(sampling);
    //t_prepare.start(Prepare);
    t_erpc.start(AutoMode);
    
    // Initialize the rpc server
    uart_transport.setCrc16(&crc16);

  printf("Initializing server.\n");
  rpc_server.setTransport(&uart_transport);
  rpc_server.setCodecFactory(&basic_cf);
  rpc_server.setMessageBufferFactory(&dynamic_mbf);

  // Add the led service to the server
  printf("Adding LCD server.\n");
  rpc_server.addService(&remote_control_service);

  // Run the server. This should never exit
  
  
    printf("Running server.\n");
    rpc_server.run();

    
}

//////////////////////////////// fuction ////////////////////////////////
void sampling() {
    while (1) {
        QTiValue = (int)qti1;
        LaserPingValue = ping1;
        ThisThread::sleep_for(1ms);
    }
}


void AutoMode() {
    int lastPattern = (int)qti1;;
    int pat;
    //int obstacle = 0;
    //car.goStraight(50);
    while (1) {
        pattern = (int)qti1;

        speed = pattern == lastPattern? lastspeed + 3: lastspeed -2;
        speed = speed > 45? 45 : speed;
        speed = speed < -20? -20: speed;
        // bright 0
        
        if  (nControlMode != ControlMode) {
            ControlMode = nControlMode;
            ThisThread::sleep_for(1s);
            car.stop();
            ThisThread::sleep_for(1s);
            //lcd.printf("Mode: %d\n", int(nControlMode));
        }
        if (!ControlMode) {
            printf("%f  %d\n",speed, pattern);    // debug
            switch (pattern) {
                // turn right
                case 0b0111: car.turn(speed, -0.001); break;
                case 0b0011: car.turn(speed, -0.3); break;
                case 0b0010: car.turn(speed, -0.05); break;
                case 0b0001: car.turn(speed, -0.01); break;

                case 0b0110: car.goStraight(speed); break;
                // turn left
                case 0b1000: car.turn(speed, 0.01); break;
                case 0b0100: car.turn(speed, 0.05); break;
                case 0b1100: car.turn(speed, 0.3); break;
                case 0b1110: car.turn(speed, 0.001); break;
                case 0b0000: 
                        //lcd.cls();              //debug
                        //lcd.printf("0000\n");   //debug
                        car.goStraight(35);
                        
                        //car.goStraight(3);
                        //ThisThread::sleep_for(10ms);
                        //car.turn(40, 0.01);
                        //ThisThread::sleep_for(3s);
                        break;
                case 0b1011:    // right
                        {
                        car.stop();
                        //spin(10*3.14/6);
                        car.goStraight(35);
                        ThisThread::sleep_for(500ms);
                        car.turn(35, -0.001);
                        ThisThread::sleep_for(1500ms);
                        //car.turn(40, -0.1);
                        //ThisThread::sleep_for(500ms);
                        }
                        break;
                case 0b1101: // left
                        {
                        car.stop();
                        //spin(-10*3.14/6);
                        car.goStraight(35);
                        ThisThread::sleep_for(500ms);
                        car.turn(35, 0.001);
                        ThisThread::sleep_for(1500ms);
                        //car.turn(40, 0.1);
                        //ThisThread::sleep_for(500ms);
                        
                        }
                        break;
                // slit 
                case 0b1111: 
                    //car.stop();
                    //ThisThread::sleep_for(1000ms);
                    //lcd.cls();
                    //lcd.printf("Y\n");
                    car.goStraight(3);
                    ThisThread::sleep_for(10ms);
                    car.turn(45, -0.01);
                    ThisThread::sleep_for(2000ms);
                    break;
                case 0b1001: 
                    //car.stop();
                    //ThisThread::sleep_for(1000ms);
                    //lcd.cls();
                    //lcd.printf("Y\n");
                    car.goStraight(3);
                    ThisThread::sleep_for(10ms);
                    car.turn(45, 0.01);
                    ThisThread::sleep_for(2000ms);
                    break;
                default: car.goStraight(40);
            }
            if (LaserPingValue < DIST2PATTERN) {
                car.stop();
                ThisThread::sleep_for(500ms);
                obstacle = SlitPattern(); //default 1
                content[1] = 1;
                ThisThread::sleep_for(150ms);
                content[1] = 0;
                //lcd.cls();
                lcd.printf("obstacle num =\n %d", obstacle);
                
                if (obstacle < 2) { // left
                    ThisThread::sleep_for(500ms);
                    spin(10*3.14159*(FIRST_ROTATION)/360);
                    //ThisThread::sleep_for(1s);
                    car.goStraight(45);
                    ThisThread::sleep_for(3s);
                } else {                // right
                    ThisThread::sleep_for(500ms);
                    spin(10*3.14159*(FIRST_ROTATION)/360);
                    ThisThread::sleep_for(1s);
                    spin(10*3.14159*(190)/360);
                    ThisThread::sleep_for(3s);
                }
            }
        }
        lastPattern = pattern;
        ThisThread::sleep_for(10ms);
    }    
}
 
int DeterDist(int pat) {

    int cnt = 0;
    /*
    int ang_temp0, ang_temp1;
    float dist1, dist0;
    //int pat = int(qti1);
    if (cnt < 2) {
        ang_temp0 = car.servo0.angle;
        ang_temp1 = car.servo1.angle;
        if (pat == 0b1001) {
            cnt = cnt + 1;
            //printf("%d: %d\n",cnt ,pat);    // debug
            lcd.printf("%d cnt: %d\n",pat, cnt);
            angles0[1] = angles0[0];
            angles0[0] = ang_temp0;
            angles1[1] = angles1[0];
            angles1[0] = ang_temp1;
            if ((angles0[1] != 0 && angles0[0] != 0) || (angles1[1] != 0 && angles1[0] != 0)) {
                dist0 = abs(angles0[0] - angles0[1])*6.5*3.14159/360;
                dist1 = abs(angles1[0] - angles1[1])*6.5*3.14159/360;
                if (dist0>dist1) {
                    //printf("%f\n",dist0); // debug
                    ThisThread::sleep_for(500s);
                    lcd.cls();
                    lcd.printf("dist: %d\n",(int)dist0);
                    //ThisThread::sleep_for(1s);
                    }
                else{
                    //printf("%f\n",dist1); // debug
                    ThisThread::sleep_for(500s);
                    lcd.cls();
                    lcd.printf("dist: %d\n",(int)dist0);
                    //ThisThread::sleep_for(1s);
                    
                }
            }
            ThisThread::sleep_for(2s);
        }
        //ThisThread::sleep_for(1ms);
    }*/ 
    if (cnt < 2) {
        return 1;
    }
        return 0;

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

void DeterSlitPattern(float distance, float original_rot, int original_ang, int angles[SAMPLE], float dist[SAMPLE]) {
    int i=SAMPLE;
    int len;    
    float d_temp;
    int ang_temp;
    float temp_rot;
    int ang_target = (int)(10*3.14159*(SECONF_ROTATION)/360*360/(6.5*3.14)) + original_ang;
    float ang_step = float(ang_target-original_ang)/SAMPLE;
    car.servo0.targetAngle = (int)(distance*360/(6.5*3.14)) + car.servo0.angle;
    car.servo1.targetAngle = (int)(distance*360/(6.5*3.14)) + car.servo1.angle;
    while (check(1)) {
        d_temp = ping1;
        ang_temp = car.servo0.angle;
        printf("%d: %f\n",i , d_temp);
        temp_rot = abs((ang_temp - original_ang)*6.5/10);
        len = (d_temp*cos((abs(original_rot - temp_rot)*6.5/10)*3.14159/180));
        if (int(len) <= (1 + DIST2PATTERN) && i >= 0) {                     // int(d_temp) < len // abs(int((original_ang + ang_step*(SAMPLE-i)) - ang_temp)) <= 2 && //  && abs(DIST2PATTERN - len) <= 2
            angles[i-1] = ang_temp;     
            dist[i-1] = d_temp;
        }
        ThisThread::sleep_for(6ms);
        i--;
    }
    car.stop();
    //ThisThread::sleep_for(1s);
    for (i = 0; i < SAMPLE; i++) {
        printf("%d angle: %d\n", i, angles[SAMPLE-1-i]);
    }
    for (i = 0; i < SAMPLE; i++) {
        printf("%d dist: %f\n", i, dist[SAMPLE-1-i]);
    }
}

int SlitPattern(void) {
    int angles[SAMPLE] = {0};
    float dist[SAMPLE] = {0};
    float original_rot;
    int original_ang;
    int original_turn;

    int i, cnt = 0;
    float avg_dist[2] = {0};
    int avg_ang[2] = {0};
    float temp_rot;
    float tan0, tan1;

    float deg = FIRST_ROTATION, rotate = SECONF_ROTATION;
    int ang_now, turns_now;

    int initial_ang = car.servo0.angle;
    int initial_turn = car.servo0.turns;
    printf("initial turns = %d\n", initial_turn);
    printf("initial angle = %d\n", initial_ang);
    spin(10*3.14159*(deg)/360);
    ThisThread::sleep_for(500ms);
    turns_now = car.servo0.turns;
    ang_now = car.servo0.angle+ 360*(turns_now-initial_turn);
    printf("now turns = %d\n", turns_now);
    printf("target distance = %f\n", (car.servo0.targetAngle)*6.5*3.14/360);
    printf("actual distance = %f\n", (ang_now-initial_ang)*6.5*3.14/360);
    printf("error distance = %f\n", (car.servo0.targetAngle - ang_now)*6.5*3.14/360);
    original_rot = (((ang_now-initial_ang)*6.5*3.14/360)/(10*3.14))*360;
    printf("actual rotate = %f\n", original_rot);
    ThisThread::sleep_for(100ms);

    initial_ang = car.servo0.angle;
    initial_turn = car.servo0.turns;
    original_turn = initial_turn; 
    original_ang = initial_ang;
    ThisThread::sleep_for(100ms);

    DeterSlitPattern(10*3.14159*(rotate)/360, original_rot, original_ang, angles, dist);
    ThisThread::sleep_for(500ms);
    int num = 0;
    for (i = 0; i < SAMPLE; i++) {
        
        if (!int(avg_dist[1])) {
            cnt = 0;
            while (dist[SAMPLE - 1 - i]) {
                cnt = cnt + 1;
                avg_dist[1] = avg_dist[1] + dist[SAMPLE - 1 - i];
                avg_ang[1] = avg_ang[1] + angles[SAMPLE - 1 - i];
                i++;
            }
            avg_dist[1] = cnt > 0 ? avg_dist[1]/cnt : 0;
            avg_ang[1] = cnt > 0 ? avg_ang[1]/cnt : 0;
        } else if (!int(avg_dist[0])) {
            cnt = 0;
            while (dist[SAMPLE - 1 - i]) {
                cnt = cnt + 1;
                avg_dist[0] = avg_dist[0] + dist[SAMPLE - 1 - i];
                avg_ang[0] = avg_ang[0] + angles[SAMPLE - 1 - i];
                i++;
            }
            avg_dist[0] = cnt > 0 ? avg_dist[0]/cnt : 0;
            avg_ang[0] = cnt > 0 ? avg_ang[0]/cnt : 0;
        }
        if (int(avg_dist[0]) && int(avg_dist[1])) {
            temp_rot = abs((avg_ang[1] - original_ang)*6.5/10);
            tan1 = avg_dist[1]*sin((abs(original_rot - temp_rot)*6.5/10)*3.14159/180);

            temp_rot = abs((avg_ang[0] - original_ang)*6.5/10);
            tan0 = avg_dist[0]*sin((abs(original_rot - temp_rot)*6.5/10)*3.14159/180);
            num = num + 1;
            printf("%d Difference = %f\n",num, abs(tan0-tan1));

            avg_ang[1] = avg_ang[0];
            avg_dist[1] = avg_dist[0];

            avg_ang[0] = 0;
            avg_dist[0] = 0;
        }
        cnt = 0;
    }
    return num;
}

void LCDshowing() {
    int i = 0;/*
    while(1) {
        for (i = 0; i < NUMBERofCONTENT; i++) {
            //content[i] = ncontent[i];
            switch (i) {
            case 0:
                if (content[0]) {
                    lcd.printf("Mode: %d\n", int(nControlMode));
                    //content[0] = 0;
                }
                break;
            case 1: 
                if (content[1]) {
                    lcd.printf("obstacle num =\n %d", obstacle + 1);
                    //content[1] = 0;
                }
                break;
            case 2:
                if (content[2]) {
                    lcd.printf("pattern: %d\n", int(qti1));
                    //content[2] = 0;
                }
                break;
            case 3:
                if (content[3]) {
                    lcd.printf("speed: %d\n", int(speed));
                    //content[3] = 0;
                }
                break;
            }
            ThisThread::sleep_for(40ms);
            //ncontent[i] = content[i];
        }
    }*/
}

void ctl_mode() {
    if (!nControlMode)
        nControlMode = 1;
    else
        nControlMode = 0;
    content[0] = 1;
    ThisThread::sleep_for(150ms);
    content[0] = 0;

    
    //printf("Mode: %d\n", nControlMode);
    lcd.printf("Mode: %d\n", int(nControlMode));
    //ThisThread::sleep_for(1s);
    //lcd.cls();
}

void RemoteShowPattern() {
    //lcd.cls();
    //printf("pattern: %d\n",(int)qti1);
    lcd.printf("pattern: %d\n", int(qti1));
    content[2] = 1;
    ThisThread::sleep_for(150ms);
    content[2] = 0;
    
}

void RemoteShowSpeed() {
    //printf("speed: %f\n",speed);
    lcd.printf("speed: %d\n", int(speed));
    ncontent[3] = 1;
    ThisThread::sleep_for(150ms);
    //lcd.cls();
    ncontent[3] = 0;
}

void RemoteTuneSpeed(double max, double min) {
    MaxSpeed = max;
    minSpeed = min;
}

void RemoteControlAction(uint8_t mode, int32_t value, double factor) {
    ControlMode = 1;
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
    if (ControlMode) {
        car.stop();
        printf("Car %d stop.\n", cars);
    }
    
}

void goStraight(uint8_t cars, int32_t  speed){
    if (ControlMode) {
        car.goStraight(speed);
        printf("Car %d go straight at speed %d.\n", cars, speed);
    }
}

void turn(uint8_t cars, int32_t speed, double factor){
    if (ControlMode) {
        car.turn(speed, factor);
        printf("Car %d turn at speed %d with a factor of %f.\n", cars, speed, factor);
    }
}