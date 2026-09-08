/* 
 * File:   gpio.h
 * Author: stockli
 *
 * Created on September 1, 2026, 8:20 AM
 */

#ifndef GPIO_H
#define	GPIO_H

#include <xc.h> // include processor files - each processor file is guarded.  

void write_outputs(void);
void read_inputs(void);
void read_analoginputs(void);
void init_displays(void);
void write_displays(void);

#endif	/* GPIO_H */

