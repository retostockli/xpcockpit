
#include <stdint.h>
#include "mcc_generated_files/memory.h"
#include <xc.h>
#include "flash.h"

/*
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

bool IPAddress_Read(uint32_t *ipAddress)
{
    uint32_t magic;

    magic = FLASH_ReadUint32(CONFIG_FLASH_ADDRESS);

    if (magic != CONFIG_MAGIC)
    {
        return false;
    }

    *ipAddress = FLASH_ReadUint32(CONFIG_FLASH_ADDRESS + 4);

    return true;
}

bool IPAddress_Write(uint32_t ipAddress)
{
    uint8_t buffer[WRITE_FLASH_BLOCKSIZE];
    uint8_t i;

    /* Start with an erased 64-byte block */
    for (i = 0; i < WRITE_FLASH_BLOCKSIZE; i++)
    {
        buffer[i] = 0xFF;
    }

    /* Magic */
    buffer[0] = (uint8_t)(CONFIG_MAGIC);
    buffer[1] = (uint8_t)(CONFIG_MAGIC >> 8);
    buffer[2] = (uint8_t)(CONFIG_MAGIC >> 16);
    buffer[3] = (uint8_t)(CONFIG_MAGIC >> 24);

    /* IP address */
    buffer[4] = (uint8_t)(ipAddress);
    buffer[5] = (uint8_t)(ipAddress >> 8);
    buffer[6] = (uint8_t)(ipAddress >> 16);
    buffer[7] = (uint8_t)(ipAddress >> 24);

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