#ifndef CHARACTERISTICS_CURVE_3D_H
#define CHARACTERISTICS_CURVE_3D_H

#include "plot/ivcharacteristicscurve.h"
#include "plot/curve3dbase.h"

class IVCharacteristicsCurve3D :
    public Curve3DBase
{
public:
    IVCharacteristicsCurve3D(QSharedPointer<PVArray> pvarray, Qwt3D::SurfacePlot* pw);

    double operator()(double current, double exposure);

    virtual void updateBoundingBox() override;

private:
    QScopedPointer<IVCharacteristicsCurve> m_IVFunction;
};

#endif // CHARACTERISTICS_CURVE_3D_H
