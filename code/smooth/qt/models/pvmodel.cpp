#include "pvmodel.h"

#include <cmath>
#include <limits>

PVCell::PVCell() :
    shortCircuitCurrent(3.0),
    openCircuitVoltage(24.0),
    darkVoltage(7.0),
    exposure(1.0)
{
}

PVCell::PVCell(double openCircuitVoltage,
               double shortCircuitCurrent,
               double darkVoltage,
               double exposure) :
    shortCircuitCurrent(shortCircuitCurrent),
    openCircuitVoltage(openCircuitVoltage),
    darkVoltage(darkVoltage),
    exposure(1.0)
{
    this->setExposure(exposure);
}

double PVCell::calculateCurrent(double voltage, double exposure) const
{
    // voltage can't be higher than open circuit voltage
    voltage = (voltage >= openCircuitVoltage ? openCircuitVoltage : voltage);

    // calculate total exposure
    exposure = (exposure <= 0.0 ? 0.0 : exposure);
    exposure = (exposure >= 1.0 ? 1.0 : exposure);
    double totalExposure = this->exposure * exposure;

    // calculate current. Current can't be lower than 0A.
    double current = shortCircuitCurrent * \
            (totalExposure - exp((voltage - openCircuitVoltage) / darkVoltage));
    current = (current <= 0 ? 0 : current);
    return (current >= shortCircuitCurrent ? shortCircuitCurrent : current);
}

double PVCell::calculateVoltage(double current, double exposure) const
{
    // current can't be higher than short circuit current
    current = (current >= shortCircuitCurrent ? shortCircuitCurrent : current);

    // calculate total exposure
    exposure = (exposure <= 0.0 ? 0.0 : exposure);
    exposure = (exposure >= 1.0 ? 1.0 : exposure);
    double totalExposure = this->exposure * exposure;

    // calculate exponent and clamp, so log() doesn't fail miserably
    double exponent = (totalExposure - (current / shortCircuitCurrent));
    exponent = (exponent <= 0.0 ? std::numeric_limits<double>::epsilon() : exponent);

    // calculate voltage. Voltage can't be lower than 0V (because out diode)
    double voltage = log(exponent) * darkVoltage + openCircuitVoltage;
    return (voltage <= 0.0 ? 0.0 : voltage);
}

void PVCell::setExposure(double exposure)
{
    exposure = (exposure >= 1.0 ? 1.0 : exposure);
    this->exposure = (exposure <= 0.0 ? 0.0 : exposure);
}

PVChain::PVChain() :
    exposure(1.0)
{
}

static void normaliseVoltages(QVector< QPair<const PVCell*, double> >& cellVoltages, double maxVoltage)
{
    double ratio = 0.0;
    for(const auto& cellVoltage : cellVoltages)
        ratio += cellVoltage.second;
    if(ratio == 0.0)
        return;
    ratio = maxVoltage / ratio;
    for(auto& cellVoltage : cellVoltages)
        cellVoltage.second *= ratio;
}

double PVChain::calculateSeriesCurrent(const QVector< QPair<const PVCell*, double> >& cellVoltages) const
{
    double minCurrent = std::numeric_limits<double>::max();
    for(const auto& cellVoltage : cellVoltages)
    {
        double current = cellVoltage.first->calculateCurrent(cellVoltage.second, this->exposure);
        minCurrent = (current < minCurrent ? current : minCurrent);
    }
    return minCurrent;
}

double PVChain::calculateIndividualCellVoltages(double current, QVector< QPair<const PVCell*, double> >& cellVoltages) const
{
    double totalVoltage = 0.0;
    for(auto& cellVoltage : cellVoltages)
    {
        cellVoltage.second = cellVoltage.first->calculateVoltage(current, this->exposure);
        totalVoltage += cellVoltage.second;
    }
    return totalVoltage;
}

void PVChain::setExposure(double exposure)
{
    exposure = (exposure <= 0.0 ? 0.0 : exposure);
    this->exposure = (exposure >= 1.0 ? 1.0 : exposure);
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
    QVector< QPair<const PVCell*, double> > cellVoltages;
    for(auto& cell : cellChain)
        cellVoltages.push_back(
                    QPair<const PVCell*, double>(&cell, targetVoltage / cellChain.size()));

    double seriesCurrent;
    while(maxIterations --> 0)
    {
        seriesCurrent = this->calculateSeriesCurrent(cellVoltages);
        double totalVoltage = this->calculateIndividualCellVoltages(seriesCurrent, cellVoltages);
        if(seriesCurrent == 0.0 && totalVoltage == 0.0)
            return 0.0;

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
        voltage += cell.calculateVoltage(current, this->exposure);
    return voltage;
}

void PVChain::addCell(const QString &cellName, const PVCell &cell)
{
    if(cellChain.contains(cellName))
        return;
    cellChain.insert(cellName, cell);
}

void PVChain::removeCell(const QString &cellName)
{
    cellChain.remove(cellName);
}

const PVCell* PVChain::getCell(const QString &cellName) const
{
    cellChain.find(cellName);
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
    return current;
}
