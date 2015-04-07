/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#elif defined(__18CXX)
    #include "p18f47j53.h"
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#endif
#include "timer.h"
#include "spi_iface.h"
#include "i2c_iface.h"
#include "eeprom_iface.h"
#include "user.h"
#include "led.h"
/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */


// What speed do we want to get ADCs processed
// Operation  ADCS    10-bit MHz  12-bit MHz
//    2 TOSC    000     2.86        2.5
//    4 TOSC    100     5.71        5
//    8 TOSC    001     11.43       10
//    16 TOSC   101     22.86       20
//    32 TOSC   010     45.71       40
//    64 TOSC   110     48          48
//    RC        011     1           1

#define ADCS_2_86_MHZ  (0b000)
#define ADCS_5_71_MHZ  (0b100)
#define ADCS_11_43_MHZ (0b001)
#define ADCS_22_86_MHZ (0b101)
#define ADCS_45_71_MHZ (0b010)
#define ADCS_48_MHZ    (0b110)
#define ADCS_RC_MHZ    (0b011)

void configureADC(void)
{
    // ADC Configuration
    ANCON0 = 0b00010000;        // Configure AN5 (RA5) as Analog
    ANCON1 = 0b00010000;        // Configure AN12(RB0) as Digital
    ADCON0bits.ADON = 1;        // ADCON0 Bit 0 ADC Enabled
    // ADCON0bits.GODONE;       // ADCON0 Bit 1 Status Bit
    ADCON0bits.CHS = 0b0100;    // ADCON0 Bit 2-5 Configure for AN5
    ADCON0bits.VCFG = 0b00;     // ADCON0 Bit 6 VCFG0 Voltage Reference bit  0 = AVDD 1 = VREF+ (AN3)
                                // ADCON0 Bit 7 VCFG1 Voltage Reference bit  0 = AVSS 1 = VREF- (AN2)
    ADCON1bits.ADCS = ADCS_5_71_MHZ;    // Speed of ADC
    ADCON1bits.ACQT = 0b000;    // 0 TAD
    ADCON1bits.ADCAL = 0;       // Dont Calibrate
    ADCON1bits.ADFM = 1;        // Right Justified

    // Configure interupts for ADC
    PIR1bits.ADIF = 0;          // AD Conversion is not complete.
    PIE1bits.ADIE = 0;          // Disable interupts
    INTCONbits.PEIE = 0;        // Disable all peripheral Interupts
    INTCONbits.GIE = 0;         // Disable all interupts

    //INTCON2bits.RBPU = 0; // enable PORTB internal pullups
    //WPUBbits.WPUB0 = 1; // enable pull up on RB0
    //ANSELH = 0b00001111; // AN8-12 are digital inputs (AN12 on RB0)
    //TRISBbits.TRISB0 = 1; // PORTB bit 0 (connected to switch) is input (1)
}


/*
;===================================================
; Unlock Registers
;===================================================

; BCF f, b, a   Bit Clear f, b, a
; BSF f, b, a   Bit Set   f, b, a
; MOVLB k       Move literal to BSR
; MOVLW kk      Move Literal to WREG
; MOVWF f, a    Move WREG to f

asm("MOVLB   0x0E")             //; Bank Select Register (BSR) points to Bank 14
asm("BCF     INTCON, GIE")      //; Disable Interupts
asm("MOVLW   0x55")             //; Setup for 1st unlock code
asm("MOVWF   EECON2, 0")        //; Place into EECON2
asm("MOVLW   0xAA")             //; Setup for 2nd unlock code
asm("MOVWF   EECON2, 0")        //; Place into EECON2

//; Turn Off PPS Write Protection
asm("BCF     PPSCON, IOLOCK, BANKED") //
*/

