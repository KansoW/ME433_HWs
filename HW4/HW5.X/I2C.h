#ifndef I2C_H__
#define I2C_H__
// Header file for I2C.c
// helps implement use I2C1 as a master without using interrupts

#define SLAVE_ADDR 0x20 //derived from opcode

void i2c_master_setup(void);              // set up I2C 1 as a master, at 100 kHz

void i2c_master_start(void);              // send a START signal
void i2c_master_restart(void);            // send a RESTART signal
void i2c_master_send(unsigned char byte); // send a byte (either an address or data)
unsigned char i2c_master_recv(void);      // receive a byte of data
void i2c_master_ack(int val);             // send an ACK (0) or NACK (1)
void i2c_master_stop(void);               // send a stop

void init_expander(void);
void set_expander(unsigned char, unsigned char);
unsigned char get_expander(unsigned char);
#endif