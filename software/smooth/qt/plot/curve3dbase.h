#ifndef CURVE_3D_BASE_H
#define CURVE_3D_BASE_H

#include "plot/pvmodelfunctionbase.h"
#include <qwtplot3d/qwt3d_function.h>

class Curve3DBase :
        public Qwt3D::Function,
        public PVModelFunctionBase
{
public:
    Curve3DBase(QSharedPointer<PVArray> pvarray, Qwt3D::SurfacePlot* pw) :
        Function(pw),
        PVModelFunctionBase(pvarray)
    {}
    virtual ~Curve3DBase() {}
};

#endif
