#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
#include "SPI.h"

#define CS LATBbits.LATB7      // chip select pin

void initSPI1() {
    
  TRISBbits.TRISB7 = 0; // set B13 as output
  CS = 1; // initialize chip select
  
  // SDI1Rbits.SDI1R = 0b0; // use B# as SDI1 pin
  RPB13Rbits.RPB13R = 0b0011;  // use B15 as SD01 pin
  
  SPI1CON = 0;              // reset spi
  SPI1BUF;                  // clear buffer
  SPI1BRG = 0x1;            // baud rate 12 MHz 
  SPI1STATbits.SPIROV = 0;  // clear overflow bit
  SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo 
  SPI1CONbits.MSTEN = 1;    // master operation
  SPI1CONbits.ON = 1;       // turn on SPI1
}

char SPI1_IO(char write)
{
    SPI1BUF = write;
    while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
        ;
    }
    return SPI1BUF;
}

void setVoltage(char channel, char voltage)
{    
    CS = 0; // start writing

    unsigned char vb = (voltage & 0xF0) >> 4;
    unsigned char va = (voltage & 0x0F) << 4;
 
    if(channel == 0) {
        SPI1_IO(((0x3)<<4)|vb);
        SPI1_IO(va);
    } else {
        SPI1_IO((0xB<<4)|vb);
        SPI1_IO(va);
    }
 
    CS = 1; // finish writing 
   
}