#include "models/pvarray.h"
#include "models/pvchain.h"
#include "models/pvcell.h"

#include <QString>

// ----------------------------------------------------------------------------
PVArray::PVArray() :
    m_ExposureWeight(1.0)
{
}

// ----------------------------------------------------------------------------
double PVArray::calculateCurrent(double voltage) const
{
    // Current just adds up because the cells are in parallel
    double current = 0.0;
    for(const auto& chain : m_Chains)
        current += chain.calculateCurrent(voltage, m_ExposureWeight);
    return current;
}

// ----------------------------------------------------------------------------
double PVArray::calculateVoltage(double targetCurrent) const
{
    /*
     * Calculating the current of a chain of cells for a given voltage is a
     * non-trivial problem. The easiest, naivest approach to solving this
     * problem is to use an iterative algorithm until the error goes below
     * a certain threshold.
     */
    int maxIterations = 10;

    // if there's only one chain then there's no reason to iterate
    if(m_Chains.size() == 1)
    {
        return m_Chains.begin()->calculateVoltage(targetCurrent, m_ExposureWeight);
    }

    // top range of approximation window
    double top = 0.0;
    for(const auto& chain : m_Chains)
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

// ----------------------------------------------------------------------------
void PVArray::addChain(const QString& chainName, const PVChain& chain)
{
    if(m_Chains.contains(chainName))
        return;
    m_Chains.insert(chainName, chain);
}

// ----------------------------------------------------------------------------
void PVArray::removeChain(const QString& chainName)
{
    m_Chains.remove(chainName);
}

// ----------------------------------------------------------------------------
QMap<QString, PVChain>& PVArray::getChains()
{
    return m_Chains;
}

// ----------------------------------------------------------------------------
double PVArray::calculateAverageParallelVoltage(double totalCurrent) const
{
    double currentPerCell = totalCurrent / m_Chains.size();
    double voltage = 0.0;
    for(const auto& chain : m_Chains)
    {
        voltage += chain.calculateVoltage(currentPerCell);
    }
    return voltage / m_Chains.size();
}
