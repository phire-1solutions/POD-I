/* 
 * File:   EEPROM.h
 * Author: phamilt5
 *
 * Created on February 25, 2015, 12:23 PM
 */

#ifndef EEPROM_H
#define	EEPROM_H

#include "p18f47j53.h"
#include "Application.h"
#include "RTC_iface.h"
#include "spi_iface.h"

/*
 * Expected usages
 * Host processor
 * -  status information is stored in page 0 of the flash. data stored is:
 *      - current page being written
 *      - next page to be read  (forms a continuous buffer)
 *      - time of accumulated date (starts at 0 grows to a number and stays at
 *        that number. )
 * -  Total Hours minutes and seconds of execution time
 * - Total identified anomalys
 * -  accumulates data for writing to the flash 16 bytes at a time
 *
 */


#define EEPROM_MAXPAGES 32768
#define EEPROM_PAGESIZE 256
#define EEPROM_BUFFER1                  1
#define EEPROM_BUFFER2                  2


#define incRoll(value) { value++; if(value == EEPROM_MAXPAGES) value = 1; }

typedef struct
{
    char            signature[8]; // PHire-1
    unsigned int    version;      // Version number of the software
    unsigned int    currentWPage; // Page being written
    unsigned int    currentRPage; // last Page Read
    unsigned char   lastByte;     // last byte written
    unsigned int    lastRPage;    // LastPage read
    UTC           lastRunTm;    // Duration of last Flight
    UTC           startTm;      // Commencement time of this measurement
    UTC           inServiceTm;  // Total amount of time that the unit has
                                  //  been in service
} sPage0;
typedef sPage0 *pPage0;

// OPCODES
#define EEPROM_STATUS_OPC               0xD7
#define EEPROM_MANUFACTURERSID_OPC      0x9F


// Following opcodes are for page size 256 and 254 bytes
// Write a variable size block of data to a specified buffer
// 256 0x84(0x87) + 16 dummy bits + 8 buffer address bits
// 264 0x84(0x87) + 15 dummy bits + 9 buffer address bits
#define EEPROM_BUFFER1_WRITE            0x84
#define EEPROM_BUFFER2_WRITE            0x87

// Following Opcodes are low speed opcodes usable up to 50MHz
// Following opcodes are for page size 256 and 264 bytes
// Read back a variable size block of data from a specified buffer
// 256 0xD1(0xD3) + 16 dummy bits + 8 buffer address bits. Then 1 dummy byte
// 264 0xD1(0xD3) + 15 dummy bits + 9 buffer address bits. Then 1 dummy byte
#define EEPROM_BUFFER1_READ_SLOW        0xD1
#define EEPROM_BUFFER2_READ_SLOW        0xD3

// Following opcodes are for page size 256 and 264 bytes
// Store a specified buffer into a specified page
// 256 0x83(0x86) 1 dummy bit + 15 bits page address bits + 8 dummy bits
// 264 0x83(0x86) 15 bits page address bits + 9 dummy bits
#define EEPROM_BUFFER1_STORE             0x83
#define EEPROM_BUFFER2_STORE             0x86

// Following Opcodes are low speed opcodes usable up to 50MHz
// Read a variable size block of data back from a page via specified buffer
// 256 0x82(0x85) + 1 dummy bit + 15 page address bits + 8 buffer address bits
// 264 0x82(0x85) + 15 page address bits + 9 buffer address bits
// WARNING The entire page is written when CS line goes high.
//     Dont think you are writing just the data that has been sent.
#define EEPROM_WRITE_B1_FLASH              0x82
#define EEPROM_WRITE_B2_FLASH              0x85

// Following Opcode is used for both 256 and 268 byte pages
// Format for both is 0xD2 + 3 bytes address + 4 bytes dummy
// 268 Address is 15 bits page address + 9 bits byte offset + 4 dummy bytes
// 256 Address is 1 dummy bit 0 + 15 bits page address + 8 bits byte offset + 4 dummy bytes
#define EEPROM_READ_FLASH               0xD2




