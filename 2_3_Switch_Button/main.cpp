#include "mbed.h"
DigitalIn mypin(BUTTON1);
DigitalOut myled(LED1);
// main() runs in its own thread in the OS
int main()
{
    if (mypin.is_connected()) {
        printf("mypin is connected and initialized! \n\r");
    }
    mypin.mode(PullNone);
    while (true) {
        printf("mypin has value : %d \n\r", mypin.read());
        myled = mypin; // toggle led based on value of button
        ThisThread::sleep_for(250ms);
    }
}

