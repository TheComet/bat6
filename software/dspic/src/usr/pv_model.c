/*!
 * @file pv_model.c
 * @author Alex Murray
 *
 * Created on 17 November 2015, 20:29
 */


#include <libq.h>
#include <stdint.h>
#include "usr/pv_model.h"

_Q16 _Q16exp(_Q16);
_Q16 _Q16mpy(_Q16, _Q16);

#ifdef TESTING
#ifndef _libq_h_
#error blub
#endif
#endif

/* -------------------------------------------------------------------------- */
static _Q16 _fp_exp(const _Q16 exponent)
{
    /*
     * calculates exp using a spline approximation
     * exponent range: 0 to ln(2)
     * result range: 1 to 2
     */

    static const _Q16 spline_coefs[8][3] = {
        {5702,    16374,    32768},
        {6217,    17856,    35734},
        {6780,    19472,    38968},
        {7394,    21234,    42495},
        {8063,    23156,    46341},
        {8793,    25252,    50535},
        {9588,    27537,    55109},
        {10458,   30029,    60097}
    };

    const _Q16 spline_index = exponent >> 13;
    const uint32_t delta = exponent << 3;
    const uint32_t delta2 = (delta * delta) >> 16;
    const uint32_t delta3 = (delta * delta2) >> 16;

    uint32_t sum = spline_coefs[spline_index][2];
    sum += delta * spline_coefs[spline_index][2] >> 16;
    sum += delta2 * spline_coefs[spline_index][1] >> 16;
    sum += delta3 * spline_coefs[spline_index][0] >> 16;

    return sum >> 1;
}

static _Q16 fp_exp(const _Q16 exponent)
{
    /*
     * calculates exp using the formula exp(x) = 2^k * exp(x - k * ln(2))
     * range -16 to 1 format: S4.11;
     * result range 0 to 2 format: Q1.15
     */

    int32_t num = (int32_t)exponent * 47274; // 47274 = 1/ln(2) * 2^15
    _Q16 k = num >> 26; // 26 = 11 + 15;
    _Q16 x = (num - (((int32_t)k)<<26)) >> 10;

    if(k > 0){
        //Overflow return max
        return 0xffff;
    }

    return _fp_exp(x) >> (-k);
}

/* -------------------------------------------------------------------------- */
static _Q16 Io_rel(const struct pv_cell_t* cell, const _Q16 vd)
{
    /* calculates exp((vd-voc)/vt) */
    const _Q16 diff = vd - cell->voc; //
    const _Q16 exponent = ((int32_t)diff << 13) / cell->vt;
    return fp_exp(exponent);
}

/* -------------------------------------------------------------------------- */
static _Q16 Id(const struct pv_cell_t* cell, const _Q16 vd)
{
    const _Q16 io = Io_rel(cell, vd);
    const _Q16 irel = (cell->g >> 1) - io;
    const _Q16 id = ((int32_t)cell->isc * irel) >> 16;
    return id;
}

/* -------------------------------------------------------------------------- */
_Q16 calc_voltage(const struct pv_cell_t* cell,
                      const _Q16 voltage_is,
                      const _Q16 current_is)
{
    /*calculates the new voltage depending on the measured voltage and current
     *voltage format: Q5.11
     *current format: Q3.13 */
    const _Q16 Idiff = Id(cell, voltage_is) - current_is;
    const _Q16 Udiff = (int32_t)Idiff * voltage_is / current_is;

    return voltage_is + Udiff;
}

/* -------------------------------------------------------------------------- */
unsigned char model_cell_create(void)
{
    return 0;
}

/* -------------------------------------------------------------------------- */
unsigned char model_cell_destroy(void)
{
    return 0;
}

/* -------------------------------------------------------------------------- */
unsigned char model_select_cell(unsigned char cell_id)
{
    return 0;
}

/* -------------------------------------------------------------------------- */
void model_set_open_circuit_voltage(_Q16 voc)
{
}

/* -------------------------------------------------------------------------- */
void model_set_short_circuit_current(_Q16 isc)
{
}

/* -------------------------------------------------------------------------- */
void model_set_thermal_voltage(_Q16 vt)
{
}

/* -------------------------------------------------------------------------- */
void model_set_relative_solar_irridation(_Q16 g)
{
}

/* -------------------------------------------------------------------------- */
_Q16 model_get_open_circuit_voltage(void)
{
    return 0;
}

/* -------------------------------------------------------------------------- */
_Q16 model_get_short_circuit_current(void)
{
    return 0;
}

/* -------------------------------------------------------------------------- */
_Q16 model_get_thermal_voltage(void)
{
    return 0;
}

/* -------------------------------------------------------------------------- */
_Q16 model_get_relative_solar_irridation(void)
{
    return 0;
}

/* -------------------------------------------------------------------------- */
/* Unit Tests */
/* -------------------------------------------------------------------------- */

#ifdef TESTING

#include "gmock/gmock.h"

using namespace ::testing;

/* -------------------------------------------------------------------------- */
class pv_model : public Test
{
    virtual void SetUp() {}
    virtual void TearDown() {}
};

/* -------------------------------------------------------------------------- */
TEST_F(pv_model, example_test)
{
    EXPECT_THAT(1, Eq(1));
}

#endif /* TESTING */
