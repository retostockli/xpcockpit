/**
  Network layer implementation
	
  Company:
    Microchip Technology Inc.

  File Name:
    network.c

  Summary:
    Network layer handling.

  Description:
    This file provides the network layer implementation for TCP/IP stack.

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

/**
 Section: Included Files
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "network.h"
#include "tcpip_types.h"
#include "arpv4.h"
#include "ipv4.h"
#include "rtcc.h"
#include "ethernet_driver.h"
#include "log.h"
#include "ip_database.h"
#ifdef ENABLE_NETWORK_DEBUG
#define logMsg(msg, msgSeverity, msgLogDest)    logMessage(msg, LOG_KERN, msgSeverity, msgLogDest) 
#else
#define logMsg(msg, msgSeverity, msgLogDest)
#endif

time_t arpTimer;
static void Network_SaveStartPosition(void);
uint16_t networkStartPosition;

const char *network_errors[] = { "ERROR","SUCCESS","LINK_NOT_FOUND","BUFFER_BUSY",
                             "TX_LOGIC_NOT_IDLE","MAC_NOT_FOUND",
                             "IP_WRONG_VERSION","IPV4_CHECKSUM_FAILS",
                             "DEST_IP_NOT_MATCHED","ICMP_CHECKSUM_FAILS",
                             "UDP_CHECKSUM_FAILS","TCP_CHECKSUM_FAILS",
                             "DMA_TIMEOUT","PORT_NOT_AVAILABLE",
                             "ARP_IP_NOT_MATCHED","EAPol_PACKET_FAILURE"};

void Network_Init(void)
{
    ETH_Init();
    ARPV4_Init();
    IPV4_Init();
    rtcc_init();
    Network_WaitForLink();  
    timersInit();
    LOG_Init();
}

void timersInit()
{
    time(&arpTimer);
    arpTimer += 10;  
}

void Network_WaitForLink(void)
{
    while(!ETH_CheckLinkUp()) asm("nop");
}

void Network_Manage(void)
{
    time_t now;
    static time_t nowPv = 0;

    ETH_EventHandler();
    Network_Read(); // handle any packets that have arrived...

    // manage any outstanding timeouts
    time(&now);
    if(now >= arpTimer)
    {
        ARPV4_Update();
        arpTimer = now + 10;
    }    
    if(now > nowPv) // at least 1 second has elapsed
    {
        // is defined as a minimum of 1 seconds in RFC973
    }
    nowPv = now;
}

void Network_Read(void)
{
    ethernetFrame_t header;
    char debug_str[80];

    if(ETH_packetReady())
    {
        ETH_NextPacketUpdate();
        ETH_ReadBlock((char *)&header, sizeof(header));
        header.id.type = ntohs(header.id.type); // reverse the type field
        Network_SaveStartPosition();
        switch (header.id.type)
        {
            case ETHERTYPE_VLAN:
                logMsg("VLAN Packet Dropped", LOG_INFO, (LOG_DEST_CONSOLE|LOG_DEST_ETHERNET));
                break;
            case ETHERTYPE_ARP:
                logMsg("RX ARPV4 Packet", LOG_INFO, (LOG_DEST_CONSOLE|LOG_DEST_ETHERNET));
                ARPV4_Packet();
                break;
            case ETHERTYPE_IPV4:
                logMsg("RX IPV4 Packet", LOG_INFO, (LOG_DEST_CONSOLE|LOG_DEST_ETHERNET));
                IPV4_Packet();
                break;
            case ETHERTYPE_IPV6:
                logMsg("RX IPV6 Packet Dropped", LOG_INFO, (LOG_DEST_CONSOLE|LOG_DEST_ETHERNET));
                break;
            default:
                {
                    long t = header.id.type;
                    if(t < 0x05dc) // this is a length field
                    {
                        sprintf(debug_str,"802.3 length 0x%04lX",t);                    
                    }
                    else
                        sprintf(debug_str,"802.3 type 0x%04lX",t);

                    logMsg(debug_str, LOG_INFO, (LOG_DEST_CONSOLE|LOG_DEST_ETHERNET));
                }
                break;
        }        
        ETH_Flush();
    }
}

static void Network_SaveStartPosition(void)
{
    networkStartPosition = ETH_GetReadPtr();
}

uint16_t Network_GetStartPosition(void)
{    
    return networkStartPosition;
}