/* 
 * File:   gpio.c
 * Author: stockli
 *
 * Created on August 28, 2026, 2:14 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "mcc_generated_files/device_config.h"
#include "mcc_generated_files/mcc.h"
#include "gpio.h"
#include "common.h"
#include "udp.h"
#include "quicksort.h"

#define MAX_REG_NOOP        0x00
#define MAX_REG_DIGIT0      0x01
#define MAX_REG_DIGIT1      0x02
#define MAX_REG_DIGIT2      0x03
#define MAX_REG_DIGIT3      0x04
#define MAX_REG_DIGIT4      0x05
#define MAX_REG_DIGIT5      0x06
#define MAX_REG_DIGIT6      0x07
#define MAX_REG_DIGIT7      0x08
#define MAX_REG_DECODE      0x09
#define MAX_REG_INTENSITY   0x0A
#define MAX_REG_SCANLIMIT   0x0B
#define MAX_REG_SHUTDOWN    0x0C
#define MAX_REG_TEST        0x0F

void write_outputs(void)
{
    uint8_t i;
     
    for (i=0;i<(MAXOUTPUTS/8);i++) {
        
        if (i<4) {
            // Port E: OUTPUTS 0-31 (DO1)
            
            IO_RF5_SetHigh();
            
            LATFbits.LATF3 = i & 0x01; 
            LATFbits.LATF4 = (i >> 1) & 0x01;

            IO_RF5_SetLow();
            LATE = outputs[i];
            IO_RF5_SetHigh();
            
        } else {
            // Port H: OUTPUTS 32-63 (DO2)

            IO_RF6_SetHigh();
 
            LATFbits.LATF3 = (i-4) & 0x01; 
            LATFbits.LATF4 = ((i-4) >> 1) & 0x01;

            IO_RF6_SetLow();
            LATH = outputs[i];
            IO_RF6_SetHigh();
            
        }               
    }
}

void read_inputs(void)
{
    // Please Make sure to close jumpers JP_PGC and JP_PGD
    // Before reading inputs (double use of bit 6 and 7 of input lines)
    uint8_t i; 
            
    for (i=0;i<(MAXINPUTS/8/2);i++) {
                    
        LATFbits.LATF1 = i & 0x01; 
        LATFbits.LATF2 = (i >> 1) & 0x01;
        
        // Port B: INPUTS 0-31 (DI1)
        __delay_us(10);
        inputs[i] = PORTB;
        
        if (inputs[i] != inputs_save[i]) {
            printf("Inputs %i 0x%02X 0x%02X \n",i,inputs[i],inputs_save[i]);
        }

        // Port C: INPUTS 32-63 (DI2)
        __delay_us(10);
        
        /* UART TX/RX uses PORTC Bits 6 & 7, so turn off for 
           not having faulty input reads when debugging through UART */
        if (!DEBUG_UART) {
            inputs[i+4] = PORTC;
        }
        
        if (inputs[i+4] != inputs_save[i+4]) {
            printf("Inputs %i 0x%02X 0x%02X \n",i+4,inputs[i+4],inputs_save[i+4]);
        }           
    }
}

