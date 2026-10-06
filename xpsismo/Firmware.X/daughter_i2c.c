/* 
 * File:   daughter_i2c.c
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
#include "sort.h"
#include "common.h"

static uint8_t historyIndex_i2c = 0;

void read_i2c_analoginputs(void) 
{
    // Write a single Byte 0x00 to the Analog Inputs daughter card
    // Read 10 16 bit analog inputs in a 22 byte array (last byte is bogus)
    
    if (daughter_analoginput == 1) {
    
        uint8_t i,h; 
        
        uint16_t temparr[MAXSAVE];
        int16_t noise = 1;
        uint16_t median;
       
        uint8_t data1[1];
        uint8_t data2[22];

        data1[0] = 0x00;
        
        if (I2C_Software_WriteRead(ANALOG_I2C_ADDRESS, data1, 1, data2, 22)) {

            for (i=0;i<MAXANALOGINPUTS_I2C;i++) {
           
                if (firstanalogread_i2c) {
                    for (h=0;h<MAXSAVE;h++) {
                        analoginputs_i2c[i][h] = ((uint16_t)data2[i*2+1] << 8) | data2[i*2];
                    }
                } else {       
                    analoginputs_i2c[i][historyIndex_i2c] = ((uint16_t)data2[i*2+1] << 8) | data2[i*2];
                }

                // make a temporary copy for median filtering
                memcpy(temparr, analoginputs_i2c[i], sizeof(analoginputs_i2c[i][0]) * MAXSAVE);
                
                if (firstanalogread_i2c) {
                    median = analoginputs_i2c[i][0];
                } else {
                    //sort_uint16(temparr, MAXSAVE);
                    sort_uint16_7(temparr); /* Faster sorting for exactly 7 elements */
                    median = temparr[MAXSAVE / 2];
                }

                /* only send current value if it is outside median and noise */
                if (((int16_t) median < ((int16_t) analoginputs_i2c_save[i] - noise)) || ((int16_t) median > ((int16_t) analoginputs_i2c_save[i] + noise))) {       
                    //printf("ANA %i 0: %i MED: %i SAV: %i \n",i, (int) analoginputs_i2c[i][historyIndex_i2c], median, analoginputs_i2c_save[i]); 
                    analoginputs_i2c_median[i] = median;         
                }

           }
            
           firstanalogread_i2c = false;

           // augment pointer to newest value in circular buffer
           historyIndex_i2c++;

           if (historyIndex_i2c >= MAXSAVE)
               historyIndex_i2c = 0;                

        } else {
            printf("Error read I2C Analog Inputs\n");
        }

    }    
    
}

void write_i2c_outputs1(void) 
{
    // 9 Byte I2C Packet
    // 1 Byte header, 8 Byte data
    // Each byte contains bitwise output values for 8 outputs
    // 64 outputs thus fill all 8 bytes.
    
    if (daughter_output1 == 1) {
     
        uint8_t i;
        bool changed = false;

        for (i=0;i<(MAXOUTPUTS_I2C/8);i++)
        {
            if (outputs_i2c1[i] != outputs_i2c1_save[i]) {
                changed = true;
                break;
            }
        }

        if (changed) {
            uint8_t data[9];

            data[0] = 0x00;
            for (i=0;i<(MAXOUTPUTS_I2C/8);i++) {
                data[i+1] = outputs_i2c1[i];
            }   

            if (!I2C_Software_Write(OUTPUTS1_I2C_ADDRESS, data, 9)) {
                printf("Error write I2C Outputs 1\n");
            }

            memcpy(outputs_i2c1_save,outputs_i2c1,sizeof(outputs_i2c1));
        }
    
    }
}

void write_i2c_outputs2(void) 
{
    // 9 Byte I2C Packet
    // 1 Byte header, 8 Byte data
    // Each byte contains bitwise output values for 8 outputs
    // 64 outputs thus fill all 8 bytes.
    
    if (daughter_output2 == 1) {
     
        uint8_t i;
        bool changed = false;

        for (i=0;i<(MAXOUTPUTS_I2C/8);i++)
        {
            if (outputs_i2c2[i] != outputs_i2c2_save[i]) {
                changed = true;
                break;
            }
        }

        if (changed) {
            uint8_t data[9];

            data[0] = 0x00;
            for (i=0;i<(MAXOUTPUTS_I2C/8);i++) {
                data[i+1] = outputs_i2c2[i];
            }   

            if (!I2C_Software_Write(OUTPUTS2_I2C_ADDRESS, data, 9)) {
                printf("Error write I2C Outputs 2\n");
            }

            memcpy(outputs_i2c2_save,outputs_i2c2,sizeof(outputs_i2c2));
        }
    
    }
}

