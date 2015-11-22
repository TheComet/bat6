#ifndef PVCHAIN_H
#define PVCHAIN_H

#include <QMap>

class PVCell;
class QString;

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
    double calculateVoltage(double current, double exposure=1.0) const;

    /*!
     * \brief Calculates the series current of the chain of cells using the
     * specified total voltage.
     * \param voltage The voltage over all of the cells in volts.
     * \param exposure (Optional) add a weight to the chain's exposure.
     * \return Returns the current in amps.
     */
    double calculateCurrent(double voltage, double exposure=1.0) const;

    /*!
     * \brief Sets the average exposure on all cells.
     * \note Note that each individual cell may have its own exposure value,
     * in which case the exposure set here is weighted with the cell's
     * exposure.
     * \param exposure The exposure, where 0.0 is no light and 1.0 is maximum
     * light. The number is clamped internally.
     */
    void setExposure(double exposure);

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
     * \brief Gets the cell object that was previously added with the specified
     * name.
     * \param The name of the cell object to get.
     * \return Returns NULL if the cell object wasn't found.
     */
    PVCell* getCell(const QString& cellName);

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
    double getMaximumSeriesCurrent(double totalVoltage, double exposure=1.0) const;

private:
    QMap<QString, PVCell> cellChain;
    double exposure;
};

#endif // PVCHAIN_H
