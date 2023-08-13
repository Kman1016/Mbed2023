#include "mbed.h"
#include <cmath>

DigitalIn mybuttom(BUTTON1);
DigitalOut myLED2(LED2);
DigitalOut myLED3(LED3);
DigitalOut myLED1(LED1);
BusOut display(D6, D7, D9, D10, D11, D5, D4, D8);
char table[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
// main() runs in its own thread in the OS
int main()
{
    int Bcode[5];
    int dec = 0;
    bool mode = 1; // 1: letter; 0: figure
    myLED3 = 0;
    myLED2 = 0;
    while (true) {
        
        for(int i = 0; i < 3; i++) {
            printf("%d\n", 3-i);
            ThisThread::sleep_for(1s);
        }

        printf("Start\n");
        ThisThread::sleep_for(1s);

        for (int i = 0; i < 5; i++) {
            printf("Code%d:\n", i);
            ThisThread::sleep_for(1s);
            Bcode[i] = !mybuttom;
            if (Bcode[i]) {
                myLED3 = 1;
                ThisThread::sleep_for(200ms);
                myLED3 = 0;
            } else {
                myLED2 = 1;
                ThisThread::sleep_for(200ms);
                myLED2 = 0;
            }
            printf("%d\n", Bcode[i]);
            ThisThread::sleep_for(200ms);
            printf("OK\n");
            ThisThread::sleep_for(200ms);
        }
        printf("Your code: ");
        for (int i = 0; i < 5; i++) {
            printf("%d", Bcode[i]);
        }
        printf("\n");
        ThisThread::sleep_for(1s);

        for (int i = 0; i < 5; i++) {
            dec += Bcode[i] * pow(2, 4-i);
        }

        if (mode == 0 && dec == 16)
            mode = 1;
        else if (mode == 1 && dec == 8)
            mode = 0;
        else
            mode = mode;

        if (mode)
            switch (dec) {
                case 1: display = 0b01011111; break; //a
                case 2: display = 0b01111001; break; //e
                case 4: display = 0b01101110; break; //y
                case 5: display = 0b00011100; break; //u
                case 6: display = 0b00010001; break; //i
                case 7: display = 0b01011100; break; //o
                case 9: display = 0b00001101; break; //j
                case 10: display = 0b00111101; break; //g
                case 11: display = 0b01110100; break; //h
                case 12: display = 0b01111100; break; //b
                case 13: display = 0b01011000; break; //c
                case 14: display = 0b01110001; break; //f
                case 15: display = 0b01011110; break; //d
                case 18: display = 0b00010100; break; //x
                case 19: display = 0b00011011; break; //z
                case 20: display = 0b00101101; break; //s
                case 21: display = 0b01111000; break; //t
                case 22: display = 0b01101010; break; //w
                case 23: display = 0b00101010; break; //v
                case 25: display = 0b01110101; break; //k
                case 26: display = 0b01010101; break; //m
                case 27: display = 0b00111000; break; //l
                case 28: display = 0b01010000; break; //r
                case 29: display = 0b01100111; break; //q
                case 30: display = 0b01110011; break; //p
                default: display = 0b00000000;
            }
        else
            switch (dec) {
                case 1: display = table[1]; break;
                case 2: display = table[2]; break;
                case 4: display = table[3]; break;
                case 5: display = table[4]; break;
                case 7: display = table[5]; break;
                case 9: display = table[6]; break;
                case 10: display = table[7]; break;
                case 12: display = table[8]; break;
                case 13: display = table[9]; break;
                case 15: display = table[0]; break;
                default: display = 0b00000000;
            }
        ThisThread::sleep_for(1s);
        dec = 0;
    }
}

