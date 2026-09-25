/**
  TMR3 Generated Driver API Header File

  @Company
    Microchip Technology Inc.

  @File Name
    tmr3.h

  @Summary
    This is the generated header file for the TMR3 driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs for driver for TMR3.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC18F86J60
        Driver Version    :  2.01
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.36 and above
        MPLAB 	          :  MPLAB X 6.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#ifndef TMR3_H
#define TMR3_H

/**
  Section: Included Files
*/

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif

#define TMR3_INTERRUPT_TICKER_FACTOR    1

/**
  Section: TMR3 APIs
*/

/**
  @Summary
    Initializes the TMR3

  @Description
    This routine initializes the TMR3.
    This routine must be called before any other TMR3 routine is called.
    This routine should only be called once during system initialization.

  @Preconditions
    None

  @Param
    None

  @Returns
    None

  @Comment
    

  @Example
    <code>
    TMR3_Initialize();
    </code>
*/
void TMR3_Initialize(void);

/**
  @Summary
    Start TMR3

  @Description
    This routine is used to  Start TMR3.

  @Preconditions
    The TMR3_Initialize() routine should be called
    prior to use this routine.

  @Param
    None

  @Returns
    None

  @Example
    <code>
    // Initialize TMR3 module

    // Start TMR3
    TMR3_StartTimer();

    while(1)
    {
    }
    </code>
 */
void TMR3_StartTimer(void);

/**
  @Summary
    Stop TMR3

  @Description
    This routine is used to  Stop TMR3.

  @Preconditions
    The TMR3_Initialize() routine should be called
    prior to use this routine.

  @Param
    None

  @Returns
    None

  @Example
    </code>
    TMR3_Initialize();

    TMR3_StartTimer();
    if(TMR3_HasOverflowOccured())
    {
        TMR3_StopTimer();
    }
    <code>
 */
void TMR3_StopTimer(void);

/**
  @Summary
    Read TMR3 register.

  @Description
    This routine is used to  Read TMR3 register.

  @Preconditions
    The TMR3_Initialize() routine should be called
    prior to use this routine.

  @Param
    None

  @Returns
    TMR3 value at the time of the function call read as a 16-bit value

  @Example
    <code>
    uint16_t timerVal = 0;

    TMR3_Initialize();

    TMR3_StartTimer();

    // some delay or code
    TMR3_StopTimer();

    timerVal = TMR3_ReadTimer();
    </code>
 */
uint16_t TMR3_ReadTimer(void);

/**
  @Summary
    Write TMR3 register.

  @Description
    This routine is used to Write TMR3 register.

  @Preconditions
    The TMR3_Initialize() routine should be called
    prior to use this routine.

  @Param
    timerVal : Timer value to be loaded

  @Returns
    None

  @Example
    <code>
    TMR3_Initialize();
    TMR3_WriteTimer(0x055);
    TMR3_StartTimer();
    </code>
 */
void TMR3_WriteTimer(uint16_t timerVal);

/**
  @Summary
    Reload TMR3 register.

  @Description
    This routine is used to reload TMR3 register.

  @Preconditions
    The TMR3_Initialize() routine should be called
    prior to use this routine.

  @Param
    None

  @Returns
    None

  @Example
    <code>
    TMR3_Initialize();
    TMR3_StartTimer();

    if(TMR3_HasOverflowOccured())
     {
        TMR3_StopTimer();
     }

     TMR3_Reload();}
     </code>
*/
void TMR3_Reload(void);

/**
  @Summary
    Implements ISR

  @Description
    This routine is used to implement the ISR for the interrupt-driven
    implementations.

  @Returns
    None

  @Param
    None
*/
void TMR3_ISR(void);

/**
  @Summary
    CallBack function.

  @Description
    This routine is called by the Interrupt Manager.

  @Preconditions
    None

  @Param
    None

  @Returns
    None
*/
void TMR3_CallBack(void);

/**
  @Summary
    Set Timer Interrupt Handler

  @Description
    This sets the function to be called during the ISR

  @Preconditions
    Initialize  the TMR3 module with interrupt before calling this.

  @Param
    Address of function to be set

  @Returns
    None
*/
 void TMR3_SetInterruptHandler(void (* InterruptHandler)(void));

/**
  @Summary
    Timer Interrupt Handler

  @Description
    This is a function pointer to the function that will be called during the ISR

  @Preconditions
    Initialize  the TMR3 module with interrupt before calling this isr.

  @Param
    None

  @Returns
    None
*/
extern void (*TMR3_InterruptHandler)(void);

/**
  @Summary
    Default Timer Interrupt Handler

  @Description
    This is the default Interrupt Handler function

  @Preconditions
    Initialize  the TMR3 module with interrupt before calling this isr.

  @Param
    None

  @Returns
    None
*/
void TMR3_DefaultInterruptHandler(void);

#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif

#endif // TMR3_H
/**
 End of File
*/

