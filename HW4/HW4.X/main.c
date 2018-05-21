#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "SPI.h"
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
    
    spi_init();
    
    // Build sine and ramp waves
    unsigned int sine_wave[100];
    unsigned int ramp_wave[200];
    double s_tmp, r_tmp;
    int i,j;
    
    for(i=0; i<100; i++){ //10Hz sine wave
        s_tmp = (255.0/2.0) + (255.0/2.0)*sin(2*M_PI*(i/100.0));
        sine_wave[i] = s_tmp;
    }
   
    for(j=0; j<200; j++){ //5Hz triangle wave
        r_tmp = (j/200.0)*255.0;
        ramp_wave[j] = r_tmp;
    }
    
    __builtin_enable_interrupts();
            
    int k = 0, l = 0; // Counters
    while(1) {
        _CP0_SET_COUNT(0);
        while(_CP0_GET_COUNT() < 48000000/2/1000){
            ;
        }
        // Write sine wave to DAC A
        write(A, sine_wave[k]);
        
        // Write ramp wave to DAC B
        write(B, ramp_wave[l]);
        
        k++;
        l++;
        
        //Reset list index
        if (k == 100){
            k = 0;
        }
        if (l == 200){
            l = 0;
        }
        
    }// end infinite while
    
}// end main