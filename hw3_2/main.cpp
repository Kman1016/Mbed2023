#include "mbed.h"
#include "TextLCD.h"
#include <cstdio>
static BufferedSerial pc(USBTX, USBRX);
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
Thread thread1, thread2;
//char cmd[4];
uint32_t num;

FileHandle *mbed::mbed_override_console(int fd)
{
   return &pc;
}

void master_thread() {
    while(1){
        char cmd[4];
        if (pc.readable()) {
            //printf("cmd:\n");
            for (int i = 0; i < 3; i++) {
                char input;
                num = pc.read(&input,  sizeof(cmd));
                cmd[i] = input;
                //printf("%c\n", input);
            }
            //lcd.puts(cmd);
            //lcd.putc('\n');
            ThisThread::sleep_for(1s);
            lcd.cls();

            printf("%s\n", cmd);
            //  (1) void locate(int column, int row); 
            //  (2) void cls(); 
            //  (3) int rows(); 
            //  (4) int columns(); 
            //  (5) int putc(int c)
            //for (int i; i < 10; i++)

            //pc.write(cmd, sizeof(cmd));
            //ThisThread::sleep_for(100ms);   

            //printf("content:");
            if (cmd[0] == 'l' && cmd[1] == 'o' && cmd[2] == 'c') {
                //printf("c, r:");
                char r, c;
                num = pc.read(&c, 32);
                num = pc.read(&r, 32);
                lcd.printf("[c,r] = [%c, %c]\n", (c), (r));
                lcd.locate(int(c)-48, int(r)-48);
            } else if (cmd[0] == 'c' && cmd[2] == 'l' && cmd[2] == 's') {
                lcd.cls();
            } else if (cmd[0] == 'r' && cmd[1] == 'o' && cmd[2] == 'w') {
                lcd.printf("row: %d\n", lcd.rows());
            } else if (cmd[0] == 'c' && cmd[1] == 'o' && cmd[2] == 'l') {
                lcd.printf("column: %d\n", lcd.columns());
            } else if (cmd[0] == 'p' && cmd[1] == 'u' && cmd[2] == 't') {
                char chr;
                num = pc.read(&chr, 8);
                lcd.putc(chr);
            } 
        }
    }
}

/*
void lcdProcess(void) {
    while (1) {
        if (pc.readable()) {
            //lcd.puts(input);
            
            if (input[0] == 'l') {
                lcd.locate(input[2], input[4]);
            } else if (input[0] == 'c' && input[1] == 'l' && input[2] == 's') {
                lcd.cls();
            } else if (input[0] == 'r' && input[1] == 'o' && input[2] == 'w') {
                lcd.puts("rows: ");
                lcd.putc(lcd.rows());
                lcd.putc('\n');
            } else if (input[0] == 'c' && input[1] == 'o' && input[2] == 'l') {
                lcd.printf("column: %d", lcd.columns());
            } else if (input[0] == 'p' && input[1] == 'u' && input[2] == 't' && input[3] == 'c') {
                lcd.putc(input[5]);
            }   
            
        }
    }
}
*/

int main()
{
    thread1.start(master_thread);
    //thread2.start(lcdProcess);
    
}

