// pin connections:
// VCC - 3.3V
// GND - GND
// CS - B7
// RESET - 3.3V
// A0 - B15
// SDA - RA1
// SCK - B14
// LED - 3.3V

// B8 is turned into SDI1 but is not used or connected to anything

#include <xc.h>
#include "LCD.h"

void SPI1_init() {
	SDI1Rbits.SDI1R = 0b0100; // B8 is SDI1 
    RPA1Rbits.RPA1R = 0b0011; // A1 is SDO1
    TRISBbits.TRISB7 = 0; // SS is B7
    LATBbits.LATB7 = 1; // SS starts high

    // A0 / DAT pin
    ANSELBbits.ANSB15 = 0;
    TRISBbits.TRISB15 = 0;
    LATBbits.LATB15 = 0;
	
	SPI1CON = 0; // turn off the spi module and reset it
    SPI1BUF; // clear the rx buffer by reading from it
    SPI1BRG = 1; // baud rate to 12 MHz [SPI1BRG = (48000000/(2*desired))-1]
    SPI1STATbits.SPIROV = 0; // clear the overflow bit
    SPI1CONbits.CKE = 1; // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1; // master operation
    SPI1CONbits.ON = 1; // turn on spi1
}

unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}

void LCD_command(unsigned char com) {
    LATBbits.LATB15 = 0; // DAT
    LATBbits.LATB7 = 0; // CS
    spi_io(com);
    LATBbits.LATB7 = 1; // CS
}

void LCD_data(unsigned char dat) {
    LATBbits.LATB15 = 1; // DAT
    LATBbits.LATB7 = 0; // CS
    spi_io(dat);
    LATBbits.LATB7 = 1; // CS
}

void LCD_data16(unsigned short dat) {
    LATBbits.LATB15 = 1; // DAT
    LATBbits.LATB7 = 0; // CS
    spi_io(dat>>8);
    spi_io(dat);
    LATBbits.LATB7 = 1; // CS
}

void LCD_init() {
    int time = 0;
    LCD_command(CMD_SWRESET);//software reset
    time = _CP0_GET_COUNT();
    while (_CP0_GET_COUNT() < time + 48000000/2/2) {} //delay(500);

	LCD_command(CMD_SLPOUT);//exit sleep
    time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/200) {} //delay(5);

	LCD_command(CMD_PIXFMT);//Set Color Format 16bit
	LCD_data(0x05);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/200) {} //delay(5);

	LCD_command(CMD_GAMMASET);//default gamma curve 3
	LCD_data(0x04);//0x04
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {} //delay(1);

	LCD_command(CMD_GAMRSEL);//Enable Gamma adj
	LCD_data(0x01);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {} //delay(1);

	LCD_command(CMD_NORML);

	LCD_command(CMD_DFUNCTR);
	LCD_data(0b11111111);
	LCD_data(0b00000110);

    int i = 0;
	LCD_command(CMD_PGAMMAC);//Positive Gamma Correction Setting
	for (i=0;i<15;i++){
		LCD_data(pGammaSet[i]);
	}

	LCD_command(CMD_NGAMMAC);//Negative Gamma Correction Setting
	for (i=0;i<15;i++){
		LCD_data(nGammaSet[i]);
	}

	LCD_command(CMD_FRMCTR1);//Frame Rate Control (In normal mode/Full colors)
	LCD_data(0x08);//0x0C//0x08
	LCD_data(0x02);//0x14//0x08
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {} //delay(1);

	LCD_command(CMD_DINVCTR);//display inversion
	LCD_data(0x07);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {} //delay(1);

	LCD_command(CMD_PWCTR1);//Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
	LCD_data(0x0A);//4.30 - 0x0A
	LCD_data(0x02);//0x05
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {} //delay(1);

	LCD_command(CMD_PWCTR2);//Set BT[2:0] for AVDD & VCL & VGH & VGL
	LCD_data(0x02);
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {} //delay(1);

	LCD_command(CMD_VCOMCTR1);//Set VMH[6:0] & VML[6:0] for VOMH & VCOML
	LCD_data(0x50);//0x50
	LCD_data(99);//0x5b
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {} //delay(1);

	LCD_command(CMD_VCOMOFFS);
	LCD_data(0);//0x40
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {} //delay(1);

	LCD_command(CMD_CLMADRS);//Set Column Address
	LCD_data16(0x00);
    LCD_data16(_GRAMWIDTH);

	LCD_command(CMD_PGEADRS);//Set Page Address
	LCD_data16(0x00);
    LCD_data16(_GRAMHEIGH);

	LCD_command(CMD_VSCLLDEF);
	LCD_data16(0); // __OFFSET
	LCD_data16(_GRAMHEIGH); // _GRAMHEIGH - __OFFSET
	LCD_data16(0);

	LCD_command(CMD_MADCTL); // rotation
    LCD_data(0b00001000); // bit 3 0 for RGB, 1 for GBR, rotation: 0b00001000, 0b01101000, 0b11001000, 0b10101000

	LCD_command(CMD_DISPON);//display ON
	time = _CP0_GET_COUNT();
	while (_CP0_GET_COUNT() < time + 48000000/2/1000) {} //delay(1);

	LCD_command(CMD_RAMWR);//Memory Write
}

