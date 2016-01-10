#ifndef PVARRAY_H
#define PVARRAY_H

#include <QMap>

class PVChain;

/*!
 * \brief Represents an array of cells.
 *
 * "Cell chains" are cells connected in series. This class will connect
 * multiple cell chains in parallel, thus, creating an array of series
 * and parallel connected cells.
 */
class PVArray
{
public:
    PVArray();

    /*!
     * \brief Calculates the voltage of the entire cell array using the
     * specified current.
     * \param current The current that should be flowing through the array
     * in amps.
     * \return Returns the resulting voltage that is produced by all of the
     * cells in volts.
     */
    double calculateVoltage(double current) const;

    /*!
     * \brief Calculates the current of the entire cell array using the
     * specified voltage.
     * \param voltage The voltage that should exist over the entire array in
     * volts.
     * \return Returns the resulting current in amps.
     */
    double calculateCurrent(double voltage) const;

    /*!
     * \brief Sets the overall exposure of the cell array.
     * \note Note that each cell chain and each individual cell can have their
     * own exposure value. The exposure specified by this function acts as
     * a weight to all of them.
     * \param exposure The exposure to set, where 0.0 is no light and 1.0 is
     * maximum light.
     */
    void setExposure(double exposure);

    /*!
     * \brief Adds a new chain of cells to the cell array.
     * \param chainName A unique name to give the chain.
     * \param chain The chain to add.
     */
    void addChain(const QString& chainName, const PVChain& chain);

    /*!
     * \brief Removes the specified cell chain.
     * \param chainName The name of the chain to remove.
     */
    void removeChain(const QString& chainName);

    /*!
     * \brief Retrieves a cell chain with the specified name.
     * \param chainName The name of the chain to get.
     * \return If the chain does not exist, NULL is returned.
     */
    PVChain* getChain(const QString& chainName);

private:
    double calculateAverageParallelVoltage(double totalCurrent) const;

    QMap<QString, PVChain> cellArray;
    double exposure;
};

#endif // PVARRAY_H
