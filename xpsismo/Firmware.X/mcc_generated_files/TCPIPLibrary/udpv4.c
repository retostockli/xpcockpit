/**
  UDP protocol v4 implementation
	
  Company:
    Microchip Technology Inc.

  File Name:
    udpv4.c

  Summary:
     This is the implementation of UDP version 4 protocol

  Description:
    This source file provides the implementation of the API for the UDP v4 protocol.

 */

/*

©  [2015] Microchip Technology Inc. and its subsidiaries.  You may use this software 
and any derivatives exclusively with Microchip products. 
  
THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER EXPRESS, 
IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF 
NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE, OR ITS 
INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE 
IN ANY APPLICATION. 

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL 
OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED 
TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY 
OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S 
TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED 
THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE TERMS. 

*/

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
//#include <conio.h>   // jira:M8TS-608
#include "ipv4.h"
#include "udpv4.h"
#include "udpv4_port_handler_table.h"
#include "network.h"
#include "ethernet_driver.h"
#include "tcpip_types.h"
#include "icmp.h"

/**
  Section: Macro Declarations
*/

/**
  Section: Local Variables
*/

uint16_t destPort;
udpHeader_t udpHeader;

/**
  Section: UDP Library APIs
*/

error_msg UDP_Start(uint32_t destIP, uint16_t srcPort, uint16_t dstPort)
{
     // Start a UDP Packet to Write UDP Header
    error_msg ret = ERROR;

    // Start IPv4 Packet to Write IPv4 Header
    ret = IPv4_Start(destIP,UDP_TCPIP);
    if(ret == SUCCESS)
    {
        //Start to Count the UDP payload length Bytes
        ETH_ResetByteCount();

        // Write UDP Source Port
        ETH_Write16(srcPort);

        //Write UDP Destination Port
        ETH_Write16(dstPort);

        //Write DataLength; Initially set to '0'
        ETH_Write16(0);

        //Write UDP Checksum; Initially set to '0'
        ETH_Write16(0);

    }
    return ret;
}

error_msg UDP_Send(void)
{
    uint16_t udpLength;
    uint16_t cksm;
    error_msg ret = ERROR;

    udpLength = ETH_GetByteCount();
    udpLength = ntohs(udpLength);
    ETH_Insert((char *)&udpLength, 2, sizeof(ethernetFrame_t) + sizeof(ipv4Header_t) + offsetof(udpHeader_t,length));
    udpLength = htons(udpLength);
    
    // add the UDP header checksum
    cksm = udpLength + UDP_TCPIP;
    cksm = ETH_TxComputeChecksum(sizeof(ethernetFrame_t) + sizeof(ipv4Header_t) - 8, udpLength + 8, cksm);

    // if the computed checksum is "0" set it to 0xFFFF
    if (cksm == 0){
        cksm = 0xffff;
    }

    ETH_Insert((char *)&cksm, 2, sizeof(ethernetFrame_t) + sizeof(ipv4Header_t) + offsetof(udpHeader_t,checksum));

    ret = IPV4_Send(udpLength);

    return ret;
}

error_msg UDP_Receive(uint16_t udpcksm) // catch all UDP packets and dispatch them to the appropriate callback
{
    error_msg ret = ERROR;
    udp_table_iterator_t  hptr;

    ETH_ReadBlock((char *)&udpHeader,sizeof(udpHeader));

    if((udpHeader.checksum == 0) || (udpcksm == 0))
    {
        udpHeader.dstPort = ntohs(udpHeader.dstPort); // reverse the port number
        destPort = ntohs(udpHeader.srcPort);
        udpHeader.length = ntohs(udpHeader.length);
        ret = PORT_NOT_AVAILABLE;
        // scan the udp port handlers and find a match.
        // call the port handler callback on a match
        hptr = udp_table_getIterator();
        
        while(hptr != NULL)
        {
            if(hptr->portNumber == udpHeader.dstPort)
            {          
                if(udpHeader.length == IPV4_GetDatagramLength())
                {
                    hptr->callBack((int16_t) (udpHeader.length - sizeof(udpHeader)));                    
                }
                ret = SUCCESS;
                break;
            }
            hptr = udp_table_nextEntry(hptr);
        }
        if(ret== PORT_NOT_AVAILABLE)
        {
            
            //Send Port unreachable                
            ICMP_PortUnreachable(UDP_GetSrcIP(), UDP_GetDestIP(), DEST_UNREACHABLE_LEN);  //jira: CAE_MCU8-5706
        }
    }
    else
    {
        ret = UDP_CHECKSUM_FAILS;
    }
    return ret;
}
