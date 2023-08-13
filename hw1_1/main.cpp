#include "mbed.h"
#include <vector>

DigitalOut myLED3(LED2);
// main() runs in its own thread in the OS
int main()
{
    char chr[4];
    bool flag = 1, nflag = 1; // 1: letter; 0: figure
    int figure2letter[5] = {1, 0, 0, 0, 0};
    int letter2figure[5] = {0, 1, 0, 0, 0};
    vector<int> Bcode;
    while (true) {
        printf("Enter: ");
        scanf("%s", chr);
        printf("\nmy enter: %s\n", chr);
        for (int i = 0; chr[i] != '\0'; i++) {
            
            if (flag == 1 && (48 <= int(chr[i]) && int(chr[i]) <= 57)) {
                nflag = 0;
            } else if (flag == 0 && (97 <= int(chr[i]) && int(chr[i]) <= 122)) {
                nflag = 1;
            } else {
                flag = nflag;
            }

            if (nflag) {
                switch (int(chr[i])) {
                    case 97: Bcode = {0, 0, 0, 0, 1}; break;    // a
                    case 98: Bcode = {0, 1, 1, 0, 0}; break;    // b
                    case 99: Bcode = {0, 1, 1, 0, 1}; break;    // c
                    case 100: Bcode = {0, 1, 1, 1, 1}; break;   // d
                    case 101: Bcode = {0, 0, 0, 1, 0}; break;   // e
                    case 102: Bcode = {0, 1, 1, 1, 0}; break;   // f
                    case 103: Bcode = {0, 1, 0, 1, 0}; break;   // g
                    case 104: Bcode = {0, 1, 0, 1, 1}; break;   // h
                    case 105: Bcode = {0, 0, 1, 1, 0}; break;   // i
                    case 106: Bcode = {0, 1, 0, 0, 1}; break;   // j
                    case 107: Bcode = {1, 1, 0, 0, 1}; break;   // k
                    case 108: Bcode = {1, 1, 0, 1, 1}; break;   // l
                    case 109: Bcode = {1, 1, 0, 1, 0}; break;   // m
                    case 110: Bcode = {1, 1, 1, 1, 0}; break;   // n
                    case 111: Bcode = {0, 0, 1, 1, 1}; break;   // o
                    case 112: Bcode = {1, 1, 1, 1, 1}; break;   // p
                    case 113: Bcode = {1, 1, 1, 0, 1}; break;   // q
                    case 114: Bcode = {1, 1, 1, 0, 0}; break;   // r
                    case 115: Bcode = {1, 0, 1, 0, 0}; break;   // s
                    case 116: Bcode = {1, 0, 1, 0, 1}; break;   // t
                    case 117: Bcode = {0, 0, 1, 0, 1}; break;   // u
                    case 118: Bcode = {1, 0, 1, 1, 1}; break;   // v
                    case 119: Bcode = {1, 0, 1, 1, 0}; break;   // w
                    case 120: Bcode = {1, 0, 0, 1, 0}; break;   // x
                    case 121: Bcode = {0, 0, 1, 0, 0}; break;   // y
                    case 122: Bcode = {0, 0, 0, 0, 1}; break;   // z
                }
            } else {
                switch (int(chr[i])) {
                    case 48: Bcode = {0, 1, 1, 1, 1}; break;
                    case 49: Bcode = {0, 0, 0, 0, 1}; break;
                    case 50: Bcode = {0, 0, 0, 1, 0}; break;
                    case 51: Bcode = {0, 0, 1, 0, 0}; break;
                    case 52: Bcode = {0, 0, 1, 0, 1}; break;
                    case 53: Bcode = {0, 0, 1, 1, 1}; break;
                    case 54: Bcode = {0, 1, 0, 0, 1}; break;
                    case 55: Bcode = {0, 1, 0, 1, 0}; break;
                    case 56: Bcode = {0, 1, 1, 0, 0}; break;
                    case 57: Bcode = {0, 1, 1, 0, 1}; break;
                }
            }

            if (flag == 1 && nflag == 0) {
                printf("01000 ");
                for (int k = 0; k < 5; k++) {
                    if (letter2figure[k]==1) {
                        myLED3 = 1;
                        ThisThread::sleep_for(1s);
                        myLED3 = 0;
                    } else {
                        myLED3 = 1;
                        ThisThread::sleep_for(200ms);
                        myLED3 = 0;
                    }
                    ThisThread::sleep_for(100ms);
                }
            } else if (flag == 0 && nflag == 1) {
                printf("10000 ");
                for (int k = 0; k < 5; k++) {    
                    if (letter2figure[k]==1) {
                        myLED3 = 1;
                        ThisThread::sleep_for(1s);
                        myLED3 = 0;
                    } else {
                        myLED3 = 1;
                        ThisThread::sleep_for(200ms);
                        myLED3 = 0;
                    }
                    ThisThread::sleep_for(100ms);
                }
            }

            for (int k = 0; k < 5; k++) {
                printf("%d", Bcode[k]);
            }
            ThisThread::sleep_for(200ms);
            
            for (int k = 0; k < 5; k++) {
                if (Bcode[k]==1) {
                    myLED3 = 1;
                    ThisThread::sleep_for(1s);
                    myLED3 = 0;
                } else {
                    myLED3 = 1;
                    ThisThread::sleep_for(200ms);
                    myLED3 = 0;
                }
                ThisThread::sleep_for(100ms);
            }
            printf(" ");
            flag = nflag;
        }
        printf("\n");
        flag = 1;
        nflag = 1;

    }
}

