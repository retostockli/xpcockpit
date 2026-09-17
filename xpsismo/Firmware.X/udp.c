/*
 *  (c) 2020 Microchip Technology Inc. and its subsidiaries.
 *
 *  Subject to your compliance with these terms,you may use this software and
 *  any derivatives exclusively with Microchip products.It is your responsibility
 *  to comply with third party license terms applicable to your use of third party
 *  software (including open source software) that may accompany Microchip software.
 *
 *  THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 *  EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 *  WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 *  PARTICULAR PURPOSE.
 *
 *  IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
 *  INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
 *  WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
 *  BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
 *  FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
 *  ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 *  THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 */

#include <stdint.h>
#include <string.h>
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/TCPIPLibrary/udpv4.h"
#include "mcc_generated_files/TCPIPLibrary/arpv4.h"
#include "mcc_generated_files/TCPIPLibrary/tcpip_config.h"
#include "mcc_generated_files/TCPIPLibrary/ip_database.h"
#include "mcc_generated_files/pin_manager.h"
#include "network_config.h"
#include "common.h"
#include "udp.h"

static udpStart_t udpPacket;

typedef struct
{
    uint8_t data[RECVMSGLEN];
    uint16_t length;
    uint32_t destIP;
} udpRxPacket_t;

static udpRxPacket_t udpRxQueue[UDP_RX_QUEUE_SIZE];

static volatile uint8_t udpRxCount;

static volatile bool ARP_Available;

bool UDP_Check_ARP(void) 
{
    uint16_t ret;
    if (ARPV4_Lookup(udpPacket.destinationAddress) != 0)
    {
        printf("ARP Ready\n");
        ARP_Available = true;
        return true;
    }
    else
    {       
        ret = ARPV4_Request(udpPacket.destinationAddress);
        if (ret == 1) {
            printf("ARP Available\n");
            ARP_Available = true;
            return true;
        } else {
            printf("ARP Not Available %i \n", ret);
            ARP_Available = false;
            return false;
        }
    }
}

void UDP_Initialize(uint32_t destinationAddress, uint16_t sourcePortNumber, uint16_t destinationPortNumber)
{
    udpPacket.destinationAddress = destinationAddress;
    udpPacket.sourcePortNumber = sourcePortNumber;  
    udpPacket.destinationPortNumber = destinationPortNumber;    
    
    udpRxCount = 0;
    ARP_Available = false;
}

void UDP_Recv_Data(int16_t length)
{
    uint8_t udpRxBuffer[RECVMSGLEN];
    
    
    if (length > RECVMSGLEN)
    {
        length = RECVMSGLEN;
    }

    UDP_ReadBlock(udpRxBuffer, (uint16_t) length);

    if (udpRxCount != UDP_RX_QUEUE_SIZE) {
        memcpy(udpRxQueue[udpRxCount].data,udpRxBuffer,sizeof(udpRxBuffer));
        udpRxQueue[udpRxCount].length = (uint16_t) length;
        udpRxQueue[udpRxCount].destIP = UDP_GetDestIP();
        udpRxCount++;
    } else {
        printf("UDP RX QUEUE SIZE OVERFLOW!\n");
    }
    
}

void UDP_Recv_Task(void)
{
    
    uint8_t i,q,g,d;
     
    if (udpRxCount == 0)
    {
        return;
    }
    
    /* Process command/data here: Eat up Queue from bottom */
    for (q=0;q<udpRxCount;q++) {

        if (udpRxQueue[q].length == RECVMSGLEN) {
            
            if ((udpRxQueue[q].data[0] == 0x53) && (udpRxQueue[q].data[1] == 0x43)) {
            
//                printf("UDP RECV %u bytes from %s\n",
//                       udpRxQueue[q].length,
//                       makeIpv4AddresstoStr(udpRxQueue[q].destIP));
                
                if (udpRxQueue[q].data[2] == 0x00) {
                    /* Data for SC-MB */
                    if (udpRxQueue[q].data[3] == 0x00) {
                        /* Digital Outputs */
                        for (i=0;i<(MAXOUTPUTS/8);i++) {
                            outputs[i] = udpRxQueue[q].data[4+i];
                        }
                    } else if (udpRxQueue[q].data[3] == 0x01) {
                        /* 7 Segment Displays */
                        g = udpRxQueue[q].data[4]-1;
                        for (i=0;i<(MAXDISPLAYS/4);i++) {
                            d = g*8 + i;
                            displays[d] = udpRxQueue[q].data[5+i];
                        }
                        brightness[g] = udpRxQueue[q].data[13];
                    }
                }
            }

        } else {
            printf("UDP RECV WRONG LENGTH: %u\n",udpRxQueue[q].length);
        }
        
    }
 
    /* shift queue by one */
    for (q=1;q<udpRxCount;q++) {
        memcpy(udpRxQueue[q-1].data,udpRxQueue[q].data,RECVMSGLEN);
        udpRxQueue[q-1].length = udpRxQueue[q].length;
        udpRxQueue[q-1].destIP = udpRxQueue[q].destIP;
    }
       
    udpRxCount--;

}

