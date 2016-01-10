/*!
 * @file pv_model.c
 * @author Alex Murray
 *
 * Created on 17 November 2015, 20:29
 */


#include <libq.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "usr/pv_model.h"

struct cell_t
{
    struct cell_t* next;
    unsigned char id;
    struct pv_cell_t params;
};

static struct cell_t* active_panel = NULL;

static _Q16 global_vt = (_Q16)(293 * 65536);
static _Q16 global_g  = (_Q16)(100 * 65536);

/* -------------------------------------------------------------------------- */
static unsigned char uid_counter = 0;
static unsigned char generate_unique_identifier(void)
{
    unsigned char uid = uid_counter++;
    if(uid == 0)
        return uid_counter++;
    return uid;
}


/* -------------------------------------------------------------------------- */
static _Q16 Io_rel(const struct pv_cell_t* cell, const _Q16 vd)
{
    /* calculates exp((vd-voc)/vt) */
    const _Q16 diff = vd - cell->voc; //
    const _Q16 exponent = _Q16div(diff, cell->vt);
    return _Q16exp(exponent);
}

/* -------------------------------------------------------------------------- */
static _Q16 Id(const struct pv_cell_t* cell, const _Q16 vd)
{
    const _Q16 io = Io_rel(cell, vd);
    const _Q16 irel = cell->g - io;
    const _Q16 id = _Q16mpy(cell->isc, irel);
    return id;
}

static _Q16 Iload(const _Q16 vd, const _Q16 I_is, const _Q16 U_is){
    return _Q16div(_Q16mpy(I_is, vd), U_is);
}

/* -------------------------------------------------------------------------- */
_Q16 calc_voltage(const struct pv_cell_t* cell,
                      const _Q16 U_is,
                      const _Q16 I_is)
{
    /*calculates the new voltage depending on the measured voltage and current
     *voltage format: Q5.11
     *current format: Q3.13 */
    
    _Q16 vd_min = 0;
    _Q16 vd_max = cell->voc;
    
    uint16_t i; 
    for(i = 0; i < 10; i++){
        _Q16 vd = (vd_min + vd_max) >> 1;
        if(Id(cell, vd) < Iload(vd, I_is, U_is)){
            vd_max = vd;
        }else{
            vd_min = vd;
        }
    }
    
    return (vd_min + vd_max) >> 1;
}

/* -------------------------------------------------------------------------- */
unsigned char model_cell_add(void)
{
    /* allocate new cell */
    struct cell_t* cell = (struct cell_t*)malloc(sizeof(struct cell_t));
    if(!cell)
        return 0;
    memset(cell, 0, sizeof *cell);
    
    /* 
     * Link cell into the global list of cells. Since the order doesn't matter,
     * we insert the cell into the beginning of the list because that's a
     * lot easier.
     */
    cell->next = active_panel;
    active_panel = cell;
    
    cell->id = generate_unique_identifier();
    return cell->id;
}

/* -------------------------------------------------------------------------- */
unsigned char model_cell_remove(unsigned char cell_id)
{
    struct cell_t* cell = active_panel;
    struct cell_t* parent_cell = NULL;
    while(cell)
    {
        if(cell->id == cell_id)
        {
            /*
             * If this is not the first cell in the list (i.e. it does not equal
             * "active_panel") then unlink this cell by setting the previous
             * cell's next cell to this cell's next cell.
             * 
             * If this is the first cell in the list, then we have to point
             * active_panel to the next cell in line, which then becomes the new
             * first cell in the list.
             */
            if(parent_cell)
                parent_cell->next = cell->next;
            else
                active_panel = cell->next;
            
            /* Cell is unlinked, free to de-allocate */
            free(cell);
            
            return 1;
        }
        
        parent_cell = cell;
        cell = cell->next;
    }
    
    return 0;
}

/* -------------------------------------------------------------------------- */
void model_cell_remove_all(void)
{
    while(active_panel)
    {
        struct cell_t* next = active_panel->next;
        free(active_panel);
        active_panel = next;
    }
}

/* -------------------------------------------------------------------------- */
static struct cell_t* cell_iterator = NULL;
unsigned char model_cell_begin_iteration(void)
{
    cell_iterator = active_panel;
    if(cell_iterator)
        return cell_iterator->id;
    return 0;
}

/* -------------------------------------------------------------------------- */
unsigned char model_cell_get_next(void)
{
    if(cell_iterator)
        cell_iterator = cell_iterator->next;
    if(cell_iterator)
        return cell_iterator->id;
    return 0;
}

/* -------------------------------------------------------------------------- */
void model_set_global_thermal_voltage(_Q16 vt)
{
    global_vt = vt;
}

/* -------------------------------------------------------------------------- */
void model_set_global_relative_solar_irradiation(_Q16 g)
{
    global_g = g;
}

/* -------------------------------------------------------------------------- */
_Q16 model_get_global_thermal_voltage(void)
{
    return global_vt;
}

/* -------------------------------------------------------------------------- */
_Q16 model_get_global_relative_solar_irradiation(void)
{
    return global_g;
}

/* -------------------------------------------------------------------------- */
void model_set_open_circuit_voltage(unsigned char cell_id, _Q16 voc)
{
    struct cell_t* cell;
    for(cell = active_panel; cell; cell = cell->next)
        if(cell->id == cell_id)
        {
            cell->params.voc = voc;
            return;
        }
}

/* -------------------------------------------------------------------------- */
void model_set_short_circuit_current(unsigned char cell_id, _Q16 isc)
{
    struct cell_t* cell;
    for(cell = active_panel; cell; cell = cell->next)
        if(cell->id == cell_id)
        {
            cell->params.isc = isc;
            return;
        }
}

/* -------------------------------------------------------------------------- */
void model_set_thermal_voltage(unsigned char cell_id, _Q16 vt)
{
    struct cell_t* cell;
    for(cell = active_panel; cell; cell = cell->next)
        if(cell->id == cell_id)
        {
            cell->params.vt = vt;
            return;
        }
}

/* -------------------------------------------------------------------------- */
void model_set_relative_solar_irradiation(unsigned char cell_id, _Q16 g)
{
    struct cell_t* cell;
    for(cell = active_panel; cell; cell = cell->next)
        if(cell->id == cell_id)
        {
            cell->params.g = g;
            return;
        }
}

/* -------------------------------------------------------------------------- */
_Q16 model_get_open_circuit_voltage(unsigned char cell_id)
{
    struct cell_t* cell;
    for(cell = active_panel; cell; cell = cell->next)
        if(cell->id == cell_id)
            return cell->params.voc;
    return 0;
}

/* -------------------------------------------------------------------------- */
_Q16 model_get_short_circuit_current(unsigned char cell_id)
{
    struct cell_t* cell;
    for(cell = active_panel; cell; cell = cell->next)
        if(cell->id == cell_id)
            return cell->params.isc;
    return 0;
}

/* -------------------------------------------------------------------------- */
_Q16 model_get_thermal_voltage(unsigned char cell_id)
{
    struct cell_t* cell;
    for(cell = active_panel; cell; cell = cell->next)
        if(cell->id == cell_id)
            return cell->params.vt;
    return 0;
}

/* -------------------------------------------------------------------------- */
_Q16 model_get_relative_solar_irradiation(unsigned char cell_id)
{
    struct cell_t* cell;
    for(cell = active_panel; cell; cell = cell->next)
        if(cell->id == cell_id)
            return cell->params.g;
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
