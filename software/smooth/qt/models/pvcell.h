#ifndef PVCELL_H
#define PVCELL_H

/*!
 * \brief Represents a single cell in a solar module.
 */
class PVCell
{
public:
    PVCell();
    PVCell(double m_OpenCircuitVoltage,
           double m_ShortCircuitCurrent,
           double m_DarkVoltage,
           double m_Exposure=1.0);

    /*!
     * \brief Calculates the voltage this cell would generate based on the
     * specified current and exposure.
     * \param current The current in amps.
     * \param exposure (Optional) add a weight to the cell's exposure.
     * \return Returns the voltage in volts.
     */
    double calculateVoltage(double current, double exposureWeight=1.0) const;

    /*!
     * \brief Calculates the current this cell would generated based on the
     * specified voltage and exposure.
     * \param voltage The voltage in volts.
     * \param exposure (Optional) add a weight to the cell's exposure.
     * \return Returns the current in amps.
     */
    double calculateCurrent(double voltage, double exposureWeight=1.0) const;

    /*!
     * \brief Sets the light exposure on this cell.
     * \param exposure The exposure where 0.0 is no light and 1.0 is maximum
     * light. The number is clamped clamped.
     */
    void setExposure(double exposure)
    {
        m_Exposure = (exposure >= 1.0 ? 1.0 : exposure);
        m_Exposure = (exposure <= 0.0 ? 0.0 : exposure);
    }

    /*!
     * \brief Gets the cell's light exposure value.
     * \return The exposure, where 0.0 is no light and 1.0 is maximum light.
     */
    double getExposure() const
        { return m_Exposure; }

    void setOpenCircuitVoltage(double openCircuitVoltage)
        { m_OpenCircuitVoltage = openCircuitVoltage; }
    double getOpenCircuitVoltage() const
        { return m_OpenCircuitVoltage; }
    void setShortCircuitCurrent(double shortCircuitCurrent)
        { m_ShortCircuitCurrent = shortCircuitCurrent; }
    double getShortCircuitCurrent() const
        { return m_ShortCircuitCurrent; }
    void setDarkVoltage(double darkVoltage)
        { m_DarkVoltage = darkVoltage; }
    double getDarkVoltage() const
        { return m_DarkVoltage; }

private:
    double m_ShortCircuitCurrent;
    double m_OpenCircuitVoltage;
    double m_DarkVoltage;
    double m_Exposure;
};

#endif // PVCELL_H
