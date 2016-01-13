#include "plot/ivcharacteristicscurve3d.h"
#include "models/pvarray.h"

// ----------------------------------------------------------------------------
IVCharacteristicsCurve3D::IVCharacteristicsCurve3D(QSharedPointer<PVArray> pvarray,
                                                   Qwt3D::SurfacePlot* pw) :
    Curve3DBase(pvarray, pw),
    m_IVFunction(new IVCharacteristicsCurve(pvarray))
{
}

// ----------------------------------------------------------------------------
double IVCharacteristicsCurve3D::operator()(double voltage, double exposure)
{
    double exposureTemp = m_PVArray->getExposure();
    m_PVArray->setExposure(exposure * 0.01); // convert percent to absolute
    double current = m_PVArray->calculateCurrent(voltage);
    m_PVArray->setExposure(exposureTemp);
    return current;
}

// ----------------------------------------------------------------------------
void IVCharacteristicsCurve3D::updateBoundingBox()
{
    m_IVFunction->updateBoundingBox();
    m_BoundingBox = m_IVFunction->getBoundingRect();
}
