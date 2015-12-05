/*! 
 * @file buck.c
 * @author Alex Murray
 *
 * Created on 13 November 2015, 17:10
 */

#include <stdint.h>
#include "drv/buck.h"
#include "drv/hw.h"
#include "core/event.h"
#include <stddef.h>

/* -------------------------------------------------------------------------- */

static uint16_t ADCdata0 = 0;
static uint16_t ADCdata1 = 0;

void buck_enable()
{
    BUCK_EN = 1;
    T1CONbits.TON = 1;      /* start timer */
}

void buck_disable()
{
    BUCK_EN = 0;
    T1CONbits.TON = 0;      /* stop timer */
}

static void buck_timer_init()
{
    /*
     * Target interrupt frequency is 4 kHz
     * 
     * Fcy = 7.37 * 65 / 8 = 59.88125 MHz
     * Prescale 1:64 ~ 936 kHz
     * Using 16-bit timer type B: count to 234 for 4 kHz
     * 
     * 
     * Notes on config:
     *  + Clock source select by default is Fosc / 2
     *  + Default mode is 16-bit mode
     */
    T1CONbits.TON = 0;      /* disable timer during config */
    T1CONbits.TCKPS = 0x02; /* prescale 1:64 */
    PR4 = 234;              /* period match, divide the 936 kHz by 234 to 
                             * reach 250us */
    IFS0bits.T1IF = 0;      /* clear interrupt flag */
    /* IEC0bits.T1IE = 1;       enable timer 4 interrupts */
}

static void buck_gpio_init()
{
    /*
     * UVLO is a digital input signal. Because all pins are configured as analog
     * inputs by default, clear analog flag.
     */
    ANSELC &= ~0x0200;      /* bit 9 */
    
    /* buck enable is a digital output */
    TRISAbits.TRISA2 = 0;   /* output */
    
    /* configure BUCK_UVLO to trigger an interrupt on a rising edge */
    RPINR1bits.INT2R = 57;  /* assign INT2 to pin RP57 (BUCK_UVLO) */
    INTCON2bits.INT2EP = 0; /* interrupt on rising edge */
    IFS1bits.INT2IF = 0;    /* clear interrupt flag */
    IEC1bits.INT2IE = 1;    /* enable INT2 interrupt */
}

static void buck_pga_init()
{
    // Configure PGA1N2 to be used as analog input.
    ANSELBbits.ANSB2 = 1;
    
    PGA1CONbits.PGAEN = 0; //Disable PGA1
    PGA1CONbits.SELPI = 0; //PGA1P1 as positive input
    PGA1CONbits.SELNI = 1; //PGA1N2 as negative input
    PGA1CONbits.GAIN = 5;  //32x PGA Gain
    PGA1CONbits.PGAEN = 1; //Enable PGA1
}

static void EnableAndCalibrate()
{
    // Set initialisation time to maximum
    ADCON5Hbits.WARMTIME = 15;
    // Turn on ADC module
    ADCON1Lbits.ADON = 1;
    
    // Turn on analog power for dedicated core 0
    ADCON5Lbits.C0PWR = 1;
    // Wait when the core 0 is ready for operation
    while(ADCON5Lbits.C0RDY == 0);
    // Turn on digital power to enable triggers to the core 0
    ADCON3Hbits.C0EN = 1;
    
    // Turn on analog power for dedicated core 1
    ADCON5Lbits.C1PWR = 1;
    // Wait when the core 1 is ready for operation
    while(ADCON5Lbits.C1RDY == 0);
    // Turn on digital power to enable triggers to the core 1
    ADCON3Hbits.C1EN = 1;
    
    // Enable calibration for the dedicated core 0
    ADCAL0Lbits.CAL0EN = 1;
    // Single-ended input calibration
    ADCAL0Lbits.CAL0DIFF = 0;
    // Start calibration
    ADCAL0Lbits.CAL0RUN = 1;
    // Poll for the calibration end
    while(ADCAL0Lbits.CAL0RDY == 0);
    // Differential input calibration
    ADCAL0Lbits.CAL0DIFF = 1;
    // Start calibration
    ADCAL0Lbits.CAL0RUN = 1;
    // Poll for the calibration end
    while(ADCAL0Lbits.CAL0RDY == 0);
    // End the core 0 calibration
    ADCAL0Lbits.CAL0EN = 0;
    
    // Enable calibration for the dedicated core 1
    ADCAL0Lbits.CAL1EN = 1;
    // Single-ended input calibration
    ADCAL0Lbits.CAL1DIFF = 0;
    // Start calibration
    ADCAL0Lbits.CAL1RUN = 1;
    // Poll for the calibration end
    while(ADCAL0Lbits.CAL1RDY == 0);
    // Differential input calibration
    ADCAL0Lbits.CAL1DIFF = 1;
    // Start calibration
    ADCAL0Lbits.CAL1RUN = 1;
    // Poll for the calibration end
    while(ADCAL0Lbits.CAL1RDY == 0);
    // End the core 1 calibration
    ADCAL0Lbits.CAL1EN = 0;
}