void LCD_drawPixel(unsigned short x, unsigned short y, unsigned short color) {
    // check boundary
    LCD_setAddr(x,y,x+1,y+1);
    LCD_data16(color);
}

void LCD_setAddr(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1) {
    LCD_command(CMD_CLMADRS); // Column
    LCD_data16(x0);
	LCD_data16(x1);

	LCD_command(CMD_PGEADRS); // Page
	LCD_data16(y0);
	LCD_data16(y1);

	LCD_command(CMD_RAMWR); //Into RAM
}

void LCD_clearScreen(unsigned short color) {
    int i;
    LCD_setAddr(0,0,_GRAMWIDTH,_GRAMHEIGH);
		for (i = 0;i < _GRAMSIZE; i++){
			LCD_data16(color);
		}
}

void draw_byte(unsigned int byte, unsigned short row, unsigned short col, unsigned short color1, unsigned short color2){
    int i;
    for (i=0;i<8;i++){// 8 pixels per column
        if(col+i<128 && col>0){// check if pixel exists
            if(byte >> i & 1){// fill pixel desired color
                LCD_drawPixel(row, col+i, color1);
            }
            else{// fill pixel with background color
                LCD_drawPixel(row, col+i, color2);
            }
        }
    }
    
}

void draw_char(char c, unsigned short x0, unsigned short y0, unsigned short color1, unsigned short color2){
    char c_ind = c-0x20; // Index of 5x8 pixel char in ASCII array
    int i;
    
    for(i=0;i<5;i++){ // 5 pixels per row
        if ((x0+i) < 128 && x0>0){ // Check if row exists
            draw_byte(ASCII[c_ind][i], x0+i, y0, color1, color2); 
        }
    }
}

void draw_string(char* msg, unsigned short x, unsigned short y, unsigned short color1, unsigned short color2){
    int i = 0;
    while (msg[i]!=0){
        draw_char(msg[i], x, y, color1, color2);
        i++;
        x+=5; // Set start position to top left pixel of new char
    }
}

void draw_xbar(unsigned short x0, unsigned short y0, unsigned short length, unsigned short height, unsigned short color1, unsigned short color2, float xscale){
    int i,j,xfill,xstep;
    unsigned short x_move,y_move,x,y; // pixel coordinates
    unsigned short l_max, h_max; // absolute value of length and height
    
    //Finding absolute dimensions of both bars
    x_move = (length >= 0)? 1:-1; //set x_move = sign(length)*1 
    y_move = (height >= 0)? 1:-1; //set y_move = sign(height)*1 
    l_max = x_move*length; //abs(length)
    h_max = y_move*height; //abs(height)
    
    xstep = (xscale >= 0)? 1:-1;
    xfill = (int)(0.01*xstep*xscale*l_max); //positive % of l_max
    if(xfill>l_max){
        xfill = l_max; //Capping value if xscale over +-100%
    }
    
    //x bar
    for(i=0;i<xfill;i++){
        x = x0 + xstep*i;
        for(j=0;j<h_max;j++){
            y = y0+y_move*j;
            LCD_drawPixel(x,y,color1); // Fill bar with desired color
        }
    }
    
    for(i=xfill;i<l_max;i++){
        x = x0 + xstep*i;
        for(j=0;j<h_max;j++){
            y = y0+y_move*j;
            LCD_drawPixel(x,y,color2); // Fill bar with bg color
        }
    }
    
}

void draw_ybar(unsigned short x0, unsigned short y0, unsigned short length, unsigned short height, unsigned short color1, unsigned short color2, float yscale){
    int i,j,yfill,ystep;
    unsigned short x_move,y_move,x,y; // pixel coordinates
    unsigned short l_max, h_max; // absolute value of length and height
    
    //Finding absolute dimensions of both bars
    x_move = (length >= 0)? 1:-1; //set x_move = sign(length)*1 
    y_move = (height >= 0)? 1:-1; //set y_move = sign(height)*1 
    l_max = x_move*length; //abs(length)
    h_max = y_move*height; //abs(height)
    
    ystep = (yscale >= 0)? 1:-1;
    yfill = (int)(0.01*ystep*yscale*h_max); //positive % of h_max
    if (yfill>h_max){
        yfill = h_max;  //Capping value if yscale over +-100%
    }
    
    //y bar
    for(i=0;i<yfill;i++){
        y = y0 + ystep*i;
        for(j=0;j<l_max;j++){
            x = x0+j*x_move;
            LCD_drawPixel(x,y,color1); // Fill bar with desired color
        }
    }
    
    for(i=yfill;i<h_max;i++){
        y = y0 + ystep*i;
        for(j=0;j<l_max;j++){
            x = x0+x_move*j;
            LCD_drawPixel(x,y,color2); // Fill bar with bg color
        }
    }
    
}