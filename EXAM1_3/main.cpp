#include "mbed.h"
#include "TextLCD.h"
#include <cstdint>
#include <cstdio>

Thread thread_master;
Thread thread_slave;
I2C i2c_lcd(D14, D15); // SDA, SCL
//TextLCD_SPI lcd(&spi_lcd, p8, TextLCD::LCD40x4);   // SPI bus, 74595 expander, CS pin, LCD Type
TextLCD_I2C lcd(&i2c_lcd, 0x4E, TextLCD::LCD16x2);   // I2C bus, PCF8574 Slaveaddress, LCD Type
                                                     //TextLCD_I2C lcd(&i2c_lcd, 0x42, TextLCD::LCD16x2, TextLCD::WS0010);
                                                     // I2C bus, PCF8574 Slaveaddress, LCD Type, Device Type
                                                     //TextLCD_SPI_N lcd(&spi_lcd, p8, p9);
                                                     // SPI bus, CS pin, RS pin, LCDType=LCD16x2, BL=NC, LCDTCtrl=ST7032_3V3
//TextLCD_I2C_N lcd(&i2c_lcd, ST7032_SA, TextLCD::LCD16x2, NC, TextLCD::ST7032_3V3);
// I2C bus, Slaveaddress, LCD Type, BL=NC, LCDTCtrl=ST7032_3V3
// main() runs in its own thread in the OS
// master def.
SPI spi(D11, D12, D13);
DigitalOut cs(D9);

// slave def.
SPISlave device(PD_4, PD_3, PD_1, PD_0);

int slave()
{
   device.format(16, 3); // 1+8+16
   device.frequency(1000000);
   int cmd, buf;
   int x, y, num;
   //device.reply(0x00); // Prime SPI with first reply
    while (1)
    {
        if (device.receive())
        {
            cmd = device.read();
            switch (cmd) {
            case 1: {printf("Slave: Give me location.\n");break;}
            case 2: {printf("Slave: Clear.\n");break;}
            case 3: {printf("Slave: Give me a number\n");break;}
            default: printf("No cmd\n");
            }

            device.reply(cmd);
            buf = device.read();

            switch (cmd) {
                case 1: {
                        x = device.read();
                        y = device.read();
                        printf("Slave: ( %d, %d ).\n", x, y);
                        break;
                    }
                case 2: {printf("Slave: Show on lcd.\n");break;}
                case 3: {
                        num = device.read();
                        printf("Slave: %d.\n", num);
                        break;
                    }
            default: printf("No OP\n");
            }
        }
    }
}

void master()
{
   int x, y, num; // 1: read; 0: write
   int cmd;
   int response = 0;
   
   // Setup the spi for 8 bit data, high steady state clock,
   // second edge capture, with a 1MHz clock rate
   spi.format(16, 3);
   spi.frequency(1000000);

   for(int i=0; i<5; ++i){ //Run for 5 times
      // Chip must be deselected
        cs = 1;
        // Select the device by seting chip select low
        cs = 0;
        

        //printf("----------master----------\n");
        
        printf("Master input CMD:\n");
        scanf("%d", &cmd);
        printf("%d\n",cmd);
        spi.write(cmd);

        ThisThread::sleep_for(100ms);
        response = spi.write(cmd);

        switch (response) {
            case 1: {
                        printf("Master: x,y =");
                        scanf("%d,%d", &x, &y);
                        printf("%d,%d\n", x, y);
                        spi.write(x);
                        spi.write(y);
                        break;
                    }
            case 2: {printf("Master: Clear.\n");break;}
            case 3: {
                        printf("Master: Number =");
                        scanf("%d", &num);
                        printf("%d", num);
                        spi.write(num);
                        break;
                    }
        } 
    }
}

// main() runs in its own thread in the OS
int main()
{
    thread_slave.start(slave);
    thread_master.start(master);
}
