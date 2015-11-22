#include "models/pvarray.h"
#include "models/pvchain.h"
#include "models/pvcell.h"

#include <QString>

PVArray::PVArray() :
    exposure(1.0)
{
}

double PVArray::calculateCurrent(double voltage) const
{
    // Current just adds up because the cells are in parallel
    double current = 0.0;
    for(const auto& chain : cellArray)
        current += chain.calculateCurrent(voltage, this->exposure);
    return current;
}

double PVArray::calculateVoltage(double targetCurrent) const
{
    /*
     * Calculating the current of a chain of cells for a given voltage is a
     * non-trivial problem. The easiest, naivest approach to solving this
     * problem is to use an iterative algorithm until the error goes below
     * a certain threshold.
     */
    int maxIterations = 20;

    // if there's only one chain then there's no reason to iterate
    if(cellArray.size() == 1)
    {
        return cellArray.begin()->calculateVoltage(targetCurrent, this->exposure);
    }

    // top range of approximation window
    double top = 0.0;
    for(const auto& chain : cellArray)
        top = (chain.getOpenCircuitVoltage() > top ? chain.getOpenCircuitVoltage() : top);

    // bottom range
    double bottom = 0.0;

    // use binary search to approximate current
    double voltage = (top + bottom) / 2.0;
    while(maxIterations --> 0)
    {
        double current = this->calculateCurrent(voltage);
        if(current < targetCurrent)
            top = voltage;
        else
            bottom = voltage;
        voltage = (top + bottom) / 2.0;
    }

    return voltage;
}

void PVArray::setExposure(double exposure)
{
    exposure = (exposure < 0.0 ? 0.0 : exposure);
    this->exposure = (exposure > 1.0 ? 1.0 : exposure);
}

void PVArray::addChain(const QString& chainName, const PVChain& chain)
{
    if(cellArray.contains(chainName))
        return;
    cellArray.insert(chainName, chain);
}

void PVArray::removeChain(const QString& chainName)
{
    cellArray.remove(chainName);
}

PVChain* PVArray::getChain(const QString& chainName)
{
    auto chain = cellArray.find(chainName);
    return chain.operator->();
}

double PVArray::calculateAverageParallelVoltage(double totalCurrent) const
{
    double currentPerCell = totalCurrent / cellArray.size();
    double voltage = 0.0;
    for(const auto& chain : cellArray)
    {
        voltage += chain.calculateVoltage(currentPerCell);
    }
    return voltage / cellArray.size();
}