//void configurePPS(void)
//{
//    asm("GIE        equ     0x07");
//    asm("IOLOCK     equ     0x00");
//    asm("MOVLB   0x0E");             //; Bank Select Register (BSR) points to Bank 14
//    asm("BCF     INTCON, GIE");      //; Disable Interupts GIE in bit 7
//    asm("MOVLW   0x55");             //; 1st unlock code
//    asm("MOVWF   EECON2, 0");
//    asm("MOVLW   0xAA");             //; 2nd unlock code
//    asm("MOVWF   EECON2, 0");
//    // ; Now Turn Off PPS Write Protection
//    asm("BCF     PPSCON, IOLOCK, 0"); // Turn off IOLOCK  IOLOCK is bit 0
//
//    // RPINR21 SDI2 SPI2 Data Input
//    asm("MOVLW   0x08");  // Set WREG to Pin RPINR21 RP8
//    asm("MOVWF   RPINR21, 1"); // Write to RP21
//
//    // RPINR22 SCK2IN SPI2 Clock Input
//    asm("MOVLW   0x07");  // Set WREG to RP
//    asm("MOVWF   RPINR22, 1");
//
//    // RPINR23 SS2IN SPI2 Slave Select Input
//    // asm("MOVLW   0x0X");  // Set WREG to RP8X
//    // asm("MOVWF   RPINR23, 0");
//
//    // Outputs
//    // RPOR18 SDO2 SPI2 SDO
//    asm("MOVLW   0x0A");  // Set WREG to SDO2 Function - Table 10.4 - 10
//    asm("MOVWF   RPOR18, 1"); // Write to Pin RP 18
//
//    // RPOR07 SCK2 SPI SCK
//    asm("MOVLW   0x0B");  // Set WREG to SDO2 Function - Table 10.4 - 11
//    asm("MOVWF   RPOR7, 1"); // Write to Pin RP 7
//
//    // Lock the PPS
//    asm("BCF     INTCON, GIE");      //; Enable Interupts GIE in bit 7
//    asm("MOVLW   0x55");             //; Setup for 1st unlock code
//    asm("MOVWF   EECON2, 0");        //; Place into EECON2
//    asm("MOVLW   0xAA");             //; Setup for 2nd unlock code
//    asm("MOVWF   EECON2, 0");        //; Place into EECON2
//    // ; Turn Off PPS Write Protection
//    asm("BSF     PPSCON, IOLOCK, 1"); // Turn on IOLOCK IOLOCK is bit 0
//}

void configureSPI(void)
{
    // ?  Initialize the SCKx/LAT value to the desired Idle SCKx level  // Nothing required
    // ?  Initialize the SCKx/TRIS bit as output (Master mode) or input (Slave mode) // See configurePINS below. TRISB pin 4
    // ?  Initialize the SDIx/PCFGx bit (if SDIx is multiplexed with the ANx function) // not multiplexing that pin.
    // ?  Initialize the SDIx/TRIS bit // See configurePINS below. TRISB pin 5
    // ?  Initialize the SSx/PCFG bit (if in Slave mode and multiplexed the with ANx function) // In Master Mode not sharing the port
    //?  Initialize the SSx/TRIS bit (Slave modes) // In master mode.
    // ?  Initialize the SDOx/TRIS bit // See configurePINS below. TRISC pin 7
    // ?  Initialize the SSP1STAT register
    // Bit 0 - BF: Buffer Full Status Bit 1 = full 0 = empty
    // Bit 1 - UA: Update Address bit I2C Only
    // Bit 2 - R/W: Read/Write Information bit I2C Only
    // Bit 3 - S: Start bit I2C Only
    // Bit 4 - P: Stop bit I2C Only
    // Bit 5 - D/A Data/Address bit I2C Only
    // Bit 6 - CKE: SPI Select Bit 1 = Txmit on Active to Idle Transition. 0 = Txmit on Idle to Active Transition
    // Bit 7 - SMP: Sample Bit In SPI Master Mode 1 = Input data sampled at end of data output time.
    //                                            0 = Input data sampled at middle of data output time
    SSP1STATbits.SMP = 0;
    SSP1STATbits.CKE = 0;

    // ?  Initialize the SSP1CON1 register
    // Bit 0-3 - SSPM: Master Slave Serial Port Select bits
        // 0101 = SPI Slave mode, clock = SCKx pin; SSx pin control disabled, SSx can be used as I/O pin
        // 0100 = SPI Slave mode, clock = SCKx pin; SSx pin control enabled
        // 0011 = SPI Master mode, clock = TMR2 output/2
        // 0010 = SPI Master mode, clock = FOSC/64
        // 0001 = SPI Master mode, clock = FOSC/16
        // 1010 = SPI Master mode, clock = FOSC/8
        // 0000 = SPI Master mode, clock = FOSC/4
    // Bit 4 - CKP: Clock Polarity Select bit
        // 1 = Idle state for clock is high level
        // 0 = Idle state for clock is low level
    // Bit 5 - SSPEN: Master Synchronous Serial Port Enable
        // 1 - Enables SCK2 SDO2 SDI2 as serial port pins
        // 0 = Disables serial port and confires pins as i/o pins
    // Bit 6 - SSPOV: Receive Overflow Indicator bit
        // Slave mode only when we are the receipient.
    // Bit 7 - WCOL: Write Collision Detection bit
        // 1 = The SSPxBUF register is still transmitting the previous word (must be cleared in software)
        // No collision
    SSP1CON1bits.SSPM = 0b0000; // FOSC/4
    SSP1CON1bits.CKP = 1;      // Set Idle = high
}


