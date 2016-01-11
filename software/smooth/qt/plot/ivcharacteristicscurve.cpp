#include "plot/ivcharacteristicscurve.h"

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
}

// ----------------------------------------------------------------------------
QRectF IVCharacteristicsCurve::boundingRect() const
{
}
