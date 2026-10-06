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
 * File:   daughter_i2c.h
 * Author: Reto Stockli
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef DAUGHTER_I2C_H
#define	DAUGHTER_I2C_H

#include <xc.h> // include processor files - each processor file is guarded.  

#define ANALOG_I2C_ADDRESS 0x28
#define SERVO_I2C_ADDRESS 0x30
#define DISPLAY1_I2C_ADDRESS 0x38
#define DISPLAY2_I2C_ADDRESS 0x39
#define OUTPUTS1_I2C_ADDRESS 0x40
#define OUTPUTS2_I2C_ADDRESS 0x41
#define INPUTS1_I2C_ADDRESS 0x48
#define INPUTS2_I2C_ADDRESS 0x49

void read_i2c_analoginputs(void);
void write_i2c_outputs1(void);
void write_i2c_outputs2(void);
void read_i2c_inputs1(void);
void read_i2c_inputs2(void);
void write_i2c_servo(void);
void write_i2c_displays1(void);
void write_i2c_displays2(void);

#endif	/* DAUGHTER_I2C_H */

