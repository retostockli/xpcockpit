/* 
 * File:   software_ic2.c
 * Author: stockli
 *
 * Created on August 27, 2026, 5:57 PM
 */

#include "software_i2c.h"
#include "mcc_generated_files/device_config.h"
#include <xc.h>

/*
 * ============================================================
 * PIC18F86J60 software I2C
 *
 * RJ4 = SCL
 * RJ5 = SDA
 *
 * I2C uses open-drain behavior:
 *
 *     LOW  -> configure pin as OUTPUT, LAT = 0
 *     HIGH -> configure pin as INPUT (released)
 *
 * External pull-up resistors are required.
 * ============================================================
 */


/*
 * ------------------------------------------------------------
 * Pin definitions
 * ------------------------------------------------------------
 */

#define I2C_SCL_LAT       LATJbits.LATJ4
#define I2C_SCL_PORT      PORTJbits.RJ4
#define I2C_SCL_TRIS      TRISJbits.TRISJ4

#define I2C_SDA_LAT       LATJbits.LATJ5
#define I2C_SDA_PORT      PORTJbits.RJ5
#define I2C_SDA_TRIS      TRISJbits.TRISJ5


/*
 * ------------------------------------------------------------
 * Delay
 *
 * This assumes _XTAL_FREQ is defined by your project.
 * ------------------------------------------------------------
 */

static void I2C_Delay(void)
{
    __delay_us(SOFT_I2C_DELAY_US);
}


/*
 * ------------------------------------------------------------
 * SCL control
 * ------------------------------------------------------------
 */

static void I2C_SCL_Low(void)
{
    I2C_SCL_LAT = 0;
    I2C_SCL_TRIS = 0;
}


static void I2C_SCL_Release(void)
{
    /*
     * Release SCL.
     * External pull-up brings it HIGH.
     */
    I2C_SCL_TRIS = 1;
}


/*
 * ------------------------------------------------------------
 * SDA control
 * ------------------------------------------------------------
 */

static void I2C_SDA_Low(void)
{
    I2C_SDA_LAT = 0;
    I2C_SDA_TRIS = 0;
}


static void I2C_SDA_Release(void)
{
    /*
     * Release SDA.
     * External pull-up brings it HIGH.
     */
    I2C_SDA_TRIS = 1;
}


static bool I2C_SDA_Read(void)
{
    return I2C_SDA_PORT != 0;
}


static bool I2C_SCL_Read(void)
{
    return I2C_SCL_PORT != 0;
}


/*
 * ------------------------------------------------------------
 * Initialize
 * ------------------------------------------------------------
 */

void I2C_Software_Initialize(void)
{
    /*
     * Make sure LAT registers contain zero before
     * switching the pins to outputs.
     */
    I2C_SCL_LAT = 0;
    I2C_SDA_LAT = 0;

    /*
     * Release both lines.
     */
    I2C_SCL_Release();
    I2C_SDA_Release();

    I2C_Delay();
}


/*
 * ------------------------------------------------------------
 * START condition
 *
 * SDA: HIGH -> LOW
 * while SCL is HIGH
 * ------------------------------------------------------------
 */

bool I2C_Software_Start(void)
{
    /*
     * Bus must be idle.
     */
    I2C_SDA_Release();
    I2C_SCL_Release();

    I2C_Delay();

    /*
     * Check that SCL actually went HIGH.
     */
    if (!I2C_SCL_Read())
    {
        return false;
    }

    /*
     * START:
     * SDA HIGH -> LOW while SCL HIGH
     */
    I2C_SDA_Low();

    I2C_Delay();

    /*
     * Pull SCL low to begin data transfer.
     */
    I2C_SCL_Low();

    I2C_Delay();

    return true;
}


/*
 * ------------------------------------------------------------
 * STOP condition
 *
 * SDA: LOW -> HIGH
 * while SCL is HIGH
 * ------------------------------------------------------------
 */

