#ifndef I2C_H
#define I2C_H

void initI2C();
void i2c_master_setup(void);             
void i2c_master_start(void);             
void i2c_master_restart(void);            
void i2c_master_send(unsigned char byte); 
unsigned char i2c_master_recv(void);     
void i2c_master_ack(int val);            
void i2c_master_stop(void);
void initExpander();
void setExpander(char pin, char level);
char getExpander();

#endif