static void buck_adc_init()
{
    // ADC INITIALIZATION
    // Configure the I/O pins to be used as analog inputs.
    ADCON4Hbits.C0CHS = 2; //PGA1
    ADCON4Hbits.C1CHS = 0; //AN1
    
    // Configure the common ADC clock.
    ADCON3Hbits.CLKSEL = 2; // clock from FRC oscillator
    ADCON3Hbits.CLKDIV = 0; // no clock divider (1:1)
    // Configure the cores? ADC clock.
    ADCORE0Hbits.ADCS = 0; // clock divider (1:2)
    ADCORE1Hbits.ADCS = 0; // clock divider (1:2)
    // Configure the ADC reference sources.
    ADCON3Lbits.REFSEL = 0; // AVdd as voltage reference
    // Configure the integer of fractional output format.
    ADCON1Hbits.FORM = 0; // integer format
    // Select single-ended input configuration and unsigned output format.
    ADMOD0Lbits.SIGN0 = 0; // AN0/RA0
    ADMOD0Lbits.DIFF0 = 0; // AN0/RA0
    ADMOD0Lbits.SIGN1 = 0; // AN1/RA1
    ADMOD0Lbits.DIFF1 = 0; // AN1/RA1
    
    // Enable and calibrate the module.
    EnableAndCalibrate(); // See Example 5-1
    
    // Set same trigger source for all inputs to sample signals simultaneously.
    ADTRIG0Lbits.TRGSRC0 = 12; // timer 1 for AN0
    ADTRIG0Lbits.TRGSRC1 = 12; // timer 1 for AN1  
    
    // Configure and enable ADC interrupts.
    ADIELbits.IE0 = 1; // enable interrupt for AN0
    ADIELbits.IE1 = 1; // enable interrupt for AN1
    _ADCAN0IF = 0; // clear interrupt flag for AN0
    _ADCAN0IE = 1; // enable interrupt for AN0
    _ADCAN1IF = 0; // clear interrupt flag for AN1
    _ADCAN1IE = 1; // enable interrupt for AN1
}

static void buck_dac_init()
{
    CMP1CONbits.DACOE = 1;
    CMP1CONbits.RANGE = 1;
    CMP1DACbits.CMREF = 0;
    CMP1CONbits.CMPON = 1;
    
    CMP2CONbits.DACOE = 1;
    CMP2CONbits.RANGE = 1;
    CMP2DACbits.CMREF = 0;
    CMP2CONbits.CMPON = 1;
}

void buck_init(void)
{
    buck_gpio_init();
    buck_timer_init();
    buck_pga_init();
    buck_adc_init();
    buck_dac_init();
}

_Q16 buck_get_voltage()
{
    _Q16 value = (_Q16)ADCdata1 * 845;
    return value;
}

_Q16 buck_get_current()
{
    _Q16 value = ((_Q16)ADCdata0 - 1862) * 330;
    return value;    
}

void buck_set_voltage(_Q16 voltage)
{
    _Q16 max_v = 1507328; // 23V
    _Q16 div = 1769472; // 18 * 1.5V
    
    int16_t value = _Q16div(max_v - voltage, div) >> 4;
    if(value < 0){
        value = 0;
    }else if(value > 0x0fff){
        value = 0x0fff;
    }
    
    CMP1DACbits.CMREF = value;
}

void buck_set_current(_Q16 current)
{
    _Q16 max_i = 327680; // 5A
    
    int16_t value = _Q16div(current, max_i) >> 4;
    
    if(value < 0){
        value = 0;
    }else if(value > 0x0fff){
        value = 0x0fff;
    }
 
    CMP2DACbits.CMREF = value;
}


// ADC AN0 ISR
void _ISR_NOPSV _ADCAN0Interrupt(void)
{
    ADCdata0 = ADCBUF0; // read conversion result
    _ADCAN0IF = 0; // clear interrupt flag
}

// ADC AN1 ISR
void _ISR_NOPSV _ADCAN1Interrupt(void)
{
    ADCdata1 = ADCBUF1; // read conversion result
    _ADCAN1IF = 0; // clear interrupt flag
}

/* -------------------------------------------------------------------------- */
/* Called when an UVLO event occurs */
void _ISR_NOPSV _INT2Interrupt(void)
{
    buck_disable();
    event_post(EVENT_UVLO, 0);
    
    /* clear interrupt flag */
    IFS1bits.INT2IF = 0;
}

/* -------------------------------------------------------------------------- */
/* Unit Tests */
/* -------------------------------------------------------------------------- */

#ifdef TESTING

#include "gmock/gmock.h"

using namespace ::testing;

class buck : public Test
{
    virtual void SetUp()
    {
        buck_init();
        ADCdata0 = 0;
        ADCdata1 = 0;
    }

    virtual void TearDown()
    {
        event_deinit();
    }
};

/* -------------------------------------------------------------------------- */
TEST_F(buck, this_is_a_test)
{
    EXPECT_THAT(1, Eq(1));
}

#endif /* TESTING */

