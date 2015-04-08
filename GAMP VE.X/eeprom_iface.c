
#include "p18f47j53.h"
#include <delays.h>
#include <string.h>
#include "Application.h"
#include "errorlog.h"
#include "spi_iface.h"
#include "eeprom_iface.h"
#include "rtc_iface.h"

// TODO:  change bufselector to an enum

// Data is written to the buffers in the flash using writeData.
// When a buffer is filled it is swapped and the full buffer is
// written to a page.

// Not sure if this step is necessary.
// If there is no write in a 10 second period and there is data to be
// written, it is written by the background interupt cycle.

void EEPROM_requestManufactureId(void);

#define JEDEC_MANUFACTURERS_ID_BYTES 5
unsigned char manufacturers_Id[JEDEC_MANUFACTURERS_ID_BYTES];
unsigned char writePending;
unsigned char currentFlashBuffer;

sPage0 eeprom_page0;

const char PHIRE1Signature[] = "PHire-1";


/* ****************************************************************************
 * Function: alternateBuffer                                                  *
 * Returns the number of the buffer not being used.                           *
 * Inputs: void                                                               *
 * Returns:unsigned char bufferNotInUse                                       *
 * Atomic SPI Bus Transaction                                                 *
 * ***************************************************************************/
unsigned char alternateBuffer(void)
{
    if (currentFlashBuffer == 1)
        return 2;
    else
        return 1;
}



/* ****************************************************************************
 * Function: requestStatus                                                    *
 * Read and return the status register from the Flash Chip                    *
 * Inputs:                                                                    *
 *  unsigned char *byte1                                                      *
 *  unsigned char *byte2                                                      *
 * Returns:void                                                               *
 * Atomic SPI Bus Transaction                                                 *
 * ***************************************************************************/
void requestStatus(EEPROM_Status_byte1 *p1, EEPROM_Status_byte2 *p2)
{
    // OPCODE 0xD7
    //
    MEM_CS_LO
    SPI_exchange8Bit(EEPROM_STATUS_OPC);
    (*p1).b1 =  SPI_exchange8Bit(DUMMY_DATA);
    (*p2).b2 = SPI_exchange8Bit(DUMMY_DATA);
    MEM_CS_HI
}


/* ****************************************************************************
 * Function: waitReady                                                        *
 * returns when the previous transazction is completed                        *
 * also checks the validity of previous program results                       *
 * Inputs: void                                                               *
 * Returns:1 if busy 0 otherwise                                              *
 * Atomic SPI Bus Transaction                                                 *
 * ***************************************************************************/
void waitReady(void)
{
    unsigned char ready = FALSE;
    EEPROM_Status_byte2 b;
    MEM_CS_LO;

    // Send the status command
    SPI_exchange8Bit(EEPROM_STATUS_OPC);

    // Continually poll for bit 7 = 1
    while (! ready)
    {
        SPI_exchange8Bit(DUMMY_DATA); // dump the first byte

        // check the second byte. This is not efficient, 
        // but the Erase Error is only available in the second byte.
        b.b2 = SPI_exchange8Bit(DUMMY_DATA);
        ready = b.Rdy_Busy;
    }
    MEM_CS_HI
   if(b.ErasePgmErr)
        APP_errorlog(PGM_ERR);
    writePending = FALSE;
}



unsigned char EEPROM_getManufacturerId(unsigned char index)
{
    if((manufacturers_Id[0]|manufacturers_Id[1]|manufacturers_Id[2]|
            manufacturers_Id[3]|manufacturers_Id[4]) == 0)
        EEPROM_requestManufactureId();
    if (index < JEDEC_MANUFACTURERS_ID_BYTES)
        return manufacturers_Id[index];
    else
        return manufacturers_Id[index];
}


void EEPROM_requestManufacturerId(void)
{
    MEM_CS_LO
    SPI_exchange8Bit(EEPROM_MANUFACTURERSID_OPC);
    SPI_readBuffer(manufacturers_Id,JEDEC_MANUFACTURERS_ID_BYTES);
    MEM_CS_HI
}