void I2C_Software_Stop(void)
{
    /*
     * Ensure SDA is LOW.
     */
    I2C_SDA_Low();

    I2C_Delay();

    /*
     * Release SCL.
     */
    I2C_SCL_Release();

    I2C_Delay();

    /*
     * Release SDA while SCL is HIGH.
     */
    I2C_SDA_Release();

    I2C_Delay();
}


/*
 * ------------------------------------------------------------
 * Wait for SCL to actually become HIGH.
 *
 * This provides basic clock-stretching support.
 * ------------------------------------------------------------
 */

static bool I2C_WaitForSCLHigh(void)
{
    /*
     * At 50 kHz, a slave normally releases SCL quickly.
     *
     * We don't want to wait forever if the bus is stuck.
     */

    uint16_t timeout = 1000;

    while (!I2C_SCL_Read())
    {
        if (--timeout == 0)
        {
            return false;
        }

        __delay_us(1);
    }

    return true;
}


/*
 * ------------------------------------------------------------
 * Generate one clock pulse
 * ------------------------------------------------------------
 */

static bool I2C_ClockHigh(void)
{
    /*
     * Release SCL.
     */
    I2C_SCL_Release();

    /*
     * Wait for SCL to actually go HIGH.
     * This also supports clock stretching.
     */
    if (!I2C_WaitForSCLHigh())
    {
        return false;
    }

    I2C_Delay();

    return true;
}


static void I2C_ClockLow(void)
{
    I2C_SCL_Low();

    I2C_Delay();
}


/*
 * ------------------------------------------------------------
 * Write one bit
 * ------------------------------------------------------------
 */

static bool I2C_WriteBit(bool bit)
{
    if (bit)
    {
        /*
         * Send HIGH by releasing SDA.
         */
        I2C_SDA_Release();
    }
    else
    {
        /*
         * Send LOW.
         */
        I2C_SDA_Low();
    }

    I2C_Delay();

    /*
     * Clock HIGH.
     */
    if (!I2C_ClockHigh())
    {
        return false;
    }

    /*
     * Clock LOW.
     */
    I2C_ClockLow();

    return true;
}


/*
 * ------------------------------------------------------------
 * Read one bit
 * ------------------------------------------------------------
 */

static bool I2C_ReadBit(bool *bit)
{
    /*
     * Release SDA so the slave can drive it.
     */
    I2C_SDA_Release();

    I2C_Delay();

    /*
     * Raise SCL.
     */
    if (!I2C_ClockHigh())
    {
        return false;
    }

    /*
     * Read SDA while SCL is HIGH.
     */
    *bit = I2C_SDA_Read();

    /*
     * Bring clock LOW.
     */
    I2C_ClockLow();

    return true;
}


/*
 * ------------------------------------------------------------
 * Write one byte and read ACK
 * ------------------------------------------------------------
 */

bool I2C_Software_WriteByte(uint8_t data)
{
    uint8_t i;
    bool ack;

    /*
     * Send MSB first.
     */
    for (i = 0; i < 8; i++)
    {
        if (!I2C_WriteBit((data & 0x80) != 0))
        {
            return false;
        }

        data <<= 1;
    }

    /*
     * Ninth clock:
     * release SDA and let slave generate ACK.
     */
    I2C_SDA_Release();

    I2C_Delay();

    if (!I2C_ClockHigh())
    {
        return false;
    }

    /*
     * ACK = LOW
     * NACK = HIGH
     */
    ack = !I2C_SDA_Read();

    I2C_ClockLow();

    return ack;
}


/*
 * ------------------------------------------------------------
 * Read one byte.
 *
 * ack = true  -> send ACK
 * ack = false -> send NACK
 * ------------------------------------------------------------
 */

