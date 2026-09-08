/**
  Generated Pin Manager File

  Company:
    Microchip Technology Inc.

  File Name:
    pin_manager.c

  Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for pin APIs for all pins selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC18F86J60
        Driver Version    :  2.0
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.36 and above
        MPLAB             :  MPLAB X 6.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include "pin_manager.h"


void PIN_MANAGER_Initialize(void)
{
    /**
    LATx registers
    */
    LATE = 0x00;
    LATJ = 0x00;
    LATD = 0x00;
    LATA = 0x00;
    LATF = 0x00;
    LATB = 0x00;
    LATG = 0x00;
    LATC = 0x00;
    LATH = 0x00;

    /**
    TRISx registers
    0 is output and 1 is input
    */
    TRISE = 0x00;
    TRISF = 0x80;
    TRISA = 0x3F;
    TRISG = 0x00;
    TRISB = 0xFF;
    TRISH = 0x00;
    TRISC = 0xBF;
    TRISD = 0x03;
    TRISJ = 0x30;
    
    /**
    PCFG setting
    */   
    // et the PCFG bits to 0x0A (0b1010)
    // Maps AN0-AN4 to Analog, and leaves everything else Digital.
    ADCON1bits.PCFG = 0x0A; 

    // Select Analog vs. Digital via ADCON1 (PCFG3:PCFG0)
    // Value 0x0A (0b00001010) sets AN0, AN1, AN2, AN3, and AN4 as Analog.
    // It also keeps internal voltage references linked to VDD and VSS.
    ADCON1 = 0x0A; 

    // Configure Alignment, Acquisition Time, and Clock (ADCON2)
    // Right-Justified (bit 7 = 1)
    // Acquisition Time: 12 TAD (bits 5-3 = 100)
    // ADC Conversion Clock: FOSC/32 (bits 2-0 = 010)
    ADCON2 = 0xA2; 

    // Power up the ADC Module
    ADCON0bits.ADON = 1; 
   
}

void PIN_MANAGER_IOC(void)
{

}

/**
 End of File
*/
