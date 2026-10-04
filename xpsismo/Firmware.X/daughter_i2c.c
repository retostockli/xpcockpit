/* 
 * File:   daugther_i2c.c
 * Author: stockli
 *
 * Created on August 28, 2026, 2:14 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mcc_generated_files/device_config.h"
#include "daughter_i2c.h"
#include "software_i2c.h"
#include "common.h"


void write_i2c_outputs1(void) 
{
    // 9 Byte I2C Packet
    // 1 Byte header, 8 Byte data
    // Each byte contains bitwise output values for 8 outputs
    // 64 outputs thus fill all 8 bytes.
    
    uint8_t i;
    bool changed = false;
    
    for (i=0;i<(MAXINPUTS_I2C/8);i++)
    {
        if (outputs_i2c1[i] != outputs_i2c1_save[i]) {
            changed = true;
            break;
        }
    }
    
    if (changed) {
        uint8_t data[9];

        data[0] = 0x00;
        for (i=0;i<(MAXINPUTS_I2C/8);i++) {
            data[i+1] = outputs_i2c1[i];
        }   

        if (!I2C_Software_Write(OUTPUTS1_I2C_ADDRESS, data, 9)) {
            printf("Error write I2C Outputs1\n");
        }
        
        memcpy(outputs_i2c1_save,outputs_i2c1,sizeof(outputs_i2c1));
    }
}

void read_i2c_inputs1(void)
{
    
    // Write a single Byte 0x00 to the Inputs daughter card
    // Read 64 inputs bitwise encoded in a 8 byte array
     
    uint8_t i; 
     
    uint8_t data1[1];
    uint8_t data2[8];
    
    data1[0] = 0x00;
    
    
    //I2C_Software_Write(INPUTS1_I2C_ADDRESS, data1, 1);   
    //if (I2C_Software_Read(INPUTS1_I2C_ADDRESS, data2, 8)) {
    
    if (I2C_Software_WriteRead(INPUTS1_I2C_ADDRESS, data1, 1, data2, 8)) {
 
        for (i=0;i<(MAXINPUTS_I2C/8);i++) {
            inputs_i2c1[i] = data2[i];
        }
//       printf("INPUTS: %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
//       data2[0], data2[1], data2[2], data2[3],
//       data2[4], data2[5], data2[6], data2[7]);
    
    } else {
        printf("Error reading I2C Inputs 1\n");
    }
    
}

void write_i2c_servo(void)
{
    // 11 Byte I2C Packet
    // 3 Byte Header
    // 11 Byte Data
    
    // Servo 1-8
    // Header 0x00 0x00 and bitwise servo byte. E.g.: Servo 2 = 0x02, servo 1+2: 0x03
    // Data 8 byte with a servo value in each byte, can have many servos set at the
    // same time
   
    // Servo 9-14
    // Header 0x00 0x01 and bitwise servo byte, like above but Servo 9 starts with 0x04
    // Data 8 byte with a servo value in each byte, servo 9 starts at byte 3
    
    uint8_t servo;
    uint8_t data[11];
    bool changed = false;
    
    memset(data,0,sizeof(data));
    for(servo=0;servo<8;servo++) {
        if (servos_i2c[servo] != servos_i2c_save[servo]) {
            data[2] |= (uint8_t) (1 << (servo));
            changed = true;
        }
    }
    
    if (changed) {
        data[0] = 0x00;
        data[1] = 0x00;
        //data[2] = 0xFF; /* Bitwise Servo Selector: Select all */
        for(servo=0;servo<8;servo++) {
            data[3+servo] = (uint8_t) servos_i2c[servo];
            //printf("%i %i %i\n",servo,servos_i2c[servo],data[2]);
        }
        if (!I2C_Software_Write(SERVO_I2C_ADDRESS, data, 11)) {
            printf("Error write I2C Servos 0-7\n");
        }
    }
    
    changed = false;

    memset(data,0,sizeof(data));
    for(servo=8;servo<14;servo++) {
        if (servos_i2c[servo] != servos_i2c_save[servo]) {
            data[2] |= (uint8_t) (1 << (servo-8+2));
            changed = true;
         }
    }
     
    if (changed) {
        data[0] = 0x00;        
        data[1] = 0x01;
        //data[2] = 0xFF; /* Bitwise Servo Selector: Select all */
        data[3] = 0x00;
        data[4] = 0x00;
        for(servo=8;servo<14;servo++) {
           data[3+2+servo-8] = (uint8_t) servos_i2c[servo];
           //printf("%i %i %i\n",servo,servos_i2c[servo],data[2]);
        }
        
        if (!I2C_Software_Write(SERVO_I2C_ADDRESS, data, 11)) {
            printf("Error write I2C Servos 8-13\n");               
        }       
    }
   
    memcpy(servos_i2c_save,servos_i2c,sizeof(servos_i2c));

}