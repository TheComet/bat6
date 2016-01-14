#ifndef POWER_CURVE_3D_H
#define POWER_CURVE_3D_H

#include "plot/powercurve.h"
#include "plot/curve3dbase.h"

class PowerCurve3D :
        public Curve3DBase
{
public:
    PowerCurve3D(QSharedPointer<PVArray> pvarray, Qwt3D::SurfacePlot* pw);

    double operator()(double current, double exposure);

    virtual void updateBoundingBox() override;

private:
    QScopedPointer<PowerCurve> m_PowerFunction;
};

#endif // POWER_CURVE_3D_H
