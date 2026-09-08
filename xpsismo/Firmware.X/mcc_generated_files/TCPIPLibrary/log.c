/**
 Message logging
	
  Company:
    Microchip Technology Inc.

  File Name:
    log.c

  Summary:
    Facilitating message logging

  Description:
    This source file provides the implementation of the APIs for message logging.

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
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "rtcc.h"
#include "log.h"
#include "log_console.h"
#include "log_syslog.h"

#define	LOG_PRIMASK	0x07	/* mask to extract priority part (internal) */
/* extract priority */
#define	LOG_PRI(p)	((p) & LOG_PRIMASK)
#define LOG_NFACILITIES 24      /* current number of facilities */
 #define LOG_FACMASK    0x03f8  /* mask to extract facility part */
/* facility of pri */
#define LOG_FAC(p)              (((p) & LOG_FACMASK) >> 3)	/* facility of pri */
#define	LOG_MAKEPRI(fac, pri)	(((fac) << 3) | (pri))

/*arguments to setlogmask */
#define	LOG_MASK(pri)	(1 << (pri))            /* mask for one priority */
#define	LOG_UPTO(pri)	((1 << ((pri)+1)) - 1)	/* all priorities through pri */
#define LOG_NILVALUE "-"

LOG_SEVERITY limit[LOG_LAST]; // one limit per facility

const logFields_t severityThresholdTable[] = {
    {  LOG_KERN,   LOG_INFO },
    {  LOG_DTLS,   LOG_NOTICE },
    {  LOG_ECC,   LOG_NOTICE },
    {  LOG_DAEMON, LOG_INFO },
    {  LOG_AUTH,   LOG_NOTICE },
    {  LOG_SYSLOG, LOG_NOTICE },
    {  LOG_LPR,    LOG_INFO },
    {  LOG_NEWS,   LOG_NOTICE },
    {  LOG_TFTP,   LOG_INFO },
    {  LOG_UUID,   LOG_NOTICE },
    {  LOG_COAP,   LOG_NOTICE },
    {  LOG_FTP,    LOG_NOTICE },
    {  LOG_NTP,    LOG_INFO },
    {  LOG_HTTP,   LOG_NOTICE },
    {  LOG_SNMP,   LOG_NOTICE },
    {  LOG_RTCC,   LOG_NOTICE },
    {  LOG_CBOR,   LOG_INFO },
    {  LOG_UDP,    LOG_NOTICE },
    {  LOG_TCP,    LOG_NOTICE },
    {  LOG_DHCP,   LOG_INFO },
    {  LOG_DNS,    LOG_NOTICE },
    {  LOG_LLDP,   LOG_INFO },
    {  LOG_LINK,   LOG_NOTICE },
    
    {  LOG_LAST,   LOG_NOTICE }
};
 
//Testing for externals property in SVN
void LOG_Init(void)
{
    unsigned long t = (unsigned long)time(0); //jira: CAE_MCU8-5647
    printf("\n\nStarting Syslog at %lu\n",t);
    for(uint8_t x=0;x<LOG_LAST;x++) //jira: CAE_MCU8-5647
    {
        limit[(uint8_t)severityThresholdTable[x].logFacility] = severityThresholdTable[x].severityThreshold;   //jira: CAE_MCU8-5647
    }
}


void logMessage(const char *message, LOG_FACILITY facility, LOG_SEVERITY severity, uint8_t logDest)
{
    uint8_t priVal;
    
    priVal = (uint8_t) LOG_MAKEPRI(facility, severity);
    
    if((severity <= limit[(uint8_t)severityThresholdTable[(uint8_t)facility].logFacility]) && // only report messages that are more severe than the limit //jira: CAE_MCU8-5647
            (priVal <= 191)) // RFC3164 requires PRIVAL 0-191
    {
        if(LOG_ETHERNET_MASK(logDest) == LOG_DEST_ETHERNET)
        {       
            logSyslog(message, priVal);
        }
        if(LOG_CONSOLE_MASK(logDest) == LOG_DEST_CONSOLE)
        {
            logConsole(message, priVal);
        }
    }
}