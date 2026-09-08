/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC18F86J60
        Driver Version    :  2.00
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

#include <stdint.h>

#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/TCPIPLibrary/ethernet_driver.h"
#include "network_config.h"
#include "software_i2c.h"
#include "mcp23017_i2c.h"
#include "daughter_i2c.h"
#include "common.h"
#include "network_config.h"
#include "udp.h"
#include "gpio.h"

/* TODO:
- Check for UDP_Start returns "MAC_NOT_FOUND" and thus not successful UDP packet writes, only update data if UDP packet was sent

*/

volatile uint32_t tmr_print_count = 0;
volatile bool print_request = false;
volatile uint32_t tmr_poll_count = 0;
volatile bool poll_request = false;
volatile uint32_t counter = 0;
      
void myTimer(void)
{
    // TMR has to be set to FOSC/4, Prescaler 1:2, Timer Interrupt, Timer period 1ms
    // We have a 1 ms Timer
    
    // Every 1 second
    tmr_print_count++;
    if (tmr_print_count >= 1000)
    {
        tmr_print_count = 0;
        print_request = true;
    }
    
    // Every 1 millisecond
    tmr_poll_count++;
    if (tmr_poll_count >= 1)
    {
        tmr_poll_count = 0;
        poll_request = true;
    }
    
}                   

// Main application

void main(void)
{
    
    __delay_ms(100);
    
    // Initialize the device
    SYSTEM_Initialize();

    // Set Up Network addresses etc.
    network_config();
 
    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global and Peripheral Interrupts
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
    
    TMR1_SetInterruptHandler(myTimer);
       
    // Zero-out GPIO data structure
    init_data();

    // Initialize the MAX7219 7 segment display drivers
    init_displays();

    // Check if we have a destination host
    // Remove again in real code, as we do not want a blocking operation here
    while (!UDP_Check_ARP())
    {
        Network_Manage();
        __delay_ms(10);
    }
    
    //UDP_Send_String("Starting Main Loop \n");
    printf("Starting Main Loop\n");
    
    
    while(1)
        // we have around a 0.1 ms loop (may change) with load)
    {
        if (poll_request)
        {
            // run network code 
            Network_Manage();
 
            UDP_Recv_Task();            
           
            //write_i2c_outputs1();
            //write_i2c_servo();
               
            /* Digital Inputs every 1 ms */
            if ((counter % 1) == 0) {
                read_inputs();
            }
            
            /* Analog Inputs every 10 ms */
            if ((counter % 10) == 0) {
                read_analoginputs();
            }
             
            if ((counter % 1000) == 0) {
                //read_inputs();
                //read_i2c_inputs1();
     
                //UDP_Send_Task(true);
                                 
            } else {
               UDP_Send_Task(false);                 
            }
            
            /* Make sure all UDP Packets have been sent */
            while (ETH_GetTxQueueSize() != 0) {
               Network_Manage(); 
            }
           
            // update save variable state with new variable state
            // Has to be in the same interval as reading the digital inputs
            // Or copy the digital and analog inputs separately etc.
            if ((counter % 1) == 0) {
                copy_data();
            }
            
            poll_request = false;
            
        }
        
        counter++;
        
        if (print_request)
        {
            
            //printf("Loops per sec: %li\n",counter);

            print_request = false;
            counter = 0;
        }
    
    }  
    
}

/**
 End of File
*/