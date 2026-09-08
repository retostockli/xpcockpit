/**
  IPv4 implementation

  Company:
    Microchip Technology Inc.

  File Name:
    ipv4.c

  Summary:
    This is the implementation of IP version 4 stack

  Description:
    This source file provides the implementation of the API for the IP v4 stack.

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
#include "ipv4.h"
#include "icmp.h"
#include "arpv4.h"
#include "udpv4.h"
#include "udpv4_port_handler_table.h"
#include "tcpip_types.h"
#include "ethernet_driver.h"
#include "log.h"
#include "ip_database.h"


#ifdef ENABLE_NETWORK_DEBUG
#define logMsg(msg, msgSeverity, msgLogDest)    logMessage(msg, LOG_KERN, msgSeverity, msgLogDest)  
#else
#define logMsg(msg, msgSeverity, msgLogDest)
#endif

ipv4Header_t ipv4Header;

static uint8_t getHeaderLen(void);   //jira: CAE_MCU8-5737

void IPV4_Init(void)
{
    ipdb_init();
}

uint16_t IPV4_PseudoHeaderChecksum(uint16_t payloadLen)
{
    ipv4_pseudo_header_t tmp;
    uint8_t len;
    uint32_t cksm = 0;
    uint16_t *v;

    tmp.srcIpAddress  = ipv4Header.srcIpAddress;
    tmp.dstIpAddress  = ipv4Header.dstIpAddress;
    tmp.protocol      = ipv4Header.protocol;
    tmp.z             = 0;
    tmp.length        = payloadLen;

    len = sizeof(tmp);
    len = (uint8_t)(len >> 1);

    v = (uint16_t *) &tmp;

    while(len)
    {
        cksm += *v;
        len--;
        v++;
    }

    // wrap the checksum
    cksm = (cksm & 0x0FFFF) + (cksm>>16);

    // Return the resulting checksum
    return (uint16_t) cksm;
}

//jira: CAE_MCU8-5737
static uint8_t getHeaderLen(void)
{
    uint16_t rxptr;
    uint8_t  header_len = 0;
    rxptr = ETH_GetReadPtr();
    header_len = ETH_Read8() & 0x0fu;
    ETH_SetRxByteCount(ETH_GetRxByteCount()+1);
    ETH_SetReadPtr(rxptr);
    return (uint8_t)(header_len<<2) ;
    
}
error_msg IPV4_Packet(void)
{
    uint16_t cksm = 0;
    uint16_t length = 0;
    char msg[40];
    uint8_t hdrLen;

    //calculate the IPv4 checksum
    hdrLen = getHeaderLen();                                 //jira: CAE_MCU8-5737
    cksm = ETH_RxComputeChecksum(hdrLen, 0);
    if (cksm != 0)
    {
        return IPV4_CHECKSUM_FAILS;
    }
    
    ETH_ReadBlock((char *)&ipv4Header, sizeof(ipv4Header_t));
    if(ipv4Header.version != 4)
    {
        return IP_WRONG_VERSION; // Incorrect version number
    }

    ipv4Header.dstIpAddress = ntohl(ipv4Header.dstIpAddress);
    ipv4Header.srcIpAddress = ntohl(ipv4Header.srcIpAddress);

    if(ipv4Header.srcIpAddress == SPECIAL_IPV4_BROADCAST_ADDRESS)
        return DEST_IP_NOT_MATCHED;

    // jira:M8TS-608
    if(ipv4Header.dstIpAddress == ipdb_getAddress() || (ipv4Header.dstIpAddress == IPV4_ZERO_ADDRESS)||
        (ipv4Header.dstIpAddress == SPECIAL_IPV4_BROADCAST_ADDRESS)
            ||((ipdb_getAddress()|CLASS_A_IPV4_REVERSE_BROADCAST_MASK)== ipv4Header.dstIpAddress)                  // jira: MCU8CC-6949
                ||((ipdb_getAddress()|CLASS_B_IPV4_REVERSE_BROADCAST_MASK)== ipv4Header.dstIpAddress)
                    ||((ipdb_getAddress()|CLASS_C_IPV4_REVERSE_BROADCAST_MASK)== ipv4Header.dstIpAddress)
            || (ipv4Header.dstIpAddress == ALL_HOST_MULTICAST_ADDRESS))
    {
        ipv4Header.length = ntohs(ipv4Header.length);

        hdrLen = (uint8_t)(ipv4Header.ihl << 2);

        if(ipv4Header.ihl < 5)
            return INCORRECT_IPV4_HLEN;

        if (ipv4Header.ihl > 5)                                      //jira: CAE_MCU8-5737
        {
            //Do not process the IPv4 Options field
            ETH_Dump((uint16_t)(hdrLen - sizeof(ipv4Header_t)));
        }
        
        switch((ipProtocolNumbers)ipv4Header.protocol)
        {
            case ICMP_TCPIP:
                {
                    // calculate and check the ICMP checksum
                    logMsg("IPv4 RX ICMP", LOG_INFO, LOG_DEST_CONSOLE);
                    if(ipv4Header.dstIpAddress == IPV4_ZERO_ADDRESS)     // jira:M8TS-608
                    {
                        return DEST_IP_NOT_MATCHED;
                    }
                    length = ipv4Header.length - hdrLen;
                    cksm = ETH_RxComputeChecksum(length, 0);

                    if (cksm == 0)
                    {
                        ICMP_Receive(&ipv4Header);
                    }
                    else
                    {
                        sprintf(msg, "icmp wrong cksm : %x",cksm);
                        logMsg(msg, LOG_INFO, LOG_DEST_CONSOLE);
                        return ICMP_CHECKSUM_FAILS;
                    }
                }
                break;
            case UDP_TCPIP:
                // check the UDP header checksum                
                logMsg("IPv4 RX UDP", LOG_INFO, LOG_DEST_CONSOLE);
                length = ipv4Header.length - hdrLen;
                cksm = IPV4_PseudoHeaderChecksum(length);//Calculate pseudo header checksum
                cksm = ETH_RxComputeChecksum(length, cksm); //1's complement of pseudo header checksum + 1's complement of UDP header, data
                UDP_Receive(cksm);
                break;
            default:
                ETH_Dump(ipv4Header.length);
                break;
        }
        return SUCCESS;
    }
    else
    {
        return DEST_IP_NOT_MATCHED;
    }
}

error_msg IPv4_Start(uint32_t destAddress, ipProtocolNumbers protocol)
{
    error_msg ret = ERROR;
    // get the dest mac address
    const mac48Address_t *destMacAddress; // Renamed from macAddress per CAE_MCU8-5648
    uint32_t targetAddress;

    // Check if we have a valid IPadress and if it's different then 127.0.0.1
    if(((ipdb_getAddress() != 0) || (protocol == UDP_TCPIP))
     && (ipdb_getAddress() != 0x7F000001))
    {
        if(((destAddress == SPECIAL_IPV4_BROADCAST_ADDRESS)
             |((destAddress | CLASS_A_IPV4_BROADCAST_MASK) == SPECIAL_IPV4_BROADCAST_ADDRESS)
                |((destAddress | CLASS_B_IPV4_BROADCAST_MASK )== SPECIAL_IPV4_BROADCAST_ADDRESS)
                    |((destAddress | CLASS_C_IPV4_BROADCAST_MASK) == SPECIAL_IPV4_BROADCAST_ADDRESS))==0) // this is NOT a broadcast message
        {
            if( ((destAddress ^ ipdb_getAddress()) & ipdb_getSubNetMASK()) == 0)
            {
                targetAddress = destAddress;
            }
            else
            {
                targetAddress = ipdb_getRouter();
            }
            destMacAddress= ARPV4_Lookup(targetAddress);
            if(destMacAddress == 0)
            {
                ret = ARPV4_Request(targetAddress); // schedule an arp request
                return ret;
            }
        }
        else
        {
            destMacAddress = &broadcastMAC;
        }
        ret = ETH_WriteStart(destMacAddress, ETHERTYPE_IPV4);
        if(ret == SUCCESS)
        {
            ETH_Write16(0x4500); // VERSION, IHL, DSCP, ECN
            ETH_Write16(0); // total packet length
            ETH_Write32(0xAA554000); // My IPV4 magic Number..., FLAGS, Fragment Offset
            ETH_Write8(IPv4_TTL); // TTL
            ETH_Write8((uint8_t) protocol); // protocol
            ETH_Write16(0); // checksum. set to zero and overwrite with correct value
            ETH_Write32(ipdb_getAddress());
            ETH_Write32(destAddress);

            // fill the pseudo header for checksum calculation
            ipv4Header.srcIpAddress = ipdb_getAddress();
            ipv4Header.dstIpAddress = destAddress;
            ipv4Header.protocol = (uint8_t) protocol;
        }
    }
    return ret;
}

error_msg IPV4_Send(uint16_t payloadLength)
{
    uint16_t totalLength;
    uint16_t cksm;
    error_msg ret;

    totalLength = 20 + payloadLength;
    totalLength = ntohs(totalLength);

    //Insert IPv4 Total Length
    ETH_Insert((char *)&totalLength, 2, sizeof(ethernetFrame_t) + offsetof(ipv4Header_t, length));

    cksm = ETH_TxComputeChecksum(sizeof(ethernetFrame_t),sizeof(ipv4Header_t),0);
    //Insert Ipv4 Header Checksum
    ETH_Insert((char *)&cksm, 2, sizeof(ethernetFrame_t) + offsetof(ipv4Header_t,headerCksm));
    ret = ETH_Send();

    return ret;
}

uint16_t IPV4_GetDatagramLength(void)
{
    return ((ipv4Header.length) - sizeof(ipv4Header_t));
}
