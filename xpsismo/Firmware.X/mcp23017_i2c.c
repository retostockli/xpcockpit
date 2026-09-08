/* 
 * File:   mcp23017.c
 * Author: stockli
 *
 * Created on August 27, 2026, 6:10 PM
 */

#include "mcc_generated_files/device_config.h"
#include "mcp23017_i2c.h"
#include "software_i2c.h"

void MCP23017_Init(void)
{
    uint8_t data[2];

    // GPA0 = output
    // All other GPA pins remain inputs
    data[0] = MCP_IODIRA;
    data[1] = 0xFE;

    I2C_Software_Write(MCP23017_ADDRESS, data, 2);
}

void MCP23017_LED_On(void)
{
    uint8_t data[2];

    data[0] = MCP_GPIOA;
    data[1] = 0x01;

    I2C_Software_Write(MCP23017_ADDRESS, data, 2);
}

void MCP23017_LED_Off(void)
{
    uint8_t data[2];

    data[0] = MCP_GPIOA;
    data[1] = 0x00;

    I2C_Software_Write(MCP23017_ADDRESS, data, 2);
}

void MCP23017_Test(void)
{
    I2C_Software_Initialize();

    MCP23017_Init();

    MCP23017_LED_On();
    
    __delay_us(1000000);
    
    MCP23017_LED_Off();
}