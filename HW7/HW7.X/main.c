#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "LCD.h"
#include "IMU.h"
#include <math.h>
#include <stdio.h>

// DEVCFG0
#pragma config DEBUG = 0b1 // no debugging
#pragma config JTAGEN = 0 // no jtag
#pragma config ICESEL = 0b11 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = 0 // no boot write protect
#pragma config CP = 1 // no code protect

// DEVCFG1
#pragma config FNOSC = 0b011 // use primary oscillator with pll
#pragma config FSOSCEN = 0 // turn off secondary oscillator
#pragma config IESO = 0 // no switching clocks
#pragma config POSCMOD = 0b10 // high speed crystal mode
#pragma config OSCIOFNC = 1 // disable secondary osc
#pragma config FPBDIV = 0b00 // divide sysclk freq by 1 for peripheral bus clock
#pragma config FCKSM = 0b1 // do not enable clock switch
#pragma config WDTPS = 00000 // use slowest wdt
#pragma config WINDIS = 1 // wdt no window mode
#pragma config FWDTEN = 0 // wdt disabled
#pragma config FWDTWINSZ = 0b11 // wdt window at 25%

// DEVCFG2 - get the sysclk clock to 48MHz from the 8MHz crystal
#pragma config FPLLIDIV = 0b001 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = 0b111 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = 0b001 //   divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = 0b001 // divider for the 8MHz input clock, then multiplied by 12 to get 48MHz for USB
#pragma config UPLLEN = 0 // USB clock on

// DEVCFG3
#pragma config USERID = 0x0101 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = 0 // allow multiple reconfigurations
#pragma config IOL1WAY = 0 // allow multiple reconfigurations
#pragma config FUSBIDIO = 1 // USB pins controlled by USB module
#pragma config FVBUSONIO = 1 // USB BUSON controlled by USB module


int main() {

    __builtin_disable_interrupts();

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;
    
    // initializations
    SPI1_init(); // Talk to LCD
    LCD_init();
    i2c_master_setup(); // Talk to IMU
    init_expander(); // Turn on accelerometer
    
    __builtin_enable_interrupts();
    char msg[100];
    int arrlen = 14;
    unsigned char data[arrlen];
    LCD_clearScreen(BLACK);
  
    sprintf(msg, "WHOAMI output: %d", get_expander(WHO_AM_I)); //0b01101001
    //draw_string(msg, 20, 20, RED, BLACK); //register returns 105
    
    while(1) {
        i2c_read_multiple(SLAVE_ADDR, OUT_TEMP_L, data, arrlen);
        
        // parse read values
        signed short temp = (data[1] << 8) | data[0]; //16-bit short
        signed short gyroX = (data[3] << 8) | data[2];
        signed short gyroY = (data[5] << 8) | data[4];
        signed short gyroZ = (data[7] << 8) | data[6];
        signed short accelX = (data[9] << 8) | data[8];
        signed short accelY = (data[11] << 8) | data[10];
        signed short accelZ = (data[13] << 8) | data[12];
        
        //scaling length and height
        float xscale = accelX*0.000061*100; 
        float yscale = accelY*0.000061*100;
        
        sprintf(msg, "x = %f", xscale);
        //draw_string(msg, 20, 100, RED, BLACK);
        sprintf(msg, "y = %f", yscale);
        //draw_string(msg, 20, 110, RED, BLACK);
        
        draw_xbar(64, 64, 50, 5, GREEN, BLACK, xscale); //xbar
        draw_ybar(64, 64, 5, 50, GREEN, BLACK, yscale); //ybar
        
        int i,j,xbox,ybox;
        for(i=0;i<5;i++){
            xbox = 64+i;
            for(j=0;j<5;j++){
                ybox = 64+j;
                LCD_drawPixel(xbox,ybox,RED);
            }
        }
        
        //5Hz loop
        _CP0_SET_COUNT(0);
        while (_CP0_GET_COUNT() < 48000000/2/5){
            ;
        }
        
    }// end infinite while
    
}// end main