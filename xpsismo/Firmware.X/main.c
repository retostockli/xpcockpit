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
#include "flash.h"

/* TODO:
- Check for UDP_Start returns "MAC_NOT_FOUND" and thus not successful UDP packet writes, only update data if UDP packet was sent

*/

volatile uint32_t tmr_print_count = 0;
volatile bool print_request = false;
volatile uint32_t tmr_poll_count = 0;
volatile bool poll_request = false;
volatile uint32_t ms_counter = 0;
volatile uint32_t loop_counter = 0;
volatile uint32_t second_counter = 0;

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
        
        second_counter++;                
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
    uint8_t modulo;
    
    __delay_ms(100);
    
    // Initialize the device
    SYSTEM_Initialize();
    
    // Initialize I2C Software interface
    I2C_Software_Initialize();

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
    
    // This is my own poll timer
    TMR3_SetInterruptHandler(myTimer);
       
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
    {
        if (poll_request)
        {
           
            poll_request = false;

            // run network code 
            Network_Manage();
 
            UDP_Recv_Task();            
           
            modulo = (uint8_t) ms_counter % 35;
            
            /* Outputs every 30 ms, but not all at the same time */
            if (modulo == 0) {
                write_outputs();
                write_displays();
            }
            if (modulo == 5) {
                write_i2c_outputs1();
            }
            //write_i2c_outputs2();
            //write_i2c_displays1();
            //write_i2c_displays2();
            if (modulo == 25) {
                write_i2c_servo();
            }
               
            /* Digital Inputs every 1 ms */
            read_inputs();
            read_i2c_inputs1();
            //read_i2c_inputs2();
            
            if (modulo == 30) {
                read_resetbutton();
                read_analoginputs();
                //read_i2c_analoginputs();)
            }            

            if ((ms_counter % 1000) == 0) {
                /* Send input state every second */
                //printf("1 Second passed\n");
     
                //UDP_Send_Task(true);
                UDP_Send_Task(false);
                                 
            } else {
                /* Or send input state every millisecond when Inputs have changed */
                UDP_Send_Task(false);             
            }
            
            /* Make sure all UDP Packets have been sent */
            while (ETH_GetTxQueueSize() != 0) {
               //if (ETH_GetTxQueueSize() > 1) printf("%i\n",ETH_GetTxQueueSize());
               Network_Manage(); 
            }
           
            // update save variable state with new variable state
            // Has to be in the same interval as reading the digital inputs
            // Or copy the digital and analog inputs separately etc.
            copy_data();
       
            ms_counter++;
            
        }
        
        if (print_request)
        {
            print_request = false;
           
            printf("Loops per sec: %li\n",loop_counter);
            printf("1 ms Interrupts per Second: %li\n",ms_counter);
            
            ms_counter = 0;
            loop_counter = 0;

        }
        
        loop_counter++;
    
    }  
    
}

/**
 End of File
*/