void read_i2c_inputs1(void)
{
    
    // Write a single Byte 0x00 to the Inputs daughter card
    // Read 64 inputs bitwise encoded in a 8 byte array
    
    if (daughter_input1 == 1) {
        
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
            printf("Error read I2C Inputs 1\n");
        }

    }
}

void read_i2c_inputs2(void)
{
    
    // Write a single Byte 0x00 to the Inputs daughter card
    // Read 64 inputs bitwise encoded in a 8 byte array
    
    if (daughter_input2 == 1) {
    
        uint8_t i; 

        uint8_t data1[1];
        uint8_t data2[8];

        data1[0] = 0x00;

        //I2C_Software_Write(INPUTS1_I2C_ADDRESS, data1, 1);   
        //if (I2C_Software_Read(INPUTS1_I2C_ADDRESS, data2, 8)) {

        if (I2C_Software_WriteRead(INPUTS2_I2C_ADDRESS, data1, 1, data2, 8)) {

            for (i=0;i<(MAXINPUTS_I2C/8);i++) {
                inputs_i2c2[i] = data2[i];
            }
    //       printf("INPUTS: %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
    //       data2[0], data2[1], data2[2], data2[3],
    //       data2[4], data2[5], data2[6], data2[7]);

        } else {
            printf("Error read I2C Inputs 2\n");
        }

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
    
    if (daughter_servo == 1) {
    
        uint8_t servo;
        uint8_t data[11];
        bool changed = false;

        memset(data,0,sizeof(data));
        for(servo=0;servo<8;servo++) {
            if (servos_i2c[servo] != servos_i2c_save[servo]) {
                data[2] |= (uint8_t) (1 << (servo)); /* Bitwise Servo Selector */
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
                data[2] |= (uint8_t) (1 << (servo-8+2)); /* Bitwise Servo Selector */
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
    
}

void write_i2c_displays1(void) 
{
    // 11 Byte I2C Packet
    // 2 Byte header, 9 Byte data
    // Each data byte contains a single digit
    // Last data byte contains the brightness of the 8 display bank
    
    if (daughter_display1 == 1) {
     
        uint8_t d,i,b;
        bool changed;
    
        // Loop through display banks
        for (b=0;b<(MAXDISPLAYS_I2C/8);b++) {
            
            changed = false;
            
            for (i=0;i<(MAXDISPLAYS_I2C/4);i++) {
                d = b*8 + i;

                if (displays_i2c1[d] != displays_i2c1_save[d]) {
                    changed = true;
                    break;
                }
            }
            
            if (brightness_i2c1[b] != brightness_i2c1_save[b]) changed = true;

            if (changed) {
                
                uint8_t data[11];

                data[0] = 0x00;
                data[1] = b + 1;
                for (i=0;i<(MAXDISPLAYS_I2C/4);i++) {
                    d = b*8 + i;
                    data[i+2] = displays_i2c1[d];
                }   
                data[10] = brightness_i2c1[b];

                if (!I2C_Software_Write(DISPLAY1_I2C_ADDRESS, data, 11)) {
                    printf("Error write I2C Displays 1\n");
                }

                memcpy(&displays_i2c1_save[b*8],&displays_i2c1[b*8],sizeof(displays_i2c1)/4);
                brightness_i2c1_save[b] = brightness_i2c1[b];
            }
            
        }
    
    }
}

void write_i2c_displays2(void) 
{
    // 11 Byte I2C Packet
    // 2 Byte header, 9 Byte data
    // Each data byte contains a single digit
    // Last data byte contains the brightness of the 8 display bank
    
    if (daughter_display2 == 1) {
     
        uint8_t d,i,b;
        bool changed;
    
        // Loop through display banks
        for (b=0;b<(MAXDISPLAYS_I2C/8);b++) {
            
            changed = false;
            
            for (i=0;i<(MAXDISPLAYS_I2C/4);i++) {
                d = b*8 + i;

                if (displays_i2c2[d] != displays_i2c2_save[d]) {
                    changed = true;
                    break;
                }
            }
            
            if (brightness_i2c2[b] != brightness_i2c2_save[b]) changed = true;

            if (changed) {
                
                uint8_t data[11];

                data[0] = 0x00;
                data[1] = b + 1;
                for (i=0;i<(MAXDISPLAYS_I2C/4);i++) {
                    d = b*8 + i;
                    data[i+2] = displays_i2c2[d];
                }   
                data[10] = brightness_i2c2[b];

                if (!I2C_Software_Write(DISPLAY2_I2C_ADDRESS, data, 11)) {
                    printf("Error write I2C Displays 2\n");
                }

                memcpy(&displays_i2c2_save[b*8],&displays_i2c2[b*8],sizeof(displays_i2c2)/4);
                brightness_i2c2_save[b] = brightness_i2c2[b];
            }
            
        }
    
    }
}
