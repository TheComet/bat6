#include "pvmodel.h"
#include <QVector>
#include <cmath>
#include <limits>

PVCell::PVCell() :
    shortCircuitCurrent(3.0),
    openCircuitVoltage(24.0),
    darkVoltage(7.0),
    exposure(1.0)
{
}

double PVCell::calculateCurrent(double voltage) const
{
    return shortCircuitCurrent * \
            (exposure - exp((voltage - openCircuitVoltage) / darkVoltage));
}

double PVCell::calculateVoltage(double current) const
{
    return log(exposure - (current / shortCircuitCurrent)) * \
            darkVoltage + openCircuitVoltage;
}

PVChain::PVChain()
{
}

static void normaliseVoltages(QVector<double>& voltages, double maxVoltage)
{
    double ratio = 0.0;
    for(const auto& v : voltages)
        ratio += v;
    ratio = maxVoltage / ratio;
    for(auto& v : voltages)
        v *= ratio;
}

double PVChain::calculateSeriesCurrent(const QVector<double>& cellVoltages) const
{
    double minCurrent = std::numeric_limits<double>::max();
    for(int i = 0; i != cellChain.size(); ++i)
    {
        double current = cellChain[i].calculateCurrent(cellVoltages[i]);
        minCurrent = (current < minCurrent ? current : minCurrent);
    }
    return minCurrent;
}

double PVChain::calculateIndividualCellVoltages(double current, QVector<double>& cellVoltages) const
{
    double totalVoltage = 0.0;
    for(int i = 0; i != cellChain.size(); ++i)
    {
        cellVoltages[i] = cellChain[i].calculateVoltage(current);
        totalVoltage += cellVoltages[i];
    }
    return totalVoltage;
}

double PVChain::calculateCurrent(double targetVoltage) const
{
    /*
     * Calculating the current of a chain of cells for a given voltage is a
     * non-trivial problem. The easiest, naivest approach to solving this
     * problem is to use an iterative algorithm until the error goes below
     * a certain threshold.
     */
    const double acceptableErrorInPercent = 0.1;
    int maxIterations = 1000;

    /*
     * We start with the assumption that each cell is producing an equal
     * amount of voltage.
     */
    QVector<double> cellVoltages;
    for(int i = 0; i != cellChain.size(); ++i)
        cellVoltages.push_back(targetVoltage / cellChain.size());

    double seriesCurrent;
    while(maxIterations --> 0)
    {
        seriesCurrent = this->calculateSeriesCurrent(cellVoltages);
        double totalVoltage = this->calculateIndividualCellVoltages(seriesCurrent, cellVoltages);
        normaliseVoltages(cellVoltages, targetVoltage);

        double error = (totalVoltage / targetVoltage) - 1.0;
        error = (error > 0 ? error : -error);
        if(error * 100.0 < acceptableErrorInPercent)
            break;
    }

    return seriesCurrent;
}

double PVChain::calculateVoltage(double current) const
{
    // Voltages just add up because the cells are in series
    double voltage = 0.0;
    for(const auto& cell : cellChain)
        voltage += cell.calculateVoltage(current);
    return voltage;
}

PVPanel::PVPanel()
{
}

double PVPanel::calculateCurrent(double voltage) const
{
    // Current just adds up because the cells are in parallel
    double current = 0.0;
    for(const auto& chain : cellArray)
        current += chain.calculateCurrent(voltage);
    return current;
}

double PVPanel::calculateVoltage(double current) const
{

}
