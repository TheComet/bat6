#ifndef CURVE_2D_BASE_H
#define CURVE_2D_BASE_H

#include "plot/pvmodelfunctionbase.h"
#include <qwt/qwt_series_data.h>

class Curve2DBase :
        public QwtSeriesData<QPointF>,
        public PVModelFunctionBase
{
public:
    Curve2DBase(QSharedPointer<PVArray> pvarray) :
        PVModelFunctionBase(pvarray)
    {}

    virtual ~Curve2DBase() {}
};

#endif // CURVE_2D_BASE_H
