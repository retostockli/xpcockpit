/**
  ICMP protocol implementation
	
  Company:
    Microchip Technology Inc.

  File Name:
    icmp.c

  Summary:
     This is the implementation of ICMP version 4 stack.

  Description:
    This source file provides the implementation of the API for the ICMP Echo Ping Request/Reply.

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

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include "network.h"
#include "tcpip_types.h"
#include "ethernet_driver.h"
#include "ipv4.h"
#include "icmp.h"
#include "ip_database.h"

/* Port 0 is N/A in both UDP and TCP */
uint16_t portUnreachable = 0;

/**
 * ICMP packet receive
 * @param ipv4_header
 * @return
 */
error_msg ICMP_Receive(ipv4Header_t *ipv4Hdr)
{
    icmpHeader_t icmpHdr;
    error_msg ret = ERROR;
    ETH_ReadBlock(&icmpHdr, sizeof(icmpHeader_t));   
    
    if(ipv4Hdr->dstIpAddress == SPECIAL_IPV4_BROADCAST_ADDRESS)
        return DEST_IP_NOT_MATCHED;
    switch(ntohs((icmpTypeCodes_t)icmpHdr.typeCode))
    {
                case ECHO_REQUEST:
        case UNASSIGNED_ECHO_TYPE_CODE_REQUEST_1:
        case UNASSIGNED_ECHO_TYPE_CODE_REQUEST_2:
        {            
            ret = ICMP_EchoReply(ipv4Hdr);
        }
        break;  
        case DEST_PORT_UNREACHABLE:
            ETH_Dump(4);
            ETH_ReadBlock(ipv4Hdr,sizeof(ipv4Header_t));            
            if(5 == ipv4Hdr->ihl)
            {
                portUnreachable = ETH_Read16();           
            }
            break;
        default:
            break;
    }

    return ret;
}

/**
 * ICMP Packet Start
 * @param icmp_header
 * @param dest_address
 * @param protocol
 * @param payload_length
 * @return
 */

error_msg ICMP_EchoReply(ipv4Header_t *ipv4Hdr)
{
    uint16_t cksm =0;
    error_msg ret = ERROR;
    uint16_t identifier;
    uint16_t sequence;

    identifier = ETH_Read16();
    sequence = ETH_Read16();        
    ret = IPv4_Start(ipv4Hdr->srcIpAddress, ipv4Hdr->protocol);
    if(ret == SUCCESS)
    {
        uint16_t icmp_cksm_start;
        uint16_t ipv4PayloadLength = ipv4Hdr->length - sizeof(ipv4Header_t);

        ipv4PayloadLength = ipv4Hdr->length - (uint16_t)(ipv4Hdr->ihl << 2);

        ETH_Write16(ECHO_REPLY);
        ETH_Write16(0); // checksum
        
        ETH_Write16(identifier);
        ETH_Write16(sequence);
        ETH_SaveRDPT(); //Get the Read Pointer
        // copy the next N bytes from the RX buffer into the TX buffer
        ret = ETH_Copy(ipv4PayloadLength - sizeof(icmpHeader_t) - 4);
        if(ret==SUCCESS) // copy can timeout in heavy network situations like flood ping
        {
            // compute a checksum over the ICMP payload
            cksm = sizeof(ethernetFrame_t) + sizeof(ipv4Header_t);
            icmp_cksm_start = sizeof(ethernetFrame_t) + sizeof(ipv4Header_t);
            cksm = ETH_TxComputeChecksum(icmp_cksm_start, ipv4PayloadLength, 0);
            ETH_Insert((char *)&cksm,sizeof(cksm),sizeof(ethernetFrame_t) + sizeof(ipv4Header_t) + offsetof(icmpHeader_t,checksum));
            ret = IPV4_Send(ipv4PayloadLength);
        }
    }
    return ret;
}

error_msg ICMP_PortUnreachable(uint32_t srcIPAddress,uint32_t destIPAddress, uint16_t length)
{    
    error_msg ret = ERROR;
    uint16_t cksm = 0;  
    
    if(srcIPAddress!=ipdb_getAddress())
    {
        return DEST_IP_NOT_MATCHED;
    }
    
    ret = IPv4_Start(destIPAddress, ICMP_TCPIP);
    if(ret == SUCCESS)
    {        
        ETH_Write16(DEST_PORT_UNREACHABLE);
        ETH_Write16(0); // checksum
        ETH_Write32(0); //unused and next-hop
        ETH_SetReadPtr(Network_GetStartPosition()); 
        ETH_SaveRDPT(); // Get the Read Pointer
        ETH_Copy(sizeof(ipv4Header_t) + length);
        cksm = ETH_TxComputeChecksum(sizeof(ethernetFrame_t) + sizeof(ipv4Header_t),  sizeof(icmpHeader_t)+ sizeof(ipv4Header_t) + length, 0);
        ETH_Insert((char *)&cksm,sizeof(cksm),sizeof(ethernetFrame_t) + sizeof(ipv4Header_t) + offsetof(icmpHeader_t,checksum));
        ret = IPV4_Send(sizeof(icmpHeader_t)+sizeof(ipv4Header_t)+length);
       
    }
    return ret;
}

bool isPortUnreachable(uint16_t port)
{
    if(portUnreachable==port)
        return true;
    else
        return false;
}

void resetPortUnreachable(void)
{
    portUnreachable = 0;
}
