#ifndef IV_CHARACTERISTICS_CURVE_H
#define IV_CHARACTERISTICS_CURVE_H

#include "plot/curve2dbase.h"

class IVCharacteristicsCurve :
        public Curve2DBase
{
public:
    IVCharacteristicsCurve(QSharedPointer<PVArray> pvarray);

    void setSampleCount(size_t sampleCount);

    virtual size_t size() const override;
    virtual QPointF sample(size_t i) const override;
    virtual void updateBoundingBox() override;
    virtual QRectF boundingRect() const override
        { return this->getBoundingRect(); }

private:
    size_t m_SampleCount;
};

#endif // IV_CHARACTERISTICS_CURVE_H
