#ifndef IV_POWER_CURVE_H
#define IV_POWER_CURVE_H

#include <qwt/qwt_series_data.h>
#include "plot/pvmodelfunctionbase.h"

class PowerCurve :
        public QwtSeriesData<QPointF>,
        public PVModelFunctionBase
{
public:
    PowerCurve(QSharedPointer<PVArray> pvarray);

    void setSampleCount(size_t sampleCount);

    virtual size_t size() const override;
    virtual QPointF sample(size_t i) const override;
    virtual QRectF boundingRect() const override
        { return this->getBoundingRect(); }
    virtual void updateBoundingBox() override;

private:
    size_t m_SampleCount;
    double m_MaxCurrent;
};

#endif // IV_POWER_CURVE_H
