/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>        /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>       /* HiTech General Include File */
#elif defined(__18XX)
    #include <p18cxxx.h>   /* C18 General Include File */
#endif
#include "p18f47j53.h"

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */
#endif

#include <delays.h>
#include "timer.h"

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
#include "delays.h"
#include "Switch_Input.h"

#include "lowlevelmacros.h"
#include "led.h"
#include "eeprom_iface.h"
#include "rtc_iface.h"
#include "nav_iface.h"
#include "v_measurement.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
/** D E C L A R A T I O N S *******************************************/
// declare constant data in program memory starting at address 0x180



/*#pragma udata

#pragma code
 */

#define BITTEST 1000


// Program Specific LED Configuration
#define STARTLED_ON LED3_ON
#define STOPLED_OFF LED3_OFF

#define OVERFLOWLED_ON LED1_ON
#define OVERFLOWLED_OFF LED1_OFF

#define MAXTESTMODES 6

unsigned char buff1[EEPROM_PAGESIZE];
unsigned char buff2[EEPROM_PAGESIZE];

// TODO LIST
// TODO:  Replace the delay function with an interrupt follower.

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/



void notifyTestMode(unsigned char mode)
{
    CLEARALLLEDS
    flashLEDN(ALLLEDSMASK,2,2);
    Delay1KTCYx(255);
    mode = mode << 1;
    flashLEDN(mode,5,2);
    CLEARALLLEDS
}



#define DEBOUNCE_DELAY 30
unsigned char getSwitch(void)
{
    if (getOverFlow())
    {
        _delay(DEBOUNCE_DELAY);
        return getOverFlow();
    }
    return 0;
}

unsigned char incMode(unsigned char mode)
{
    mode++;
    if(mode >= MAXTESTMODES)
        mode = 0;
    return mode;
}

unsigned char getMode(unsigned char mode)
{
    CLEARALLLEDS
    DISPLAYHEXDIGIT(mode)

    unsigned int starttick = secondsTick;

    while((secondsTick - starttick) < 4)
    {
        if(getSwitch())
        {
            mode = incMode(mode);
            CLEARALLLEDS
            DISPLAYHEXDIGIT(mode)
            starttick = secondsTick;
            while (getSwitch())
            {}
        }
    }
    return mode;
}



//===========================================================//
//                    Test Functions
//===========================================================//



void initialiseOverFlowTest(void)
{
    CLEARALLLEDS
}

unsigned char OverFlowTest(void)
{
    initialiseOverFlowTest();
    if(getOverFlow())
    {
        flashLEDN( ALLLEDSONMASK,1,2);
    }
    return SUCCESS;
}

void initialiseADCTest(void)
{
}

unsigned char ADCTest(void)
{
    initialiseADCTest();
   unsigned int result = getADC();
   if (result < 256)
       LED1_ON
    else
    if(result < 512)
        LED2_ON
    else
        LED3_ON
    return SUCCESS;
}

void initialiseMEMTraceTest(void)
{
    MEM_CS_HI;
    MEM_RST_HI;
}

unsigned char MEMTraceTest(void)
{
    initialiseMEMTraceTest();

    for(int n=0; n < BITTEST; n++)
    {
        MEM_CS_LO           // Active low remember
        MEM_CS_HI
    }

    for(int n=0; n < BITTEST; n++)
    {
        MEM_RST_LO
        MEM_RST_HI
    }
    return SUCCESS;
}

void initialiseEEPROMWriteBufferTest(void)
{
    // Just generate it here for the time being.
    for(unsigned n = 0; n< EEPROM_PAGESIZE;n++)
    {
        buff1[n] = n;
        buff2[n] = 0;
    }
}
unsigned char compareBuffers(unsigned char length)
{
    for(unsigned n = 0; n < length; n++)
    {
        if (buff1[n] != buff2[n])
            return ERROR;
    }
    return SUCCESS;
}

