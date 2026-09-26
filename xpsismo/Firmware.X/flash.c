
#include <stdint.h>
#include "mcc_generated_files/memory.h"
#include <xc.h>
#include "flash.h"

/*
 *   Flash has to be aligned in 1024 byte steps in memory
 *   Requiured Linker Option: -mreserve=rom@0xF800:0xFBFF
 *   Required PicKit 5 Option: Preserve Range F800-FBFF
 */

static uint32_t FLASH_ReadUint32(uint32_t address)
{
    uint32_t value;

    value  = (uint32_t)FLASH_ReadByte(address);
    value |= (uint32_t)FLASH_ReadByte(address + 1) << 8;
    value |= (uint32_t)FLASH_ReadByte(address + 2) << 16;
    value |= (uint32_t)FLASH_ReadByte(address + 3) << 24;

    return value;
}

static uint16_t FLASH_ReadUint16(uint32_t address)
{
    uint16_t value;

    value  = (uint16_t)FLASH_ReadByte(address);
    value |= (uint16_t)FLASH_ReadByte(address + 1) << 8;

    return value;
}

/* Writes a 64 byte data block (this is the minimum size that can be written to flash, see mcc memory.h */
static void FLASH_Write64(uint32_t address, const uint8_t *data)
{
    uint8_t i;
    uint8_t gieState;

    TBLPTRU = (uint8_t)((address >> 16) & 0xFF);
    TBLPTRH = (uint8_t)((address >> 8) & 0xFF);
    TBLPTRL = (uint8_t)(address & 0xFF);

    for (i = 0; i < WRITE_FLASH_BLOCKSIZE; i++)
    {
        TABLAT = data[i];

        if (i == (WRITE_FLASH_BLOCKSIZE - 1))
        {
            asm("TBLWT");
        }
        else
        {
            asm("TBLWTPOSTINC");
        }
    }

    EECON1bits.WREN = 1;

    gieState = INTCONbits.GIE;
    INTCONbits.GIE = 0;

    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;

    EECON1bits.WREN = 0;
    INTCONbits.GIE = gieState;
}

bool Config_Read(uint32_t *myIpAddress, uint32_t *mySubnetMask, uint32_t *myGateway, uint8_t *myMacAddress, 
        uint32_t *yourIpAddress, uint16_t *myPort, uint16_t *yourPort, uint8_t *daughterCardConfig)
{
    uint32_t magic;

    magic = FLASH_ReadUint32(CONFIG_FLASH_ADDRESS);

    if (magic != CONFIG_MAGIC)
    {
        return false;
    }

    *myIpAddress = FLASH_ReadUint32(CONFIG_FLASH_ADDRESS + 4);
    *mySubnetMask = FLASH_ReadUint32(CONFIG_FLASH_ADDRESS + 8);
    *myGateway = FLASH_ReadUint32(CONFIG_FLASH_ADDRESS + 12);
    myMacAddress[0] = FLASH_ReadByte(CONFIG_FLASH_ADDRESS + 16);
    myMacAddress[1] = FLASH_ReadByte(CONFIG_FLASH_ADDRESS + 17);
    myMacAddress[2] = FLASH_ReadByte(CONFIG_FLASH_ADDRESS + 18);
    myMacAddress[3] = FLASH_ReadByte(CONFIG_FLASH_ADDRESS + 19);
    myMacAddress[4] = FLASH_ReadByte(CONFIG_FLASH_ADDRESS + 20);
    myMacAddress[5] = FLASH_ReadByte(CONFIG_FLASH_ADDRESS + 21);
    *yourIpAddress = FLASH_ReadUint32(CONFIG_FLASH_ADDRESS + 22);
    *myPort = FLASH_ReadUint16(CONFIG_FLASH_ADDRESS + 26);
    *yourPort = FLASH_ReadUint16(CONFIG_FLASH_ADDRESS + 28);
    *daughterCardConfig = FLASH_ReadByte(CONFIG_FLASH_ADDRESS + 30);
    
    return true;
}

bool Config_Write(uint32_t myIpAddress, uint32_t mySubnetMask, uint32_t myGateway, uint8_t myMacAddress[6], 
        uint32_t yourIpAddress, uint16_t myPort, uint16_t yourPort, uint8_t daughterCardConfig)
{
    uint8_t buffer[WRITE_FLASH_BLOCKSIZE];
    uint8_t i;

    /* Start with an erased 64-byte block */
    for (i = 0; i < WRITE_FLASH_BLOCKSIZE; i++)
    {
        buffer[i] = 0xFF;
    }

    /* Magic Number (to check during read if real data is present in the flash memory */
    buffer[0] = (uint8_t)(CONFIG_MAGIC);
    buffer[1] = (uint8_t)(CONFIG_MAGIC >> 8);
    buffer[2] = (uint8_t)(CONFIG_MAGIC >> 16);
    buffer[3] = (uint8_t)(CONFIG_MAGIC >> 24);

    /* My IP address */
    buffer[4] = (uint8_t)(myIpAddress);
    buffer[5] = (uint8_t)(myIpAddress >> 8);
    buffer[6] = (uint8_t)(myIpAddress >> 16);
    buffer[7] = (uint8_t)(myIpAddress >> 24);

     /* My Subnet Mask */
    buffer[8] = (uint8_t)(mySubnetMask);
    buffer[9] = (uint8_t)(mySubnetMask >> 8);
    buffer[10] = (uint8_t)(mySubnetMask >> 16);
    buffer[11] = (uint8_t)(mySubnetMask >> 24);

     /* My Gateway address */
    buffer[12] = (uint8_t)(myGateway);
    buffer[13] = (uint8_t)(myGateway >> 8);
    buffer[14] = (uint8_t)(myGateway >> 16);
    buffer[15] = (uint8_t)(myGateway >> 24);

    /* My MAC address */
    buffer[16] = myMacAddress[0];
    buffer[17] = myMacAddress[1];
    buffer[18] = myMacAddress[2];
    buffer[19] = myMacAddress[3];
    buffer[20] = myMacAddress[4];
    buffer[21] = myMacAddress[5];

    /* IP address of UDP Server */
    buffer[22] = (uint8_t)(yourIpAddress);
    buffer[23] = (uint8_t)(yourIpAddress >> 8);
    buffer[24] = (uint8_t)(yourIpAddress >> 16);
    buffer[25] = (uint8_t)(yourIpAddress >> 24);
            
     /* My UDP Port */
    buffer[26] = (uint8_t)(myPort);
    buffer[27] = (uint8_t)(myPort >> 8);
 
     /* UDP Port of Server */
    buffer[28] = (uint8_t)(yourPort);
    buffer[29] = (uint8_t)(yourPort >> 8);
 
    /* Daughter I2C Cards Config Byte */
    buffer[30] = daughterCardConfig;
    
    /*
     * Erases the entire 1024-byte configuration block
     * 0xF800 - 0xFBFF.
     */
    FLASH_EraseBlock(CONFIG_FLASH_ADDRESS);

    /*
     * Program only the first 64-byte write block.
     * Everything else remains erased (0xFF).
     */
    FLASH_Write64(CONFIG_FLASH_ADDRESS, buffer);

    return true;
}

bool Config_Erase(void)
{
 /*
     * Erases the entire 1024-byte configuration block
     * 0xF800 - 0xFBFF.
     */
    FLASH_EraseBlock(CONFIG_FLASH_ADDRESS);
    
    return true;
}