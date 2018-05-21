#ifndef SPI_H
#define SPI_H

// Shorthand for toggling LED pin
#define CS LATAbits.LATA4
#define A 0
#define B 1

// Prototypes
void spi_init(void);
char spi_io(unsigned char);
void write(unsigned int, unsigned int);

#endif