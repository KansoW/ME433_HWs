#ifndef SPI_H
#define SPI_H

#include <xc.h>

void initSPI1();
char SPI1_IO(char write);
void setVoltage(char channel, char voltage);

#endif