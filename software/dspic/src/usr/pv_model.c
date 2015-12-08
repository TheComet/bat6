/*! 
 * @file pv_model.c
 * @author Alex Murray
 *
 * Created on 17 November 2015, 20:29
 */

#include <stdint.h>
#include "usr/pv_model.h"

/* -------------------------------------------------------------------------- */

static uint16_t _fp_exp(const uint16_t exponent){
    /* 
     * calculates exp using a spline approximation
     * exponent range: 0 to ln(2) 
     * result range: 1 to 2
     */
    
    static const uint16_t spline_coefs[8][3] = {
        {5702,    16374,    32768},    
        {6217,    17856,    35734},    
        {6780,    19472,    38968},    
        {7394,    21234,    42495},    
        {8063,    23156,    46341},    
        {8793,    25252,    50535},    
        {9588,    27537,    55109},    
        {10458,   30029,    60097}        
    };
    
    const uint16_t spline_index = exponent >> 13;
    const uint32_t delta = exponent << 3;
    const uint32_t delta2 = (delta * delta) >> 16;
    const uint32_t delta3 = (delta * delta2) >> 16;
    
    uint32_t sum = spline_coefs[spline_index][2];
    sum += delta * spline_coefs[spline_index][2] >> 16;
    sum += delta2 * spline_coefs[spline_index][1] >> 16;
    sum += delta3 * spline_coefs[spline_index][0] >> 16;
    
    return sum >> 1;
}

static uint16_t fp_exp(const int16_t exponent){
    /*
     * calculates exp using the formula exp(x) = 2^k * exp(x - k * ln(2))
     * range -16 to 1 format: S4.11;
     * result range 0 to 2 format: Q1.15
     */
    
    int32_t num = (int32_t)exponent * 47274; // 47274 = 1/ln(2) * 2^15
    int16_t k = num >> 26; // 26 = 11 + 15; 
    uint16_t x = (num - (((int32_t)k)<<26)) >> 10; 
    
    if(k > 0){
        //Overflow return max
        return 0xffff;
    }
   
    return _fp_exp(x) >> (-k);
}

static uint16_t Io_rel(pv_cell_t* cell, const uint16_t vd){
    /* calculates exp((vd-voc)/vt) */
    const int16_t diff = vd - cell->voc; //
    const int16_t exponent = ((int32_t)diff << 13) / cell->vt;
    return fp_exp(exponent);
}

static uint16_t Id(pv_cell_t* cell, const uint16_t vd){
    const int16_t io = Io_rel(cell, vd);
    const int16_t irel = (cell->g >> 1) - io;
    const int16_t id = ((int32_t)cell->isc * irel) >> 16;
    return id;
}

uint16_t calc_voltage(pv_cell_t* cell, uint16_t voltage_is, int16_t current_is){
    /*calculates the new voltage depending on the measured voltage and current
     *voltage format: Q5.11
     *current format: Q3.13 */
    const int16_t Idiff = Id(cell, voltage_is) - current_is;
    const int16_t Udiff = (int32_t)Idiff * voltage_is / current_is;
    
    return voltage_is + Udiff;
}