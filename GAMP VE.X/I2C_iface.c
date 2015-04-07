
#include "LowlevelMacros.h"
#include "I2C_iface.h"
#include <p18cxxx.h>
#include "led.h"
#include <i2c.h>

/********************************************************************
*   Function Name:  OpenI2C2                                        *
*   Return Value:   void                                            *
*   Parameters:     SSP2 peripheral setup bytes                     *
*   Description:    This function sets up the SSP2 module on a      *
*                   PIC18CXXX device for use with a Microchip I2C   *
*                   EEPROM device or I2C bus device.                *
********************************************************************/
#if defined (I2C_V3)
void OpenI2C2( unsigned char sync_mode, unsigned char slew )
{
  SSP2STAT &= 0x3F;                // power on state
  SSP2CON1 = 0x00;                 // power on state
  SSP2CON2 = 0x00;                 // power on state
  SSP2CON1 |= sync_mode;           // select serial mode
  SSP2STAT |= slew;                // slew rate on/off

  I2C2_SCL = 1;
  I2C2_SDA = 1;

  SSP2CON1 |= SSPENB;              // enable synchronous serial port
}
#endif

#if defined (I2C_V6) || defined (I2C_V6_1)
void openI2C2( unsigned char sync_mode, unsigned char slew )
{
  SSP2STAT &= 0x3F;                // power on state
  SSP2CON1 = 0x00;                 // power on state
  SSP2CON2 = 0x00;                 // power on state
  SSP2CON1 |= sync_mode;           // select serial mode
  SSP2STAT |= slew;                // slew rate on/off

  SSP2CON1 |= SSPENB;              // enable synchronous serial port
}
#endif




/********************************************************************
*     Function Name:    WriteI2C2                                   *
*     Return Value:     Status byte for WCOL detection.             *
*     Parameters:       Single data byte for I2C2 bus.              *
*     Description:      This routine writes a single byte to the    *
*                       I2C2 bus.                                   *
********************************************************************/

#if defined (I2C_V3) || defined (I2C_V6) || defined (I2C_V6_1)
signed char writeI2C2( unsigned char data_out )
{
  SSP2BUF = data_out;           // write single byte to SSP2BUF
  if ( SSP2CON1bits.WCOL )      // test if write collision occurred
    return ( -1 );              // if WCOL bit is set return negative #
  else
  {
	if( ((SSP2CON1&0x0F)!=0x08) && ((SSP2CON1&0x0F)!=0x0B) )	//slave mode only
	{
	 #if defined (I2C_SFR_V1)
     if ( ( !SSP2STATbits.R_NOT_W ) && ( !SSP2STATbits.BF ) )
	 #else
      if ( ( !SSP2STATbits.R_W ) && ( !SSP2STATbits.BF ) )// if R/W=0 and BF=0, NOT ACK was received
	 #endif
      {
        return ( -2 );             // return NACK
      }
	  else return(0);				// return ACK

	}
    else
        if( ((SSP2CON1&0x0F)==0x08) || ((SSP2CON1&0x0F)==0x0B) )	//master mode only
	{
	    while( SSP2STATbits.BF );   // wait until write cycle is complete
	    IdleI2C2();                  // ensure module is idle
	    if ( SSP2CON2bits.ACKSTAT ) // test for ACK condition received
	    	 return ( -2 );				//return NACK
		else return ( 0 );   			//return ACK
	}

  }
  return 0;
}
#endif



/********************************************************************
*     Function Name:    ReadI2C2                                    *
*     Return Value:     contents of SSP2BUF register                *
*     Parameters:       void                                        *
*     Description:      Read single byte from I2C2 bus.             *
********************************************************************/
#if defined (I2C_V3) || defined (I2C_V6) || defined (I2C_V6_1)
unsigned char readI2C2( void )
{

if( ((SSP2CON1&0x0F)==0x08) || ((SSP2CON1&0x0F)==0x0B) )	//master mode only
  SSP2CON2bits.RCEN = 1;           // enable master for 1 byte reception
  while ( !SSP2STATbits.BF );      // wait until byte received
  return ( SSP2BUF );              // return with read byte
}
#endif


/********************************************************************
*     Function Name:    RestartI2C2                                 *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      Send I2C2 bus restart condition.            *
********************************************************************/
#undef RestartI2C2
#if defined (I2C_V3) || defined (I2C_V6) || defined (I2C_V6_1)
void restartI2C2( void )
{
  SSP2CON2bits.RSEN = 1;           // initiate bus restart condition
}
#endif




/********************************************************************
*     Function Name:    StopI2C2                                    *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      Send I2C2 bus stop condition.               *
********************************************************************/
#undef StopI2C2
#if defined (I2C_V3) || defined (I2C_V6) || defined (I2C_V6_1)
void stopI2C2( void )
{
  SSP2CON2bits.PEN = 1;            // initiate bus stop condition
}
#endif