typedef union { // For Addresses 3 Bytes long
//    struct {  // This structure is used for addressing pages for 256 Bytes.
//        unsigned ABlank1               :1;  // Dummy bit
//        unsigned APage7                :7;  // 15 bit page number
//        unsigned APage8                :8;
//        unsigned AOffset8              :8;  // 8 bit byte offset
//    };
//    struct {  // This structure is used for addressing pages for 268 Bytes.
//        unsigned BPage15               :15; // 15 bit page number
//        unsigned BOffset9              :9;  // 9 bit byte offset
//    };
    struct { // Unsigned Char access
        unsigned char buc1            :8;  // Byte 1
        unsigned char buc2            :8;  // Byte 2
        unsigned char buc3            :8;  // Byte 3
    };
} EEPROM_ADDRESS3;
typedef EEPROM_ADDRESS3 *EEPROM_pADDRESS3;

#define SETADDRESS3_256(Adr,page,offset) { \
                                Adr.buc1=(unsigned char)((page&0x7f00)>>8); \
                                Adr.buc2 = (unsigned char)(page&0x00FF),\
                                Adr.buc3=offset; };

#define SETADDRESS3_264(Adr,page,offset) { \
                                Adr.buc1=(unsigned char)((page& 0xFF00)>>8); \
                                Adr.buc2=(unsigned char)(page&0x00FF), \
                                Adr.BOffset9=offset; };

#define CLEARADDRESS3(Adr) { Adr.buc1 = 0x00; Adr.buc2 = 0x00; \
                            Adr.buc3 = 0x00; };

#define SETADDRESS_4(Adr, b1,b2,b3,b4) { Adr.buc1 = b1; Adr.buc2 = b2; \
                            Adr.buc3 = b3; Adr.buc4 = b4; }

#define PAGEADDRESS256(Adr, c1) { Adr.CPAHiBit = 0; Adr.CPAPage = c1; \
                            Adr.CPABlank = 0x0000; }


#define EEPROM_RST_TRIS TRISDbits.TRISD4
#define EEPROM_CS_TRIS  TRISDbits.TRISD5

#define EEPROM_RST_LAT LATDbits.LATD4
#define EEPROM_CS_LAT  LATDbits.LATD5


typedef union {
    struct {
        unsigned PageSize   :1; // bit 0
        unsigned Protect    :1; // bit 1
        unsigned Density    :4; // bit 2-5
        unsigned CompResult :1; // bit 6
        unsigned Rdy_Busy   :1; // bit 7
    };
    unsigned char           b1;
}EEPROM_Status_byte1;

typedef union {
struct {
        unsigned EraseSusp  :1; // bit 0
        unsigned PgmSusp1   :1; // bit 1
        unsigned PgmSusp2   :1; // bit 2
        unsigned SectLockEN :1; // bit 3
        unsigned Reserved1  :1; // bit 4
        unsigned ErasePgmErr:1; // bit 5
        unsigned Reserved2  :1; // bit 6
        unsigned Rdy_Busy   :1; // bit 7
    };
    unsigned char           b2;
}EEPROM_Status_byte2;



/******************************************************************************
* Macro: SETMEMRST                                                            *
* Preconditions: none                                                         *
* Overview: This macro sets or RSTs the MEM_RST Port                          *
* Input: value to set to 0 or 1                                               *
* Output: None.                                                               *
*                                                                             *
******************************************************************************/
#define SETMEMRST(u) EEPROM_RST_LAT = u;

/******************************************************************************
* Macro: MEM_RST_HI                                                           *
* Preconditions: none                                                         *
* Overview: This macro sets Memory Rest Line to logic 1                       *
* Input: None.                                                                *
* Output: None.                                                               *
*                                                                             *
******************************************************************************/
#define MEM_RST_HI SETMEMRST(1)

/******************************************************************************
* Macro: MEM_RST_LO                                                           *
* Preconditions: none                                                         *
* Overview: This macro sets Memory Rest Line to logic 0                       *
* Input: None.                                                                *
* Output: None.                                                               *
*                                                                             *
******************************************************************************/
#define MEM_RST_LO SETMEMRST(0)

