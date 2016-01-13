#ifndef PV_MODEL_FUNCTION_BASE_H
#define PV_MODEL_FUNCTION_BASE_H

#include <QSharedPointer>
#include <QRectF>

class PVArray;

class PVModelFunctionBase
{
public:
    PVModelFunctionBase(QSharedPointer<PVArray> pvarray);
    virtual ~PVModelFunctionBase();

    virtual void updateBoundingBox() = 0;

    const QRectF& getBoundingRect() const
        { return m_BoundingBox; }

protected:
    QSharedPointer<PVArray> m_PVArray;
    QRectF m_BoundingBox;
};

#endif // PV_MODEL_FUNCTION_BASE_H
