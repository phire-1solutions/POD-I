/* 
 * File:   spi_iface.h
 * Author: phamilt5
 *
 * Created on February 25, 2015, 12:36 PM
 */

#ifndef SPI_IFACE_H
#define	SPI_IFACE_H

#include "p18f47j53.h"
#include "LowLevelMacros.h"


#define readSPI1 writeSPI1(junkbyte);

#define ENABLESPI1                      SSP1CON1bits.SSPEN = 1;
#define DISABLESPI1                     SSP1CON1bits.SSPEN = 0;

#define SPI1BUF                         SSP1BUFbits.SSPBUF
#define SPI1STATUS                      SSP1STATbits.BF
#define DUMMY_DATA                      0xFF

unsigned char SPI_exchange8Bit( unsigned char i );
void  SPI_writeBuffer(unsigned char *buffer, unsigned len);
void SPI_readBuffer(unsigned char *buffer, unsigned count);

#endif	/* SPI_IFACE_H */