/******************************************************************************
* Macro: SETMEMCS                                                             *
* Preconditions: none                                                         *
* Overview: This macro sets or RSTs the MEM_RST Port                          *
* Input: value to set to 0 or 1                                               *
* Output: None.                                                               *
*                                                                             *
******************************************************************************/
#define SETMEMCS(u) EEPROM_CS_LAT = u;

/******************************************************************************
* Macro: MEM_CS_HI                                                            *
* Preconditions: none                                                         *
* Overview: This macro sets Memory CS Line to logic 1                         *
* Input: None.                                                                *
* Output: None.                                                               *
*                                                                             *
******************************************************************************/
#define MEM_CS_HI SETMEMCS(1)

/******************************************************************************
* Macro: MEM_CS_LO                                                            *
* Preconditions: none                                                         *
* Overview: This macro sets Memory CS Line to logic 0                         *
* Input: None.                                                                *
* Output: None.                                                               *
*                                                                             *
******************************************************************************/
#define MEM_CS_LO SETMEMCS(0)


/* ****************************************************************************
 * Function: EEPROM_writeBuffer                                               *
 * Writes memory block pointed to by buffer of length len to the FLASH buffer *
 * addressed by buf selector ( 1 or 0) commencing at the position in the      *
 * buffer pointed to by address.                                              *
 * Inputs:                                                                    *
 *  unsigned char bufselector                                                 *
 *  EEPROM_BUFFADDX2 *address                                                 *
 *  unsigned char *buffer                                                     *
 *  unsigned len                                                              *
 * Returns:void                                                               *
 * Atomic SPI Bus Transaction                                                 *
 * ***************************************************************************/
void EEPROM_writeBuffer(unsigned char bufselector, EEPROM_pADDRESS3 paddress, unsigned char *buffer, unsigned len);

/* ****************************************************************************
 * Function: EEPROM_readBuffer                                                *
 * Returns memory block pointed to by address of length len from the FLASH    *
 * buffer addressed by buf selector ( 1 or 0) commencing at the position in   *
 * the buffer pointed to by address.                                          *
 * Inputs:                                                                    *
 *  unsigned char bufselector                                                 *
 *  EEPROM_BUFFADDX2 *address                                                 *
 *  unsigned char *buffer                                                     *
 *  unsigned len                                                              *
 * Returns:void + buffer filled for len.                                      *
 * Atomic SPI Bus Transaction                                                 *
 * ***************************************************************************/
void EEPROM_readBuffer(unsigned char bufselector, EEPROM_pADDRESS3 paddress, unsigned char *buffer, unsigned len);

/* ****************************************************************************
 * Function: EEPROM_storeBuffer                                               *
 * Transfers the buffer pointed to by bufselector (1 or 0) to the FLASH page  *
 * pointed to by address                                                      *
 * Inputs:                                                                    *
 *  unsigned char bufselector                                                 *
 *  EEPROM_BUFFADDX2 *address                                                 *
 * Returns:void                                                               *
 * Atomic SPI Bus Transaction                                                 *
* ***************************************************************************/
void EEPROM_storeBuffer(unsigned char bufselector,EEPROM_pADDRESS3 paddress);

/* ****************************************************************************
 * Function: EEPROM_writeFlash                                                *
 * Writes the memory block pointed to by buffer of length len to the Flash    *
 * via the buffer bufselector commencing at address                           *
 * Inputs:                                                                    *
 *  unsigned char bufselector                                                 *
 *  EEPROM_BUFFADDX2 *address                                                 *
 *  unsigned char *buffer                                                     *
 *  unsigned len                                                              *
 * Returns:void + buffer filled for len.                                      *
 * Atomic SPI Bus Transaction                                                 *
 * ***************************************************************************/
void EEPROM_writeFlash(unsigned char bufselector, EEPROM_pADDRESS3 paddress, unsigned char *buffer, unsigned len);

