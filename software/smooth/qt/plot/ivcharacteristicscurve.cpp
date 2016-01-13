#include "plot/ivcharacteristicscurve.h"
#include "models/pvarray.h"

// ----------------------------------------------------------------------------
IVCharacteristicsCurve::IVCharacteristicsCurve(QSharedPointer<PVArray> pvarray) :
    PVModelFunctionBase(pvarray),
    m_SampleCount(1024)
{
}

// ----------------------------------------------------------------------------
void IVCharacteristicsCurve::setSampleCount(size_t sampleCount)
{
    m_SampleCount = sampleCount;
}

// ----------------------------------------------------------------------------
size_t IVCharacteristicsCurve::size() const
{
    return m_SampleCount;
}

// ----------------------------------------------------------------------------
QPointF IVCharacteristicsCurve::sample(size_t i) const
{
    double maxCurrent = this->boundingRect().y() + this->boundingRect().height();
    double current = maxCurrent * double(i) / double(m_SampleCount - 1);
    double voltage = m_PVArray->calculateVoltage(current);
    return QPointF(voltage, current);
}

// ----------------------------------------------------------------------------
void IVCharacteristicsCurve::updateBoundingBox()
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

    double maxVoltage = tempPVArray.calculateVoltage(0); // 0 amps = open circuit
    double maxCurrent = tempPVArray.calculateCurrent(0); // 0 volts = short circuit

    m_BoundingBox.setRect(0, 0, maxVoltage, maxCurrent);
}
