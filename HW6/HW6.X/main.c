#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "LCD.h"
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
    SPI1_init();
    LCD_init();
    
    __builtin_enable_interrupts();
    LCD_clearScreen(YELLOW);
    char hhog1[20];
    char hhog2[20];
    char hhog3[20];
    char hhog4[20];
    char message[20];
    int count = 0;
    float fps = 0;
    
    while(1) {
        _CP0_SET_COUNT(0);
        sprintf(hhog1, "   ..::::::::.");
        sprintf(hhog2,"  :::::::::::::");
        sprintf(hhog3," /. `:::::::::::");
        sprintf(hhog4,"o__,_||||||||||'");
        draw_string(hhog1,25,20,BLUE,YELLOW);
        draw_string(hhog2,25,30,BLUE,YELLOW);
        draw_string(hhog3,25,40,BLUE,YELLOW);
        draw_string(hhog4,25,50,BLUE,YELLOW);  
        
        sprintf(message, "Hello World! %3d", count);
        draw_string(message,28,90,BLACK,YELLOW);

        //draw_bar
        draw_bar(15,70,100,5,RED,WHITE, count);
        
        sprintf(message, "FPS: %.2f", fps);
        draw_string(message,28,100,BLACK,YELLOW);
        fps = 24000000./_CP0_GET_COUNT();
        
        while(_CP0_GET_COUNT()<48000000/2/5){//Loop at 5Hz
            //USB 2x slower than peripheral
            ;
        }
        count++;
        
        if(count == 101 || count == -101){
            count = 0;
        } 
        
    }// end infinite while
    
}// end main