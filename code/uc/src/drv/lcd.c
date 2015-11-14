/*! 
 * @file lcd.c
 * @author Alex Murray
 *
 * Created on 12 November 2015, 14:29
 */

#include "drv/lcd.h"
#include "drv/hw.h"

/* -------------------------------------------------------------------------- */
/* master event interrupt */
void _ISR_NOPSV _MI2C2Interrupt(void)
{
    
}

/* -------------------------------------------------------------------------- */
/* slave event interrupt */
void _ISR_NOPSV _SI2C2Interrupt(void)
{
    
}
