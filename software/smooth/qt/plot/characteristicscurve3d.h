#ifndef CHARACTERISTICS_CURVE_3D_H
#define CHARACTERISTICS_CURVE_3D_H

#include <qwtplot3d/qwt3d_function.h>
#include "plot/pvmodelfunctionbase.h"

class CharacteristicsCurve3D :
        public Qwt3D::Function,
        public PVModelFunctionBase
{
public:
    CharacteristicsCurve3D(Qwt3D::SurfacePlot* pw, QSharedPointer<PVArray> pvarray);

    double operator()(double current, double exposure);

    virtual void updateBoundingBox() override;
};

#endif // CHARACTERISTICS_CURVE_3D_H
