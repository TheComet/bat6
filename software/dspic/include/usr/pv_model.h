/*!
 * @file pv_model.h
 * @author Alex Murray
 *
 * Created on 17 November 2015, 20:29
 */

#ifndef PV_MODEL_H
#define	PV_MODEL_H

#include <libq.h>

#ifdef	__cplusplus
extern "C" {
#endif

struct pv_cell_t {
    _Q16 voc; /* open circuit voltage Q5.11*/
    _Q16 isc; /* short circuit current Q3.13*/
    _Q16 vt; /* thermal voltage Q3.13*/
    _Q16 g; /* relative solar iridation Q0.16*/
};

_Q16 calc_voltage(const struct pv_cell_t* cell,
                      const _Q16 voltage_is,
                      const _Q16 current_is);

/*!
 * @brief Creates a new cell and adds it in series with the existing cells.
 * @return Will return a unique cell ID if successful, or 0 if unsuccessful.
 */
unsigned char model_cell_add(void);

/*!
 * @brief Destroys the actively selected cell.
 * @note The active cell is reset to 0.
 * @return Returns non-zero if successful, 0 if otherwise.
 */
unsigned char model_cell_remove(void);

/*!
 * @brief Destroys all cells.
 * @return Returns non-zero if successful, 0 if otherwise.
 */
void model_cell_remove_all(void);

/*!
 * @brief Starts a new iteration and returns the first cell ID in the chain.
 *
 * Internally, the iterator is stored globally so that when get_next() is
 * called, the next cell ID can be retrieved.
 * @return Returns the first cell ID if it exists, or 0 if it does not exist.
 */
short model_cell_begin_iteration(void);

/*!
 * @brief Gets the next cell ID in the chain, relative to the last retrieved
 * cell.
 *
 * Internally, the iterator is incremented to point to the next item.
 * @return Returns the next cell ID in the chain if it exists, or 0 if it does
 * not exist.
 */
short model_cell_get_next(void);

/*!
 * @brief Sets the open circuit voltage of the actively selected cell.
 */
void model_set_open_circuit_voltage(unsigned char cell_id, _Q16 voc);

/*!
 * @brief Sets the short circuit current of the actively selected cell.
 */
void model_set_short_circuit_current(unsigned char cell_id, _Q16 isc);

/*!
 * @brief Sets the thermal voltage of the actively selected cell.
 */
void model_set_thermal_voltage(unsigned char cell_id, _Q16 vt);

/*!
 * @brief Sets the relative solar irradiation of the actively selected cell.
 */
void model_set_relative_solar_irridation(unsigned char cell_id, _Q16 g);

/*!
 * @brief Gets the open circuit voltage of the actively selected cell.
 */
_Q16 model_get_open_circuit_voltage(unsigned char cell_id);

/*!
 * @brief Gets the short circuit current of the actively selected cell.
 */
_Q16 model_get_short_circuit_current(unsigned char cell_id);

/*!
 * @brief Gets the thermal voltage of the actively selected cell.
 */
_Q16 model_get_thermal_voltage(unsigned char cell_id);

/*!
 * @brief Gets the relative solar irradiation of the actively selected cell.
 */
_Q16 model_get_relative_solar_irridation(unsigned char cell_id);

#ifdef	__cplusplus
}
#endif

#endif	/* PV_MODEL_H */
