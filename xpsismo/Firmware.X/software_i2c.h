/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  


#ifndef SOFTWARE_I2C_H
#define SOFTWARE_I2C_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
#include <stdbool.h>


/*
 * PIC18F86J60
 *
 * RJ4 = SCL
 * RJ5 = SDA
 *
 * Start at 50 kHz.
 *
 * 10 us LOW + 10 us HIGH ? 50 kHz
 */

#define SOFT_I2C_DELAY_US    5

void I2C_Software_Initialize(void);

bool I2C_Software_Start(void);
void I2C_Software_Stop(void);

bool I2C_Software_WriteByte(uint8_t data);
uint8_t I2C_Software_ReadByte(bool ack);

bool I2C_Software_Write(uint8_t address,
                        const uint8_t *data,
                        uint8_t length);

bool I2C_Software_Read(uint8_t address,
                       uint8_t *data,
                       uint8_t length);

bool I2C_Software_WriteRead(uint8_t address,
                            const uint8_t *writeData,
                            uint8_t writeLength,
                            uint8_t *readData,
                            uint8_t readLength);


#endif	/* SOFTWARE_I2C_H */

