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
    double current = m_MaxCurrent * double(i) / double(m_SampleCount - 1);
    double voltage = m_PVArray->calculateVoltage(current);
    return QPointF(voltage, current);
}

// ----------------------------------------------------------------------------
QRectF IVCharacteristicsCurve::boundingRect() const
{
    return QRectF(m_MinVoltage,
                  m_MinCurrent + this->getCurrentDomain(),
                  this->getVoltageDomain(),
                  this->getCurrentDomain());
}
