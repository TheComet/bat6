#include "plot/powercurve.h"
#include "models/pvarray.h"

// ----------------------------------------------------------------------------
PowerCurve::PowerCurve(QSharedPointer<PVArray> pvarray) :
    Curve2DBase(pvarray),
    m_SampleCount(1024),
    m_MaxCurrent(0)
{
}

// ----------------------------------------------------------------------------
void PowerCurve::setSampleCount(size_t sampleCount)
{
    m_SampleCount = sampleCount;
}

// ----------------------------------------------------------------------------
size_t PowerCurve::size() const
{
    return m_SampleCount;
}

// ----------------------------------------------------------------------------
QPointF PowerCurve::sample(size_t i) const
{
    double current = m_MaxCurrent * double(i) / double(m_SampleCount - 1);
    double voltage = m_PVArray->calculateVoltage(current);
    double power = voltage * current;
    return QPointF(voltage, power);
}

// ----------------------------------------------------------------------------
void PowerCurve::updateBoundingBox()
{
    // we want to calculate the bounding box of an array where the exposure is
    // set to its maximum
    PVArray tempPVArray = m_PVArray.operator*();
    tempPVArray.setExposure(1);
    for(QMap<QString, PVChain>::iterator chainIt = tempPVArray.getChains().begin();
        chainIt != tempPVArray.getChains().end(); ++chainIt)
    {
        for(QMap<QString, PVCell>::iterator cellIt = chainIt.value().getCells().begin();
            cellIt != chainIt.value().getCells().end(); ++cellIt)
            cellIt.value().setExposure(1);
        chainIt.value().setExposure(1);
    }

    // required because this->sample() depends on the maximum current
    // being set correctly
    m_MaxCurrent = tempPVArray.calculateCurrent(0);      // 0 volts = short circuit
    double maxVoltage = tempPVArray.calculateVoltage(0); // 0 amps = open circuit

    // now find the maximum power
    double maxPower = 0;
    for(unsigned int i = 0; i != this->size(); ++i)
    {
        double current = m_MaxCurrent * double(i) / double(m_SampleCount - 1);
        double voltage = tempPVArray.calculateVoltage(current);
        double power = voltage * current;
        maxPower = (power > maxPower ? power : maxPower);
    }

    m_BoundingBox.setRect(0, 0, maxVoltage, maxPower);
}