/********************************************************************
*     Function Name:    StartI2C2                                   *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      Send I2C2 bus start condition.              *
********************************************************************/
#if defined (I2C_V3) || defined (I2C_V6) || defined (I2C_V6_1)
#undef StartI2C2
void startI2C2( void )
{
  SSP2CON2bits.SEN = 1;            // initiate bus start condition
}
#endif



/********************************************************************
*     Function Name:    IdleI2C2                                    *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      Test and wait until I2C2 module is idle.    *
********************************************************************/

#if defined (I2C_V3) || defined (I2C_V6) || defined (I2C_V6_1)
#undef IdleI2C2
void idleI2C2( void )
{
  #if defined (I2C_SFR_V1)
    while ((SSP2CON2 & 0x1F) | (SSP2STATbits.R_NOT_W))
  #else
    while ((SSP2CON2 & 0x1F) | (SSP2STATbits.R_W))
  #endif
     continue;
}
#endif



/********************************************************************
*     Function Name:    AckI2C2                                      *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      Initiate ACK bus condition.                 *
********************************************************************/
#if defined (I2C_V3) || defined (I2C_V6) || defined (I2C_V6_1)
#undef ackI2C2
void ackI2C2( void )
{
  SSP2CON2bits.ACKDT = 0;           // set acknowledge bit state for ACK
  SSP2CON2bits.ACKEN = 1;           // initiate bus acknowledge sequence
}
#endif



/********************************************************************
*     Function Name:    NotAckI2C2                                  *
*     Return Value:     void                                        *
*     Parameters:       void                                        *
*     Description:      Initiate NOT ACK bus condition.             *
********************************************************************/
#undef notAckI2C2
#if defined (I2C_V3) || defined (I2C_V6) || defined (I2C_V6_1)
void notAckI2C2( void )
{
  SSP2CON2bits.ACKDT = 1;          // set acknowledge bit for not ACK
  SSP2CON2bits.ACKEN = 1;          // initiate bus acknowledge sequence
}
#endif


unsigned char I2CReadRegister(unsigned char busAdr, unsigned char reg, unsigned char *buffer, unsigned len)
{
    signed char result = 0;

    // Start the bus
    idleI2C2();
    startI2C2();
    idleI2C2();
    // Nominate the register on the bus device
    if((result = writeI2C2(MAKEI2CWRITEADDRESS(busAdr))) == -2)
    {
        // if collision we should send again.
        // if Nack we should fail the device somewhere
        debugI2C(1);
        stopI2C2();
        return ERROR;
    }
    idleI2C2();
    if((result = writeI2C2(reg)) == -2)
    {
        // if collision we should send again.
        // if Nack we should fail the device somewhere
        debugI2C(2);
        stopI2C2();
        return ERROR;
    }
    idleI2C2();

    // Restart the bus and send the read command
    restartI2C2();
    idleI2C2();
    if((result = writeI2C2(MAKEI2CREADADDRESS(busAdr))) == -2)
    {
        // if collision we should send again.
        // if Nack we should fail the device somewhere
        debugI2C(2);
        stopI2C2();
        return ERROR;
    }
    unsigned n = 0;
    // Clock the data into the PIC
    do
    {
        idleI2C2();
        buffer[n] = readI2C2();
        idleI2C2();
        n++;
        if(n < len)
            ackI2C2();
        else
            notAckI2C2();
    } while (n < len);

    // Stop the Bus Transaction
    idleI2C2();
    stopI2C2();

    return SUCCESS;
}

unsigned char I2CWriteRegister(unsigned char busAdr, unsigned char reg, unsigned char *buffer, unsigned len)
{
    signed char result = 0;
    idleI2C2();
    startI2C2();
    idleI2C2();
    if((result = writeI2C2(MAKEI2CWRITEADDRESS(busAdr))) == -2)
    {
        // if collision we should send again.
        // if Nack we should fail the device somewhere
        debugI2C(4);
        stopI2C2();
        return ERROR;
    }
    idleI2C2();
    if((result = writeI2C2(reg)) == -2)
    {
        // if collision we should send again.
        // if Nack we should fail the device somewhere
        debugI2C(5);
        stopI2C2();
        return ERROR;
    }
    idleI2C2();
    for(unsigned n = 0 ; n < len; n++)
    {
        if((result = writeI2C2(buffer[n])) == -2)
        {
            // if collision we should send again.
            // if Nack we should fail the device somewhere
            debugI2C(6);
            stopI2C2();
            return ERROR;
        }
        idleI2C2();
    }
    stopI2C2();
    return SUCCESS;
}

void debugI2C(unsigned n)
{
    flashLEDN(LED2MASK,n,255);
};