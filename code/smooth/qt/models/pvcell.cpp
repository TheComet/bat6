#include "models/pvcell.h"

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

double PVCell::getOpenCircuitVoltage() const
{
    return openCircuitVoltage;
}
