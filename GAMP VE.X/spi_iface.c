
#include "spi_iface.h"


unsigned char SPI_exchange8Bit( unsigned char i )
{
    SPI1BUF = i;			// write to buffer for TX
    while(SSP1STATbits.BF == 0){};      // wait for transfer to complete
    return SPI1BUF;    			// read the received value
}


void SPI_writeBuffer(unsigned char *buffer, unsigned len)
{
    unsigned n;
    unsigned char returned;
    // Just generate it here for the time being.
    for(n = 0; n< len;n++)
    {
        SPI1BUF = buffer[n];
        while(SSP1STATbits.BF == 0){};

        returned = SPI1BUF;
    }
}

void SPI_readBuffer(unsigned char *buffer, unsigned count)
{
    unsigned n;
    for (n=0; n<count; n++)
    {
        SPI1BUF = DUMMY_DATA;
        while(SSP1STATbits.BF == 0){};
        buffer[n] = SPI1BUF;
    }
}