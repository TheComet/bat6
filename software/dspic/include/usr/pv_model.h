/*!
 * @file pv_model.h
 * @author Alex Murray
 *
 * Created on 17 November 2015, 20:29
 */

#ifndef PV_MODEL_H
#define	PV_MODEL_H

#ifdef	__cplusplus
extern "C" {
#endif

struct pv_cell_t
{
    uint16_t voc;  /* open circuit voltage Q5.11*/
    uint16_t isc;  /* short circuit current Q3.13*/
    uint16_t vt;   /* thermal voltage Q3.13*/
    uint16_t g;    /* relative solar irradiation Q0.16*/
};

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
void model_set_open_circuit_voltage(uint16_t voc);

/*!
 * @brief Sets the short circuit current of the actively selected cell.
 */
void model_set_short_circuit_current(uint16_t isc);

/*!
 * @brief Sets the thermal voltage of the actively selected cell.
 */
void model_set_thermal_voltage(uint16_t vt);

/*!
 * @brief Sets the relative solar irradiation of the actively selected cell.
 */
void model_set_relative_solar_irridation(uint16_t g);

/*!
 * @brief Gets the open circuit voltage of the actively selected cell.
 */
uint16_t model_get_open_circuit_voltage(void);

/*!
 * @brief Gets the short circuit current of the actively selected cell.
 */
uint16_t model_get_short_circuit_current(void);

/*!
 * @brief Gets the thermal voltage of the actively selected cell.
 */
uint16_t model_get_thermal_voltage(void);

/*!
 * @brief Gets the relative solar irradiation of the actively selected cell.
 */
uint16_t model_get_relative_solar_irridation(void);

#ifdef	__cplusplus
}
#endif

#endif	/* PV_MODEL_H */
