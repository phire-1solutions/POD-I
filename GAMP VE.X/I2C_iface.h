/* 
 * File:   I2C_iface.h
 * Author: Paul
 *
 * Created on 7 March 2015, 9:04 AM
 */

#ifndef I2C_IFACE_H
#define	I2C_IFACE_H


#define MAKEI2CREADADDRESS(busAdr) (busAdr | 0x01)
#define MAKEI2CWRITEADDRESS(busAdr) (busAdr)


void openI2C2( unsigned char sync_mode, unsigned char slew );
signed char writeI2C2( unsigned char data_out );
unsigned char readI2C2( void );
void restartI2C2( void );
void stopI2C2( void );
void startI2C2( void );
void idleI2C2(void);
void ackI2C2( void );
void notAckI2C2( void );
void debugI2C(unsigned n);

unsigned char I2CReadRegister(unsigned char busAdr, unsigned char reg, unsigned char *buffer, unsigned len);
unsigned char I2CWriteRegister(unsigned char busAdr, unsigned char reg, unsigned char *buffer, unsigned len);


#endif	/* I2C_IFACE_H */

