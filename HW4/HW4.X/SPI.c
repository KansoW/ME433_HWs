#include <proc/p32mx250f128b.h>
#include "SPI.h"

void spi_init(void){
    SPI1CON = 0;
    SPI1BUF;                 // clear SPI buffer
    SPI1BRG = 1;
    
    SPI1STATbits.SPIROV = 0; // clear overflow bit
    SPI1CONbits.CKE = 1;     // change voltage output when clk active -> idle
    SPI1CONbits.CKP = 0;     // clk active when high
    SPI1CONbits.MSTEN = 1;    
    SPI1CONbits.ON = 1;

    //set cs pin as output
    TRISBbits.TRISB7 = 0; // connect cs to A4
           
    SDI1Rbits.SDI1R = 0x0;// RPA1 (Pin 3) as SDI1 Master input
    ANSELBbits.ANSB13 = 0; // Disable analog
    RPB13Rbits.RPB13R = 0b0011; // RPB13 as SDO1 Master output
    // RPB14 is fixed to SCK1 for SPI1    
}

//Sends a byte using SPI and returns the response
char spi_io(unsigned char c){
    SPI1BUF = c;
    while(!SPI1STATbits.SPIRBF){
        ;
    }
    return SPI1BUF;
}

void write(unsigned int channel, unsigned int voltage){
    unsigned int b1 = 0, b2 = 0; //bytes 1 and 2
    
    channel = (channel<<3 | 0b0111); //0b#111 
    b1 = (channel<<4 | voltage>>4); //#111 0000, 0000 #### = #111 ####
    b2 = voltage<<4; //#### 0000
    
    CS = 0; // start writing
    spi_io(b1);
    spi_io(b2);
    CS = 1; // finish writing
}