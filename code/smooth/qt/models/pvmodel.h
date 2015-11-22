#ifndef PVMODEL_H
#define PVMODEL_H

#include <QMap>
#include <QVector>
#include <QPair>

/*!
 * \brief Represents a single cell in a solar module.
 */
class PVCell
{
public:
    PVCell();
    PVCell(double openCircuitVoltage,
           double shortCircuitCurrent,
           double darkVoltage,
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

    /*!
     * \brief Returns the short circuit current of this cell.
     * \return The short circuit current in amps.
     */
    double getShortCircuitCurrent() const;

private:
    double shortCircuitCurrent;
    double openCircuitVoltage;
    double darkVoltage;
    double exposure;
};

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
     * \return Returns the Voltage in volts.
     */
    double calculateVoltage(double current) const;

    /*!
     * \brief Calculates the series current of the chain of cells using the
     * specified total voltage.
     * \param voltage The voltage over all of the cells in volts.
     * \return Returns the current in amps.
     */
    double calculateCurrent(double voltage) const;

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

private:
    double calculateAverageSeriesCurrent(double totalVoltage) const;

    QMap<QString, PVCell> cellChain;
    double exposure;
};

class PVPanel
{
public:
    PVPanel();

    double calculateVoltage(double current) const;
    double calculateCurrent(double voltage) const;

private:
    QList<PVChain> cellArray;
};

#endif // PVMODEL_H