unsigned char EEPROMWriteBufferTest(void)
{
    EEPROM_ADDRESS3 address;

    initialiseEEPROMWriteBufferTest();

    SETADDRESS3_256(address,0,0)  // use page 0

    // read the buffer first so that the uninitialised data is visible

    EEPROM_writeBuffer(EEPROM_BUFFER1,&address, &buff1,EEPROM_PAGESIZE);

    EEPROM_readBuffer(EEPROM_BUFFER1, (EEPROM_pADDRESS3)&address, &buff2,EEPROM_PAGESIZE);
    return compareBuffers(EEPROM_PAGESIZE);
}

void initialiseEEPROMReadWriteFlash(void)
{
    for(unsigned n = 0; n<EEPROM_PAGESIZE;n++)
    {
        buff1[n] = n;
        buff2[n] = 0;
    }
}

#define FLASHITERATIONS 9

unsigned char EEPROMReadWriteFlashTest(void)
{
    initialiseEEPROMReadWriteFlash();

    unsigned int n = 0;
    for(n = 0; n < FLASHITERATIONS; n++) // 8 to start with
    {
        EEPROM_writeData(buff1,128);
    }
    EEPROM_ADDRESS3 address;
    SETADDRESS3_256(address,1,0);
//    EEPROM_storeBuffer(1,&address);
//    EEPROM_readFlash(&address, &buff2,128);
//        if (compareBuffers(128) == ERROR)
//            return ERROR;

    for(n = 0; n< FLASHITERATIONS; n++)
    {
        unsigned t = (n << 7)+256;
        SETADDRESS3_256(address,(unsigned char)(((t)&0xFF00)>>8),
                            (unsigned char)((t)&0x00FF));
        EEPROM_readFlash(&address, &buff2,128);
        if (compareBuffers(128) == ERROR)
            return ERROR;
    }
    return SUCCESS;
}

void initialiseRTCTest(void)
{
    
}


unsigned char I2CRTCTest(void)
{
    UTC reg_out, reg_in;
    // 15-03-12 - 23:50:00
    reg_out.sec = 00;
    reg_out.min = 50;
    reg_out.hrs = 23;
    reg_out.dte = 01;
    reg_out.mth = 03;
    reg_out.yrs = 15;
    //reg_out.ms = 0;

    RTCgetTime(&reg_in);
    if(reg_in.hrs != reg_out.hrs)
    {
        RTCsetTime(&reg_out);
    }

    for(unsigned n = 0; n < 10; n++)
    {
        RTCgetTime(&reg_in);
         if(reg_out.min == reg_in.min)
             flashLEDSuccess();
         else
         {
             flashLEDFailure();
             return ERROR;
         }
    }
    return SUCCESS;
}


void initialiseNAVTest(void)
{

}

unsigned char I2CNAVTest(void)
{
    unsigned result = 0;
    unsigned aggregate = true;
    unsigned char gyrin[3];
    unsigned char accin[3];
    initialiseNAVTest();
    for (unsigned n = 0; n < 20000 ; n++)
    {
        gyrin[0] = gyrin[1] = gyrin[2] = 0;
        accin[0] = accin[1] = accin[2] = 0;
        result = NAVReadRegister(SELF_TEST_X_GYRO, (unsigned char *)&gyrin[0], sizeof(unsigned char));
        result = NAVReadRegister(SELF_TEST_Y_GYRO, (unsigned char *)&gyrin[1], sizeof(unsigned char));
        result = NAVReadRegister(SELF_TEST_Z_GYRO, (unsigned char *)&gyrin[2], sizeof(unsigned char));
        result = NAVReadRegister(SELF_TEST_X_ACCEL, (unsigned char *)&accin[0], sizeof(unsigned char));
        result = NAVReadRegister(SELF_TEST_Y_ACCEL, (unsigned char *)&accin[1], sizeof(unsigned char));
        result = NAVReadRegister(SELF_TEST_Z_ACCEL, (unsigned char *)&accin[2], sizeof(unsigned char));

        // Check the returned values if we had a failure one would not be set.
        if(((gyrin[0] != 0xC6) && (gyrin[0] != 0xBD)) ||
           ((gyrin[1] != 0xCD) && (gyrin[1] != 0xC4)) ||
           ((gyrin[2] != 0xE0) && (gyrin[2] != 0xE6)) ||
           ((accin[0] != 0x63) && (accin[0] != 0x68)) ||
           ((accin[1] != 0x5F) && (accin[1] != 0x63)) ||
           ((accin[2] != 0x80) && (accin[2] != 0x6E)))
            return ERROR;
    }
    return SUCCESS;
}