void configureI2C(void)
{
    //  0x09 - 400KHz
    //  0x0C - 308KHz
    //  0x27 - 100KHz
    
    SSP2ADDbits.SSPADD = 0x09;
    openI2C2(MASTER ,SLEW_OFF );
}


void configurePINS(void)
{
    // Tristate Pins PORTA
    TRISAbits.TRISA0 = 0;       // bit 0 output (0) N/C
    TRISAbits.TRISA1 = 0;       // bit 1 output (0) N/C
    TRISAbits.TRISA2 = 0;       // bit 2 output (0) N/C
    TRISAbits.TRISA3 = 0;       // bit 3 output (0) N/C
    //TRISAbits.TRISA4 = 0;     // Bit does not exist bit 4 output (0) N/C
    TRISAbits.TRISA5 = 1;       // bit 5 input  (1) VADC - Analog
    //TRISAbits.TRISA6 = 0;     // Set in configureOscillator bit 6 output (0) OSC1 - set in configure Oscillator
    //TRISAbits.TRISA7 = 1;     // Set in configureOscillator bit 7 input  (1) OSC2

    // Tristate Pins PORTB
    // bit 0 input  (1) Overflow
    // bit 1 output (0) Led 1
    // bit 2 output (0) Led 2
    // bit 3 output (0) Led 3
    // bit 4 output (0) SPI_SCK1
    // bit 5 input  (1) SPI_SDI1
    // bit 6 PGC
    // bit 7 PGD
    TRISB = 0b00100001;

    // Tristate Pins PORTC
    // bit 0 ???    (0) T1OSO
    // bit 1 ???    (1) T1OSI
    // bit 2 output (0) N/C
    // bit 3 output (0) Notused
    // bit 4 i/o    (0) USB_D_N
    // bit 5 i/o    (0) USB_D_P
    // bit 6 output (0) N/C
    // bit 7 output (0) SPI_SDO
    TRISC = 0b00000010;

    // Tristate Pins PORTD
    // bit 0 output (1) I2C_SCL
    // bit 1 i/o    (1) I2C_SDA
    // bit 2 input  (1) GYR_INT
    // bit 3 input  (1) MEM_INT
    // bit 4 output (0) MEM_RST
    // bit 5 output (0) MEM_CS
    // bit 6 output (0) Test output pin
    // bit 7 output (0) USB_EN
    TRISD = 0b10001111;

    MEM_RST_HI  // Resets on low
    MEM_CS_HI     // Select on Low
}


void configureTimers(void)
{
    // Prescaler 8000000 / (4 * 256 *2^16)
//    SETUPTIMER0OFFSET(xxxxxxx);         // Set up for 10.0032Hz
    littleTick = 0;
    secondsTick = 0;
//    openTimer0(TIMER_INT_ON &           // Enable Interupts
//                T0_16BIT &              // 16 Bit timer
//                T0_SOURCE_INT &         // Internal CLock
//                T0_EDGE_RISE &          // Low to High Edge
//                T0_PS_1_256 );          // Prescaler divide by 256


    SETUPTIMER1OFFSET(0xFF80)              // Configure for 256 ticks / second
//    openTimer1(0b10001110,
//                    0b00000000            // Disable the Gate function
//             );
    openTimer1(TIMER_INT_ON &
            T1_SOURCE_PINOSC &              // Use the T1OSC or T1CKI
            T1_PS_1_1 &                     // Prescaler /8
            T1_OSC1EN_ON &                  // T1OSC, not T1CKI
            T1_SYNC_EXT_OFF &               // dont use the external clock
            T1_16BIT_RW,                    // 256 bit 32k /256 = 128
            0b00000000);

}

void enableInterupts(void)
{
    /* Enable interrupts */
    /* Configure the IPEN bit (1=on) in RCON to turn on/off int priorities */
    RCONbits.IPEN = 1;              // Enable Interupt priorities
    INTCONbits.GIE = 1;             // Enable Interupts

}