/*** Application to send data using UDP protocol ***/

void UDP_Send_String (char text[])
{
    error_msg ret = ERROR;
    //char text[] = "Hello World";
   
        /**************** Start UDP Packet ****************************
         * @Param1 - Destination Address
         * @Param2 - Source Port Number
         * @Param3 - Destination Port Number
         **********************************************************************/
    ret = UDP_Start(udpPacket.destinationAddress, udpPacket.sourcePortNumber, udpPacket.destinationPortNumber);
      
       
         /**************** Write UDP Packet ****************************
         * @Param1 - Data to write 
         ***********************************************************************/
    if(ret == SUCCESS)
    {
           
        UDP_WriteString(text);
        /**************** Send UDP Packet ****************************/
        UDP_Send();
                
    } else {
       //printf("UDP_Start Error: %s\n", network_errors[ret]);      
       //printf("UDP_Start Error: %i\n", ret);      
    }    
    
}

void UDP_Send_Data (uint8_t data[], uint16_t length)
{
    error_msg ret = ERROR;
    //char text[] = "Hello World";
   
        /**************** Start UDP Packet ****************************
         * @Param1 - Destination Address
         * @Param2 - Source Port Number
         * @Param3 - Destination Port Number
         **********************************************************************/
    ret = UDP_Start(udpPacket.destinationAddress, udpPacket.sourcePortNumber, udpPacket.destinationPortNumber);
      
       
         /**************** Write UDP Packet ****************************
         * @Param1 - Data to write 
         ***********************************************************************/
    if(ret == SUCCESS)
    {
           
        UDP_WriteBlock((const char *) data,length);
        /**************** Send UDP Packet ****************************/
        UDP_Send();
                
    } else {
       //printf("UDP_Start Error: %s\n", network_errors[ret]);      
       //printf("UDP_Start Error: %i\n", ret);      
    }    
    
}

void UDP_Send_Task(bool force)
{
    uint8_t senddata[SENDMSGLEN];
    int8_t i;
    bool changed = false;
   
    for (i=0;i<(MAXINPUTS/8);i++) {
        if (inputs[i] != inputs_save[i]) {
            changed = true;
            break;
        }
    }
    
    for (i=0;i<MAXANALOGINPUTS;i++) {
        if (analoginputs_median[i] != analoginputs_save[i]) {
            changed = true;
            break;
        }
    }
    
    if (force) changed = true;
    if (changed) {

        memset(senddata,0,sizeof(senddata));

        senddata[0] = 0x53;
        senddata[1] = 0x43;
        senddata[2] = myMacAddress[4];
        senddata[3] = myMacAddress[5];
        senddata[4] = 0x00; // 0x00: Master digital / analog inputs, 0x01/0x02 Daughter 1/2 digital inputs, 0x03: daughter analog inputs
        senddata[5] = 0x00; // Activated Daughter Cards (I2C): Todo
        senddata[6] = myPort & 0xFF;
        senddata[7] = myPort >> 8;

        for (i=0;i<(MAXINPUTS/8);i++) {
            senddata[i+8] = inputs[i];
        }

        for (i=0;i<MAXANALOGINPUTS;i++) {
            senddata[i*2 + 16] = analoginputs_median[i] & 0xFF;
            senddata[i*2 + 1 + 16] = analoginputs_median[i] >> 8;
        }

        //printf("UDP SEND\n");
        UDP_Send_Data(senddata, sizeof(senddata));
    }
 
}