uint8_t I2C_Software_ReadByte(bool ack)
{
    uint8_t i;
    uint8_t data = 0;
    bool bit;

    for (i = 0; i < 8; i++)
    {
        data <<= 1;

        if (!I2C_ReadBit(&bit))
        {
            return data;
        }

        if (bit)
        {
            data |= 1;
        }
    }

    /*
     * Master sends ACK/NACK.
     */
    if (ack)
    {
        I2C_SDA_Low();
    }
    else
    {
        I2C_SDA_Release();
    }

    I2C_Delay();

    I2C_ClockHigh();
    I2C_ClockLow();

    I2C_SDA_Release();

    return data;
}


/*
 * ------------------------------------------------------------
 * Write transaction
 *
 * address = 7-bit I2C address
 *
 * Example:
 *
 * I2C_Software_Write(0x50, data, 2);
 *
 * generates:
 *
 * START
 * 0xA0 (address + WRITE)
 * data[0]
 * data[1]
 * STOP
 * ------------------------------------------------------------
 */

bool I2C_Software_Write(uint8_t address,
                        const uint8_t *data,
                        uint8_t length)
{
    uint8_t i;

    if (!I2C_Software_Start())
    {
        return false;
    }

    /*
     * Address + WRITE.
     */
    if (!I2C_Software_WriteByte((uint8_t) (address << 1) | 0))
    {
        I2C_Software_Stop();
        return false;
    }

    /*
     * Data.
     */
    for (i = 0; i < length; i++)
    {
        if (!I2C_Software_WriteByte(data[i]))
        {
            I2C_Software_Stop();
            return false;
        }
    }

    I2C_Software_Stop();

    return true;
}


/*
 * ------------------------------------------------------------
 * Read transaction
 *
 * address = 7-bit address
 * ------------------------------------------------------------
 */

bool I2C_Software_Read(uint8_t address,
                       uint8_t *data,
                       uint8_t length)
{
    uint8_t i;

    if (!I2C_Software_Start())
    {
        return false;
    }

    /*
     * Address + READ.
     */
    if (!I2C_Software_WriteByte((uint8_t) (address << 1) | 1))
    {
        I2C_Software_Stop();
        return false;
    }

    /*
     * Receive bytes.
     *
     * ACK every byte except the last one.
     */
    for (i = 0; i < length; i++)
    {
        data[i] = I2C_Software_ReadByte(i < (length - 1));
    }

    I2C_Software_Stop();

    return true;
}


/*
 * ------------------------------------------------------------
 * Write followed by repeated START and read
 *
 * Very common for sensors and EEPROMs:
 *
 * START
 * ADDRESS + WRITE
 * register
 * REPEATED START
 * ADDRESS + READ
 * data...
 * STOP
 * ------------------------------------------------------------
 */

bool I2C_Software_WriteRead(uint8_t address,
                            const uint8_t *writeData,
                            uint8_t writeLength,
                            uint8_t *readData,
                            uint8_t readLength)
{
    uint8_t i;

    if (!I2C_Software_Start())
    {
        return false;
    }

    /*
     * Address + WRITE.
     */
    if (!I2C_Software_WriteByte((uint8_t) (address << 1) | 0))
    {
        I2C_Software_Stop();
        return false;
    }

    /*
     * Write data.
     */
    for (i = 0; i < writeLength; i++)
    {
        if (!I2C_Software_WriteByte(writeData[i]))
        {
            I2C_Software_Stop();
            return false;
        }
    }

    /*
     * Repeated START.
     */
    if (!I2C_Software_Start())
    {
        I2C_Software_Stop();
        return false;
    }

    /*
     * Address + READ.
     */
    if (!I2C_Software_WriteByte((uint8_t) (address << 1) | 1))
    {
        I2C_Software_Stop();
        return false;
    }

    /*
     * Read data.
     */
    for (i = 0; i < readLength; i++)
    {
        readData[i] =
            I2C_Software_ReadByte(i < (readLength - 1));
    }

    I2C_Software_Stop();

    return true;
}