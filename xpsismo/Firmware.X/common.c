/* 
 * File:   common.c
 * Author: stockli
 *
 * Created on August 28, 2026, 2:14 PM
 */

//#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/eusart1.h"
#include "mcc_generated_files/TCPIPLibrary/ethernet_driver.h"
#include "udp.h"

static uint8_t debugbuffer[SENDMSGLEN];
static uint8_t debugindex = 0;

void init_data(void)
{
    uint8_t i,j;
    firstanalogread = true;
    
    for (i=0;i<(MAXOUTPUTS/8);i++)
    {
        outputs[i] = OUTPUTSINITVAL;
        outputs_save[i] = OUTPUTSINITVAL;
    }
    for (i=0;i<(MAXINPUTS/8);i++)
    {
        inputs[i] = INPUTSINITVAL;
        inputs_save[i] = INPUTSINITVAL;
    }
   for (i=0;i<MAXANALOGINPUTS;i++)
    {
       for (j=0;j<MAXSAVE;j++) {
            analoginputs[i][j] = INPUTSINITVAL;
       }
       analoginputs_save[i] = INPUTSINITVAL;
       analoginputs_median[i] = INPUTSINITVAL;
    }
   
}

void copy_data(void)
{
    uint8_t i;
    
    memcpy(outputs_save,outputs,sizeof(outputs));
    memcpy(inputs_save,inputs,sizeof(inputs));
    
    for (i=0;i<MAXANALOGINPUTS;i++) {
        analoginputs_save[i] = analoginputs_median[i];
    }
    
}

//char getch(void)
//{
//    return EUSART1_Read();
//}

void putch(char txData)
{
#if DEBUG_UART
    EUSART1_Write(txData);
#endif
    
#if DEBUG_UDP
    
    /* We need to send exactly SENDMSGLEN of data and not
     just to the string terminator since the receiving party
     reads exactly SENDMSGLEN of bytes per UDP packet. */
    if (debugindex >= (SENDMSGLEN - 1)) {
        UDP_Send_Data(debugbuffer,SENDMSGLEN);
        debugindex = 0;
        memset(debugbuffer,0,sizeof(debugbuffer));          
    }

    if (debugindex < SENDMSGLEN) {
        // 2. Store the incoming character from printf into our RAM buffer
        debugbuffer[debugindex] = (uint8_t) txData;
        debugindex++;

        // 3. Check if the character is a line terminator (\n or \r)
        if (txData == '\n' || txData == '\r') {
            UDP_Send_Data(debugbuffer,SENDMSGLEN);
            debugindex = 0;
            memset(debugbuffer,0,sizeof(debugbuffer));          
        }   
    }

#endif
    
}