/* ****************************************************************************
 * Function: EEPROM_readFlash                                                 *
 * Reads into the memory block pointed to by buffer a length of len from the  *
 * Flash commencing at address                                                *
 * Inputs:                                                                    *
 *  EEPROM_BUFFADDX2 *address                                                 *
 *  unsigned char *buffer                                                     *
 *  unsigned len                                                              *
 * Returns:void + buffer filled for len.                                      *
 * Atomic SPI Bus Transaction                                                 *
 * ***************************************************************************/
void EEPROM_readFlash(EEPROM_pADDRESS3 paddress, unsigned char *buffer, unsigned len);

/* ****************************************************************************
 * Function: EEPROM_eraseChip                                                 *
 * Clears the chip completely.  Remember that this can only be done a         *
 * Flash commencing at address                                                *
 * Inputs:                                                                    *
 *  EEPROM_BUFFADDX2 *address                                                 *
 *  unsigned char *buffer                                                     *
 *  unsigned len                                                              *
 * Returns:void + buffer filled for len.                                      *
 * Atomic SPI Bus Transaction                                                 *
 * ***************************************************************************/
void EEPROM_eraseChip(void);

/* ****************************************************************************
 * Function: EEPROM_getManufacturerId                                         *
 * returns requested byte based on the index from the manufacturers Id        *
 * Inputs:                                                                    *
 *  unsigned char index (0-5)                                                 *
 * Returns:void + buffer filled for len.                                      *
 * Atomic SPI Bus Transaction                                                 *
 * ***************************************************************************/
unsigned char EEPROM_getManufacturerId(unsigned char index);


/* ****************************************************************************
 * Functrion: EEPROM_readPage0                                                *
 * Loads Page 0 into a standard structure in memory                           *
 * inputs void                                                                *
 * returns void                                                               *
 * Atomic SPI Bus Transaction                                                 *
 * ***************************************************************************/
void EEPROM_readPage0(void);

/* ****************************************************************************
 * Functrion: EEPROM_writePage0                                               *
 * Writes Page 0 into the flash memory                                        *
 * inputs void                                                                *
 * returns void                                                               *
 * Atomic SPI Bus Transaction                                                 *
 * ***************************************************************************/
void EEPROM_writePage0(void);


/* ****************************************************************************
 * Functrion: EEPROM_initialise                                               *
 * performs all initialisation functions for the EEPROM                       *
 * inputs void                                                                *
 * returns void                                                               *
 * Incorporates Atomic SPI Bus Transactions                                   *
 * ***************************************************************************/
void EEPROM_initialise(void);

/* ****************************************************************************
 * Function: EEPROM_nextWritePage                                             *
 * Returns the next page number to be used if there is an overflow, it        *
 * resatarts at page 1,skipping page 0. The page 0 references are updated.    *
 * Inputs void                                                                *
 * Returns unsigned pageNumber                                                *
 * ***************************************************************************/
//unsigned EEPROM_nextWritePage(void);

/******************************************************************************
 * Function: EEPROM_newPage                                                   *
 * Sets up to write the next page of the flash                                *
 * skipping page 0. The page 0 references are updated.                        *
 * Inputs void                                                                *
 * Returns unsigned pageNumber                                                *
 * ***************************************************************************/
unsigned EEPROM_newPage(void);


/******************************************************************************
 * Function: EEPROM_writeData                                                 *
 * write up to 256 bytes to the flash buffer for subsequent write to page     *
 * Inputs                                                                     *
 *   unsigned char *buffer // up to 2566 buttes fof data                      *
 *   unsigned char len // the length of the data buffer                       *
 * Returns void                                                               *
 * ***************************************************************************/
void EEPROM_writeData(unsigned char *buffer, unsigned char len);


/******************************************************************************
 * Function: EEPROM_initialisePage0                                           *
 * clears page 0 to defaults                                                  *
 * Inputs void                                                                *
 * Returns void                                                               *
 * ***************************************************************************/
void EEPROM_initialisePage0(void);

#endif	/* EEPROM_H */

