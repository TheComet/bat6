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
 * @note This will automatically select the new cell as the active cell if
 * successful.
 * @return Will return a unique cell ID if successful, or 0 if unsuccessful.
 */
unsigned char model_cell_create(void);

/*!
 * @brief Destroys the actively selected cell.
 * @note The active cell is reset to 0.
 * @return Returns non-zero if successful, 0 if otherwise.
 */
unsigned char model_cell_destroy(void);

/*!
 * @brief Selects the cell corresponding to the specified cell ID as the active
 * cell.
 * @return Returns non-zero if the cell ID exists, 0 if otherwise.
 */
unsigned char model_select_cell(unsigned char cell_id);

/*!
 * @brief Sets the open circuit voltage of the actively selected cell.
 */
void model_set_open_circuit_voltage(_Q16 voc);

/*!
 * @brief Sets the short circuit current of the actively selected cell.
 */
void model_set_short_circuit_current(_Q16 isc);

/*!
 * @brief Sets the thermal voltage of the actively selected cell.
 */
void model_set_thermal_voltage(_Q16 vt);

/*!
 * @brief Sets the relative solar irradiation of the actively selected cell.
 */
void model_set_relative_solar_irridation(_Q16 g);

/*!
 * @brief Gets the open circuit voltage of the actively selected cell.
 */
_Q16 model_get_open_circuit_voltage(void);

/*!
 * @brief Gets the short circuit current of the actively selected cell.
 */
_Q16 model_get_short_circuit_current(void);

/*!
 * @brief Gets the thermal voltage of the actively selected cell.
 */
_Q16 model_get_thermal_voltage(void);

/*!
 * @brief Gets the relative solar irradiation of the actively selected cell.
 */
_Q16 model_get_relative_solar_irridation(void);

#ifdef	__cplusplus
}
#endif

#endif	/* PV_MODEL_H */
