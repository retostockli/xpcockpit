/* 
 * File:   common.h
 * Author: stockli
 *
 * Created on September 1, 2026, 8:43 AM
 */

#ifndef COMMON_H
#define	COMMON_H

#include <stdint.h>
#include <stdbool.h>

// TURN OFF ANY DEBUGGING IN OPERATIONAL USE IN ORDER TO AVOID UDP TIME LAG AND TIMING PROBLEMS!

#define DEBUG_UART 1   /* Send Debug Output to UART */
#define DEBUG_UDP 0    /* Send Debug Output to Ethernet via UDP */

#define SENDMSGLEN 30          /* number of bytes in sent UDP packet */
#define RECVMSGLEN 28          /* number of bytes in received UDP packet */
#define MAXANALOGINPUTS 5      /* 5 on master */
#define MAXANALOGINPUTS_I2C 10 /* 10 on daughter */
#define MAXINPUTS 64           /* 64 on master */
#define MAXINPUTS_I2C 64    /* 2x64 on daughters */
#define MAXOUTPUTS 64          /* 64 on master */
#define MAXOUTPUTS_I2C 64   /* 2x64 on daughters */
#define MAXDISPLAYS 32         /* 32 on master */
#define MAXDISPLAYS_I2C 32  /* 2x32 on daughters */
#define MAXBRIGHTNESS 15   /* 0-15 Brightness of 7 segment displays */
#define MAXSERVOS_I2C 14       /* 14 on daughter */
#define MAXSAVE 9             /* maximum number of history values in data structure */
#define INPUTSINITVAL 0         /* initial value of inputs upon startup */
#define DISPLAYSINITVAL 0      /* initial value of displays upon startup (BLANK) */
#define OUTPUTSINITVAL 0       /* initial value of outputs upon startup (OFF) */
#define SERVOSINITVAL 0        /* initial value of servos upon startup (PARK) */
#define ANALOGINPUTNBITS 10    /* number of bits of analog inputs */


/* Storage variables for inputs / outputs */
uint8_t outputs[MAXOUTPUTS/8];
uint8_t outputs_save[MAXOUTPUTS/8];
uint8_t outputs_i2c1[MAXOUTPUTS_I2C/8];
uint8_t outputs_i2c1_save[MAXOUTPUTS_I2C/8];
uint8_t outputs_i2c2[MAXOUTPUTS_I2C/8];
uint8_t outputs_i2c2_save[MAXOUTPUTS_I2C/8];
uint8_t inputs[MAXINPUTS/8];
uint8_t inputs_save[MAXINPUTS/8];
uint8_t inputs_i2c1[MAXINPUTS_I2C/8];
uint8_t inputs_i2c1_save[MAXINPUTS_I2C/8];
uint8_t inputs_i2c2[MAXINPUTS_I2C/8];
uint8_t inputs_i2c2_save[MAXINPUTS_I2C/8];
uint8_t displays[MAXDISPLAYS];
uint8_t displays_save[MAXDISPLAYS];
uint8_t displays_i2c1[MAXDISPLAYS_I2C];
uint8_t displays_i2c1_save[MAXDISPLAYS_I2C];
uint8_t displays_i2c2[MAXDISPLAYS_I2C];
uint8_t displays_i2c2_save[MAXDISPLAYS_I2C];
uint8_t brightness[MAXDISPLAYS/8];
uint8_t brightness_save[MAXDISPLAYS/8];
uint8_t brightness_i2c1[MAXDISPLAYS_I2C/8];
uint8_t brightness_i2c1_save[MAXDISPLAYS_I2C/8];
uint8_t brightness_i2c2[MAXDISPLAYS_I2C/8];
uint8_t brightness_i2c2_save[MAXDISPLAYS_I2C/8];
uint16_t analoginputs[MAXANALOGINPUTS][MAXSAVE];
uint16_t analoginputs_save[MAXANALOGINPUTS];
uint16_t analoginputs_median[MAXANALOGINPUTS];
uint16_t analoginputs_i2c[MAXANALOGINPUTS_I2C][MAXSAVE];
uint16_t analoginputs_i2c_save[MAXANALOGINPUTS_I2C];
uint16_t analoginputs_i2c_median[MAXANALOGINPUTS_I2C];
bool firstanalogread;

/* Storage variable for selected daughter cards */
uint8_t daughtercards;

void init_data(void);
void copy_data(void);

#endif	/* COMMON_H */