void EEPROM_writeBuffer(unsigned char bufselector, EEPROM_pADDRESS3 paddress, unsigned char *buffer, unsigned len)
{
    waitReady();

    MEM_CS_LO
    if(bufselector == EEPROM_BUFFER1)
        SPI_exchange8Bit(EEPROM_BUFFER1_WRITE);
    else
        SPI_exchange8Bit(EEPROM_BUFFER2_WRITE);

    SPI_writeBuffer((unsigned char *)paddress,sizeof(EEPROM_ADDRESS3));
    
    SPI_writeBuffer(buffer,len);
    MEM_CS_HI
}

void EEPROM_readBuffer(unsigned char bufselector, EEPROM_pADDRESS3 paddress, unsigned char *buffer, unsigned len)
{
    waitReady();

    MEM_CS_LO
    if(bufselector == EEPROM_BUFFER1)
        SPI_exchange8Bit(EEPROM_BUFFER1_READ_SLOW);
    else
        SPI_exchange8Bit(EEPROM_BUFFER2_READ_SLOW);

    SPI_writeBuffer((unsigned char *)paddress,sizeof(EEPROM_ADDRESS3));

    SPI_readBuffer(buffer,len);
    MEM_CS_HI
}


void EEPROM_storeBuffer(unsigned char bufselector,EEPROM_pADDRESS3 paddress)
{
    waitReady();

    MEM_CS_LO
    if(bufselector == EEPROM_BUFFER1)
        SPI_exchange8Bit(EEPROM_BUFFER1_STORE);
    else
        SPI_exchange8Bit(EEPROM_BUFFER2_STORE);
    SPI_writeBuffer((unsigned char *)paddress,sizeof(EEPROM_ADDRESS3));
    writePending = TRUE;

    MEM_CS_HI
}


void EEPROM_writeFlash(unsigned char bufselector, EEPROM_pADDRESS3 paddress, unsigned char *buffer, unsigned len)
{
    // OPCODE 0x82 or 0x85
    waitReady();

    MEM_CS_LO
    if(bufselector == EEPROM_BUFFER1)
        SPI_exchange8Bit(EEPROM_WRITE_B1_FLASH);
    else
        SPI_exchange8Bit(EEPROM_WRITE_B2_FLASH);
    SPI_writeBuffer((unsigned char *)paddress,sizeof(EEPROM_ADDRESS3));
    SPI_writeBuffer(buffer,len);
    writePending = TRUE;
    MEM_CS_HI
}

void EEPROM_readFlash(EEPROM_pADDRESS3 paddress, unsigned char *buffer, unsigned len)
{
    // OPCODE 0xD2
    unsigned char dummy[4];
    dummy[0] = 0;
    dummy[1] = 0;
    dummy[2] = 0;
    dummy[3] = 0;

    waitReady();

    MEM_CS_LO
    // Send OPCODE
    SPI_exchange8Bit(EEPROM_READ_FLASH);

    // Send 3 byte ADDRESS 256 or 264
    SPI_writeBuffer((unsigned char *)paddress,sizeof(EEPROM_ADDRESS3));

    // Send 4 bytes prepare clocking
    SPI_writeBuffer((unsigned char *)dummy,4);

    // Now get the buffer full of data
    SPI_readBuffer(buffer,len);                    
    MEM_CS_HI
}


void EEPROM_eraseChip(void)
{
    waitReady();

    MEM_CS_LO
    // do the erase
    writePending = TRUE;
    MEM_CS_HI

}


void EEPROM_readPage0(void)
{
    EEPROM_ADDRESS3 address;
    
    SETADDRESS3_256(address,0,0) // Page 0 Offset 0
    
    //Atomic Transaction
    EEPROM_readFlash(&address, (unsigned char*)&eeprom_page0, sizeof(eeprom_page0));
}


void EEPROM_writePage0(void)
{
    EEPROM_ADDRESS3 address;

    SETADDRESS3_256(address,0,0) // page 0 Offset 0

    // Atomic Transaction
    EEPROM_writeFlash(alternateBuffer(), &address, (unsigned char *)&eeprom_page0, sizeof(eeprom_page0));
}


