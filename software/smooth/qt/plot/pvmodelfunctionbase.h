#ifndef PV_MODEL_FUNCTION_BASE_H
#define PV_MODEL_FUNCTION_BASE_H

#include <QSharedPointer>

class PVArray;

class PVModelFunctionBase
{
public:
    PVModelFunctionBase(QSharedPointer<PVArray> pvarray);
    virtual ~PVModelFunctionBase();

    void updateBoundingBox();

    double getVoltageDomain() const
        { return m_MaxVoltage - m_MinVoltage; }
    double getCurrentDomain() const
        { return m_MaxCurrent - m_MinCurrent; }
    double getExposureDomain() const
        { return m_MaxExposure - m_MinExposure; }

protected:
    QSharedPointer<PVArray> m_PVArray;

    double m_MinVoltage;
    double m_MaxVoltage;
    double m_MinCurrent;
    double m_MaxCurrent;
    double m_MinExposure;
    double m_MaxExposure;
};

#endif // PV_MODEL_FUNCTION_BASE_H
