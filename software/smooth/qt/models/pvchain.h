#ifndef PVCHAIN_H
#define PVCHAIN_H

#include <QMap>
#include <QString>

#include "models/pvcell.h"

/*!
 * \brief Represents a number of cells connected in series.
 */
class PVChain
{
public:
    PVChain();

    /*!
     * \brief Calculates the combined voltage the chain of cells would generate.
     * \param current The current in amps.
     * \param exposure (Optional) add a weight to the chain's exposure.
     * \return Returns the Voltage in volts.
     */
    double calculateVoltage(double current, double m_ExposureWeight=1.0) const;

    /*!
     * \brief Calculates the series current of the chain of cells using the
     * specified total voltage.
     * \param voltage The voltage over all of the cells in volts.
     * \param exposure (Optional) add a weight to the chain's exposure.
     * \return Returns the current in amps.
     */
    double calculateCurrent(double voltage, double m_ExposureWeight=1.0) const;

    /*!
     * \brief Sets the average exposure on all cells.
     * \note Note that each individual cell may have its own exposure value,
     * in which case the exposure set here is weighted with the cell's
     * exposure.
     * \param exposure The exposure, where 0.0 is no light and 1.0 is maximum
     * light. The number is clamped internally.
     */
    void setExposure(double exposureWeight)
    {
        m_ExposureWeight = (exposureWeight <= 0.0 ? 0.0 : exposureWeight);
        m_ExposureWeight = (exposureWeight >= 1.0 ? 1.0 : exposureWeight);
    }

    double getExposure() const
        { return m_ExposureWeight; }

    /*!
     * \brief Returns the largets open circuit voltage found amongst all cells.
     * \return The voltage in volts.
     */
    double getOpenCircuitVoltage() const;

    /*!
     * \brief Calculates the largets series current using the specified total
     * voltage over all cells.
     * \param totalVoltage The voltage over all cells, used to calculate the
     * current of each inidivual cell.
     * \param exposure (Optional) Apply and additional weight to the chain's
     * exposure.
     * \return The largest current found amongst all cells.
     */
    double getMaximumSeriesCurrent(double totalVoltage, double m_ExposureWeight=1.0) const;

    /*!
     * \brief Adds a new cell to the chain of cells.
     * \param cellName A unique name for this cell.
     * \param cell The cell object.
     */
    void addCell(const QString& cellName, const PVCell& cell);

    /*!
     * \brief Removes the specified cell from the chain of cells.
     * \param cellName The name of the cell to remove.
     */
    void removeCell(const QString& cellName);

    /*!
     * \brief Gets the container of PVCell objects.
     */
    QMap<QString, PVCell>& getCells();

private:
    QMap<QString, PVCell> m_CellChain;
    double m_ExposureWeight;
};

#endif // PVCHAIN_H
