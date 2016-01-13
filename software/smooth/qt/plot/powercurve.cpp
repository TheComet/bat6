#include "plot/powercurve.h"
#include "models/pvarray.h"

// ----------------------------------------------------------------------------
PowerCurve::PowerCurve(QSharedPointer<PVArray> pvarray) :
    PVModelFunctionBase(pvarray),
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
    for(PVChain& chain : tempPVArray.getChains())
    {
        for(PVCell& cell : chain.getCells())
            cell.setExposure(1);
        chain.setExposure(1);
    }

    // required because this->sample() depends on the maximum current
    // being set correctly
    m_MaxCurrent = tempPVArray.calculateCurrent(0);      // 0 volts = short circuit
    double maxVoltage = tempPVArray.calculateVoltage(0); // 0 amps = open circuit

    // now find the maximum power
    double maxPower = 0;
    for(unsigned int i = 0; i != this->size(); ++i)
    {
        QPointF pv = this->sample(i);
        maxPower = (pv.y() > maxPower ? pv.y() : maxPower);
    }

    m_BoundingBox.setRect(0, 0, maxVoltage, maxPower);
}
