#include "models/pvchain.h"
#include "models/pvcell.h"

#include <QString>

// ----------------------------------------------------------------------------
PVChain::PVChain() :
    m_ExposureWeight(1.0)
{
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
    int maxIterations = 10;

    // Corner case when voltage is 0. Just return average current.
    if(targetVoltage == 0.0)
        return this->getMaximumSeriesCurrent(targetVoltage, exposure);

    // if there is only one cell then there's no need to iterate
    if(m_CellChain.size() == 1)
    {
        exposure = (exposure < 0.0 ? 0.0 : exposure);
        exposure = (exposure > 1.0 ? 1.0 : exposure);
        double totalExposure = exposure * m_ExposureWeight;
        return m_CellChain.begin()->calculateCurrent(targetVoltage, totalExposure);
    }

    // top range of approximation window
    double top = 0.0;
    for(const auto& cell : m_CellChain)
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
    double totalExposure = exposure * m_ExposureWeight;

    // Voltages just add up because the cells are in series
    double voltage = 0.0;
    for(const auto& cell : m_CellChain)
        voltage += cell.calculateVoltage(current, totalExposure);
    return voltage;
}

// ----------------------------------------------------------------------------
double PVChain::getOpenCircuitVoltage() const
{
    double voltage = 0.0;
    for(const auto& cell : m_CellChain)
        voltage += cell.getOpenCircuitVoltage();
    return voltage;
}

// ----------------------------------------------------------------------------
double PVChain::getMaximumSeriesCurrent(double totalVoltage, double exposure) const
{
    exposure = (exposure < 0.0 ? 0.0 : exposure);
    exposure = (exposure > 1.0 ? 1.0 : exposure);
    double totalExposure = m_ExposureWeight * exposure;

    const double voltagePerCell = totalVoltage / m_CellChain.size();
    double current = 0.0;
    for(const auto& cell : m_CellChain)
    {
        double cellCurrent = cell.calculateCurrent(voltagePerCell, totalExposure);
        current = (cellCurrent > current ? cellCurrent : current);
    }
    return current;
}

// ----------------------------------------------------------------------------
void PVChain::addCell(const QString& cellName, const PVCell& cell)
{
    if(m_CellChain.contains(cellName))
        return;
    m_CellChain.insert(cellName, cell);
}

// ----------------------------------------------------------------------------
void PVChain::removeCell(const QString& cellName)
{
    m_CellChain.remove(cellName);
}

// ----------------------------------------------------------------------------
QMap<QString, PVCell>& PVChain::getCells()
{
    return m_CellChain;
}

