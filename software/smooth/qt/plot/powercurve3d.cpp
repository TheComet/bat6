#include "plot/powercurve3d.h"
#include "plot/powercurve.h"
#include "models/pvarray.h"

// ----------------------------------------------------------------------------
PowerCurve3D::PowerCurve3D(QSharedPointer<PVArray> pvarray,
                           Qwt3D::SurfacePlot* pw) :
    Curve3DBase(pvarray, pw),
    m_PowerFunction(new PowerCurve(pvarray))
{
}

// ----------------------------------------------------------------------------
double PowerCurve3D::operator()(double voltage, double exposure)
{
    double exposureTemp = m_PVArray->getExposure();
    m_PVArray->setExposure(exposure * 0.01); // convert percent to absolute
    double current = m_PVArray->calculateCurrent(voltage);
    m_PVArray->setExposure(exposureTemp);
    return voltage * current;
}

// ----------------------------------------------------------------------------
void PowerCurve3D::updateBoundingBox()
{
    m_PowerFunction->updateBoundingBox();
    /*QRectF b = m_PowerFunction->getBoundingRect();
    m_BoundingBox.setRect(b.y(), b.x(), b.height(), b.width());*/
    m_BoundingBox = m_PowerFunction->getBoundingRect();
}
