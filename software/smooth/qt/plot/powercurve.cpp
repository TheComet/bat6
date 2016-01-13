#include "plot/powercurve.h"
#include "models/pvarray.h"

// ----------------------------------------------------------------------------
PowerCurve::PowerCurve(QSharedPointer<PVArray> pvarray) :
    PVModelFunctionBase(pvarray),
    m_SampleCount(1024)
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
QRectF PowerCurve::boundingRect() const
{
    return QRectF(m_MinVoltage,
                  m_MinCurrent + this->getCurrentDomain() * m_MaxVoltage,
                  this->getVoltageDomain(),
                  this->getCurrentDomain());
}
