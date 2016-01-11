#include "models/pvcell.h"

#include <cmath>
#include <limits>

// ----------------------------------------------------------------------------
PVCell::PVCell() :
    m_ShortCircuitCurrent(3.0),
    m_OpenCircuitVoltage(24.0),
    m_DarkVoltage(7.0),
    m_Exposure(1.0)
{
}

// ----------------------------------------------------------------------------
PVCell::PVCell(double openCircuitVoltage,
               double shortCircuitCurrent,
               double darkVoltage,
               double exposure) :
    m_ShortCircuitCurrent(shortCircuitCurrent),
    m_OpenCircuitVoltage(openCircuitVoltage),
    m_DarkVoltage(darkVoltage),
    m_Exposure(1.0)
{
    this->setExposure(exposure);
}

// ----------------------------------------------------------------------------
double PVCell::calculateCurrent(double voltage, double exposureWeight) const
{
    // voltage can't be higher than open circuit voltage
    voltage = (voltage >= m_OpenCircuitVoltage ? m_OpenCircuitVoltage : voltage);

    // calculate total exposure
    exposureWeight = (exposureWeight <= 0.0 ? 0.0 : exposureWeight);
    exposureWeight = (exposureWeight >= 1.0 ? 1.0 : exposureWeight);
    double totalExposure = m_Exposure * exposureWeight;

    // calculate current. Current can't be lower than 0A.
    double current = m_ShortCircuitCurrent * \
            (totalExposure - exp((voltage - m_OpenCircuitVoltage) / m_DarkVoltage));
    current = (current <= 0 ? 0 : current);
    return (current >= m_ShortCircuitCurrent ? m_ShortCircuitCurrent : current);
}

// ----------------------------------------------------------------------------
double PVCell::calculateVoltage(double current, double exposureWeight) const
{
    // current can't be higher than short circuit current
    current = (current >= m_ShortCircuitCurrent ? m_ShortCircuitCurrent : current);

    // calculate total exposure
    exposureWeight = (exposureWeight <= 0.0 ? 0.0 : exposureWeight);
    exposureWeight = (exposureWeight >= 1.0 ? 1.0 : exposureWeight);
    double totalExposure = m_Exposure * exposureWeight;

    // calculate exponent and clamp, so log() doesn't fail miserably
    double exponent = (totalExposure - (current / m_ShortCircuitCurrent));
    exponent = (exponent <= 0.0 ? std::numeric_limits<double>::epsilon() : exponent);

    // calculate voltage. Voltage can't be lower than 0V (because of diode)
    double voltage = log(exponent) * m_DarkVoltage + m_OpenCircuitVoltage;
    return (voltage <= 0.0 ? 0.0 : voltage);
}