void EEPROM_initialisePage0(void)
{

    for (unsigned n=0; n < sizeof(PHIRE1Signature);n++)
        eeprom_page0.signature[n] = PHIRE1Signature[n];
    eeprom_page0.version = Product_Version;
    eeprom_page0.currentRPage = 1;
    eeprom_page0.currentWPage = 1;
    eeprom_page0.lastByte = 0;
    eeprom_page0.inServiceTm.hrs = 0;
    eeprom_page0.inServiceTm.min = 0;
    eeprom_page0.inServiceTm.sec = 0;
    eeprom_page0.lastRunTm.hrs = 0;
    eeprom_page0.lastRunTm.min = 0;
    eeprom_page0.lastRunTm.sec = 0;
    eeprom_page0.startTm.hrs = 0;
    eeprom_page0.startTm.min = 0;
    eeprom_page0.startTm.sec = 0;
    EEPROM_writePage0();
}
#define  copyUTC(out,in) memcpy(out, in, sizeof(UTC));



void EEPROM_initialise(void)
{
    EEPROM_requestManufacturerId();
    writePending = FALSE;

    currentFlashBuffer = EEPROM_BUFFER1;

    EEPROM_readPage0();

    if ((strncmp(PHIRE1Signature,eeprom_page0.signature,7) != 0) ||
            (eeprom_page0.version != Product_Version))
    {
        EEPROM_initialisePage0();
    }

    RTCgetTime(&(eeprom_page0.startTm));
}

unsigned int EEPROM_newPage(void)
{
    EEPROM_ADDRESS3 address;
    SETADDRESS3_256(address,eeprom_page0.currentWPage,0);

    // Get save the current buffer
    // Atomic Transaction
    EEPROM_storeBuffer(currentFlashBuffer,&address);

    // reset the index position to start of page
    eeprom_page0.lastByte = 0;

    // Increment the Write page position
    eeprom_page0.currentWPage++;

    // if we hit the end, wrap around.
    if(eeprom_page0.currentWPage == EEPROM_MAXPAGES)
        eeprom_page0.currentWPage = 1;

    // now check if we have caught the read page pointer
    if(eeprom_page0.currentRPage == eeprom_page0.currentWPage)
    {
        // and if so, increment it
        eeprom_page0.currentRPage++;
        // and check for its overflow
        if (eeprom_page0.currentRPage == EEPROM_MAXPAGES)
            eeprom_page0.currentRPage = 1;
    }
    // write away page 0 to page 0.
    EEPROM_writePage0();

    currentFlashBuffer = alternateBuffer();

    return eeprom_page0.currentWPage;
}


void EEPROM_writeData(unsigned char *buffer, unsigned char len)
{
    EEPROM_ADDRESS3 address;

    // get semaphore if required
    unsigned availableSpace = EEPROM_PAGESIZE - eeprom_page0.lastByte;

    SETADDRESS3_256(address,eeprom_page0.currentWPage,eeprom_page0.lastByte)


    if(len < availableSpace)
    {
        // The current data fits into the space in the current page.
        //Atomic Transaction
        EEPROM_writeBuffer(currentFlashBuffer,&address,buffer,len);

        // update the index position
        eeprom_page0.lastByte += len;
    }
    else
    {
        // The current data does not fit into the current page
        // so we must finish the current page and start the next one
        // Atomic Transaction
        EEPROM_writeBuffer(currentFlashBuffer, &address,buffer,availableSpace);

        // Flush the current page and set up a new page
        EEPROM_newPage();

        // Now write the remainder of the buffer
        SETADDRESS3_256(address,eeprom_page0.currentWPage,eeprom_page0.lastByte)
        unsigned char overflow = len - availableSpace;
        if(overflow > 0)
        {
            EEPROM_writeBuffer(currentFlashBuffer,&address,&buffer[availableSpace],overflow);
            eeprom_page0.lastByte = overflow;
        }
    }
}