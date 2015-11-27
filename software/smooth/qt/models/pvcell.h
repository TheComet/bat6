#ifndef PVCELL_H
#define PVCELL_H

/*!
 * \brief Represents a single cell in a solar module.
 */
class PVCell
{
public:
    PVCell();
    PVCell(double openCircuitVoltage,
           double shortCircuitCurrent,
           double temperature=273.13+25.0,
           double exposure=1.0);

    /*!
     * \brief Calculates the voltage this cell would generate based on the
     * specified current and exposure.
     * \param current The current in amps.
     * \param exposure (Optional) add a weight to the cell's exposure.
     * \return Returns the voltage in volts.
     */
    double calculateVoltage(double current, double exposure=1.0) const;

    /*!
     * \brief Calculates the current this cell would generated based on the
     * specified voltage and exposure.
     * \param voltage The voltage in volts.
     * \param exposure (Optional) add a weight to the cell's exposure.
     * \return Returns the current in amps.
     */
    double calculateCurrent(double voltage, double exposure=1.0) const;

    /*!
     * \brief Sets the light exposure on this cell.
     * \param exposure The exposure where 0.0 is no light and 1.0 is maximum
     * light. The number is clamped clamped.
     */
    void setExposure(double exposure);

    /*!
     * \brief Gets the cell's light exposure value.
     * \return The exposure, where 0.0 is no light and 1.0 is maximum light.
     */
    double getExposure() const;

    void setTemperature(double temperature);

    double getTemperature() const;

    /*!
     * \brief Returns the short circuit current of this cell.
     * \return The short circuit current in amps.
     */
    double getShortCircuitCurrent() const;

    /*!
     * \brief Returns the open circuit voltage of this cell.
     * \return The open circuit voltage in volts.
     */
    double getOpenCircuitVoltage() const;

private:
    double shortCircuitCurrent;
    double openCircuitVoltage;
    double temperature;
    double exposure;
};

#endif // PVCELL_H
