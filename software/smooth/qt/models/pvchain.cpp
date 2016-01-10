#include "models/pvchain.h"
#include "models/pvcell.h"

#include <QString>

// ----------------------------------------------------------------------------
PVChain::PVChain() :
    exposure(1.0)
{
}

// ----------------------------------------------------------------------------
double PVChain::getMaximumSeriesCurrent(double totalVoltage, double exposure) const
{
    exposure = (exposure < 0.0 ? 0.0 : exposure);
    exposure = (exposure > 1.0 ? 1.0 : exposure);
    double totalExposure = this->exposure * exposure;

    const double voltagePerCell = totalVoltage / cellChain.size();
    double current = 0.0;
    for(const auto& cell : cellChain)
    {
        double cellCurrent = cell.calculateCurrent(voltagePerCell, totalExposure);
        current = (cellCurrent > current ? cellCurrent : current);
    }
    return current;
}

// ----------------------------------------------------------------------------
void PVChain::setExposure(double exposure)
{
    exposure = (exposure <= 0.0 ? 0.0 : exposure);
    this->exposure = (exposure >= 1.0 ? 1.0 : exposure);
}

// ----------------------------------------------------------------------------
double PVChain::calculateCurrent(double targetVoltage, double exposure) const
{
    /*
     * Calculating the current of a chain of cells for a given voltage is a
     * non-trivial problem. The easiest, naivest approach to solving this
     * problem is to use an iterative algorithm until the error goes below
     * a certain threshold.
     */
    int maxIterations = 20;

    // Corner case when voltage is 0. Just return average current.
    if(targetVoltage == 0.0)
        return this->getMaximumSeriesCurrent(targetVoltage, exposure);

    // if there is only one cell then there's no need to iterate
    if(cellChain.size() == 1)
    {
        exposure = (exposure < 0.0 ? 0.0 : exposure);
        exposure = (exposure > 1.0 ? 1.0 : exposure);
        double totalExposure = exposure * this->exposure;
        return cellChain.begin()->calculateCurrent(targetVoltage, totalExposure);
    }

    // top range of approximation window
    double top = 0.0;
    for(const auto& cell : cellChain)
        top = (cell.getShortCircuitCurrent() > top ? cell.getShortCircuitCurrent() : top);

    // bottom range
    double bottom = 0.0;

    // use binary search to approximate current
    double current = (top + bottom) / 2.0;
    while(maxIterations --> 0)
    {
        double voltage = this->calculateVoltage(current, exposure);
        if(voltage < targetVoltage)
            top = current;
        else
            bottom = current;
        current = (top + bottom) / 2.0;
    }

    return current;
}

// ----------------------------------------------------------------------------
double PVChain::calculateVoltage(double current, double exposure) const
{
    exposure = (exposure < 0.0 ? 0.0 : exposure);
    exposure = (exposure > 1.0 ? 1.0 : exposure);
    double totalExposure = exposure * this->exposure;

    // Voltages just add up because the cells are in series
    double voltage = 0.0;
    for(const auto& cell : cellChain)
        voltage += cell.calculateVoltage(current, totalExposure);
    return voltage;
}

// ----------------------------------------------------------------------------
void PVChain::addCell(const QString& cellName, const PVCell& cell)
{
    if(cellChain.contains(cellName))
        return;
    cellChain.insert(cellName, cell);
}

// ----------------------------------------------------------------------------
void PVChain::removeCell(const QString& cellName)
{
    cellChain.remove(cellName);
}

// ----------------------------------------------------------------------------
PVCell* PVChain::getCell(const QString& cellName)
{
    auto cell = cellChain.find(cellName);
    return cell.operator->();
}

// ----------------------------------------------------------------------------
double PVChain::getOpenCircuitVoltage() const
{
    double voltage = 0.0;
    for(const auto& cell : cellChain)
        voltage += cell.getOpenCircuitVoltage();
    return voltage;
}
