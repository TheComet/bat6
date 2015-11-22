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

    // calculate voltage. Voltage can't be lower than 0V (because of diode)
    double voltage = log(exponent) * darkVoltage + openCircuitVoltage;
    return (voltage <= 0.0 ? 0.0 : voltage);
}

void PVCell::setExposure(double exposure)
{
    exposure = (exposure >= 1.0 ? 1.0 : exposure);
    this->exposure = (exposure <= 0.0 ? 0.0 : exposure);
}

double PVCell::getShortCircuitCurrent() const
{
    return shortCircuitCurrent;
}

PVChain::PVChain() :
    exposure(1.0)
{
}

double PVChain::calculateAverageSeriesCurrent(double totalVoltage) const
{
    const double voltagePerCell = totalVoltage / cellChain.size();
    double averageCurrent = 0.0;
    for(const auto& cell : cellChain)
    {
        averageCurrent += cell.calculateCurrent(voltagePerCell, this->exposure);
    }
    return averageCurrent / cellChain.size();
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
    int maxIterations = 20;

    if(targetVoltage == 0.0)
        return this->calculateAverageSeriesCurrent(targetVoltage);

    double top = 0.0;
    for(const auto& cell : cellChain)
        top = (cell.getShortCircuitCurrent() > top ? cell.getShortCircuitCurrent() : top);

    double bottom = 0.0;
    double current = (top + bottom) / 2.0;
    while(maxIterations --> 0)
    {
        double voltage = this->calculateVoltage(current);
        if(voltage < targetVoltage)
            top = current;
        else
            bottom = current;
        current = (top + bottom) / 2.0;
    }

    return current;
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

PVCell* PVChain::getCell(const QString &cellName)
{
    auto cell = cellChain.find(cellName);
    return cell.operator->();
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