void read_analoginputs(void)
{
    uint8_t i;
    int8_t h;
    
    int16_t temparr[MAXSAVE];
    int16_t noise = 1;
    int16_t median;
            
    for (i=0;i<MAXANALOGINPUTS;i++) {
    //for (i=0;i<1;i++) {
         // 1. Assign the channel (Clear old CHS bits, then inject new channel)
        ADCON0bits.CHS = i; 

        // 2. Start conversion sequence
        ADCON0bits.GO_DONE = 1; 

        // 3. Poll hardware status until conversion finishes
        while (ADCON0bits.GO_DONE); 

        // 4. Return combined 10-bit result

        /* Shift History of analog inputs and update current value */
        for (h = MAXSAVE - 2; h >= 0; h--) {
           analoginputs[i][h + 1] = analoginputs[i][h];
        }
        analoginputs[i][0] = (uint16_t)(ADRESH << 8) | ADRESL;
        
        /* fill history with current read upon start */
        if (firstanalogread) {
            for (h=1;h<MAXSAVE;h++) {
                analoginputs[i][h] = analoginputs[i][0];
            }
        }

        memcpy(temparr, analoginputs[i], sizeof(analoginputs[i][0]) * MAXSAVE);
        //quicksort(temparr, 0, MAXSAVE - 1);
        sort_int16(temparr, MAXSAVE);
        if (firstanalogread) {
            median = (int16_t) analoginputs[i][0];
        } else {
            median = temparr[MAXSAVE / 2];
        }
            
        /* only send current value if it is outside median and noise */
        if ((median < ((int16_t) analoginputs_save[i] - noise)) || (median > ((int16_t) analoginputs_save[i] + noise))) {       
          printf("ANA %i 0: %i MED: %i SAV: %i \n",i, (int) analoginputs[i][0], median, analoginputs_save[i]); 
          analoginputs_median[i] = (uint16_t) median;
        }
    }
    firstanalogread = false;
}

/* MAX7219 7 segment display driver */
void MAX7219_Send(uint16_t data, uint8_t bank)
{
    /* MAX 7219 PIN Connections:
     CLK = RG0
     CS/LOAD = RG1
     DIN Chip 0 = RG2
     DIN Chip 1 = RG3
     DIN Chip 2 = RG4
     DIN Chip 3 = RD2
     */
    
    int8_t bit;

    LATGbits.LATG1 = 0;
    
    for(bit = 15; bit >= 0; bit--)
    {
        LATGbits.LATG0 = 0;   // CLK low

        // Default all unused MAX7219s to NO-OP = all zero bits
        LATGbits.LATG2 = 0;
        LATGbits.LATG3 = 0;
        LATGbits.LATG4 = 0;
        LATDbits.LATD2 = 0;
        
        if (bank == 0) {
            LATGbits.LATG2 = (data >> bit) & 0x01;
        } else if (bank == 1) {
            LATGbits.LATG3 = (data >> bit) & 0x01; 
        } else if (bank == 2) {
            LATGbits.LATG4 = (data >> bit) & 0x01;             
        } else {
            LATDbits.LATD2 = (data >> bit) & 0x01; 
        }

        LATGbits.LATG0 = 1;   // rising edge clocks bit        
    }

    LATGbits.LATG0 = 0;
    
    // Latch all MAX7219s simultaneously.
    // Non-selected devices received 0x0000 = NO-OP.
    LATGbits.LATG1  = 1;
}

void MAX7219_Write(uint8_t reg, uint8_t val, uint8_t bank)
{
    uint16_t d = ((uint16_t)reg << 8) | val;

    MAX7219_Send(d, bank);
}

void init_displays(void)
{
    
   
    LATGbits.LATG0  = 0;
    LATGbits.LATG1 = 1;

    for (uint8_t bank=0; bank<4; bank++)
    {
    
        // Display test off
        MAX7219_Write(MAX_REG_TEST, 0, bank);

        // No BCD decoding; write raw segment bits
        MAX7219_Write(MAX_REG_DECODE, 0, bank);

        // Use all 8 digits
        MAX7219_Write(MAX_REG_SCANLIMIT, 7, bank);

        // Medium/low brightness
        MAX7219_Write(MAX_REG_INTENSITY, 3, bank);

        // Normal operation
        MAX7219_Write(MAX_REG_SHUTDOWN, 1, bank);

        // Clear all digits
        for(uint8_t i = 1; i <= 8; i++)
        {
            MAX7219_Write(i, 0, bank);
        }
    
    }
}

void write_displays(void)
{
    MAX7219_Write(MAX_REG_TEST, 1, 0);
    __delay_ms(1000);
    MAX7219_Write(MAX_REG_TEST, 0, 0);
    MAX7219_Write(MAX_REG_DIGIT0, 0xFF, 0);
}