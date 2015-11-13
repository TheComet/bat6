/*! 
 * @file lcd.c
 * @author Alex Murray
 *
 * Created on 12 November 2015, 14:29
 */

#include "lcd.h"
#include "hw.h"

/* -------------------------------------------------------------------------- */
void __attribute__((__interrupt__)) _CNInterrupt(void)
{
    IFS1bits.CNIF = 0;
}
