/*! 
 * @file lcd.c
 * @author Alex Murray
 *
 * Created on 12 November 2015, 14:29
 */

#include "lcd.h"
#include "hw.h"

/* -------------------------------------------------------------------------- */
void _ISR_NOPSV _CNInterrupt(void)
{
    IFS1bits.CNIF = 0;
}