void initialiseApplication(void)
{
    // Configure all pins
    configurePINS();

    // Configure PPS for SPI2 and ...
//    configurePPS();

    // Set up the SPI 2 module
    configureSPI();

    // Setup the I2C Module
    configureI2C();

    // Setup the ADC
    configureADC();

    // Set up the timers
    configureTimers();

    /* Initialize peripherals */
    ENABLESPI1
    EEPROM_initialise();

    enableInterupts();


}


void main(void)
{
    unsigned char mode = 3;
    unsigned char result = 0;

    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize I/O and Peripherals for application */
    initialiseApplication();
    startLEDSequence(5);
    CLEARALLLEDS

    while (1)
    {
        mode = getMode(mode);
        unsigned starttick = secondsTick;
        while (!getOverFlow() && ((secondsTick - starttick) < 20 ))
        {
            startLEDSequence(1); // Wait until we have pushed the button.
        }

        switch (mode)
        {
        // Puts out a square wave on the MEM CS and RST lines
        case 0: result = MEMTraceTest();
            break;
        // If returned result < 256, flash led 1  result < 512 led 2 else led 3
        case 1: result = ADCTest();
            break;
        // Writes values to buffer 1 then seaves to page 1 the reads back buffer and comparews to original data.
        case 2: result = EEPROMWriteBufferTest();
            break;
        case 3: result = EEPROMReadWriteFlashTest();
            break;
        case 4: result = I2CRTCTest();
            break;
        case 5: result = I2CNAVTest();
            break;
        }
        if(result == SUCCESS)
            flashLEDSuccess();
        else
            flashLEDFailure();
       // mode = incMode(mode);  // automatically move to the next test
    }
}



// Timer 2 is used in PWM mode.

//"Rule of thumb: Always read inputs from PORTx and write outputs to LATx. If you need to read what you set an output to, read LATx."

/*
 * Timer 0 - 10Hz interupt
 * Timer 1 - 1 second interupt
 * Timer 2 - ?
 *
 * At Power on:
 *      Load in configuration information from page 0 of the memory
 *      Check if the RTC has a value if so use it.
 *          If not add an error entry to the memory buffer
 *
 * Every 1/128th second
 *      Finish a power check
 *      if state is normal
 *          if power is outside tolerance
 *              state is transient
 *              write voltage to transient buffer
 *              increment detections
 *          else // voltage is inside tolerance
 *              bin the results
 *      else if state is transient
 *          if power is outside tolerance and <= 100 detections
 *              write to transient memory
 *              increment detections
 *          if power is within tolerance
 *              write time of transient complete
 *              queue transient to flash
 *      start next power check
 * Every 1 second:
 *      write the last seconds average voltage level to flash
 *      if there has been a transiet write the voltage signature to flash
 *      write the current buffer to memory
 * Every 5 seconds:
 *      get positional data and write to buffer
 *      check to see if a USB stick or smart device has been inserted into the USB
 *      If so, change modes as required
 * Every 10 seconds:
 *      Ensure anything that the page 0 has been written if it has been updated.
 * Every 600 seconds
 *      recalibrate the time from the RTC
 */