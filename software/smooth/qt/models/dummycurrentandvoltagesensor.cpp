#include "models/dummycurrentandvoltagesensor.h"

#include <QTimer>
#include <QtGlobal>

// ----------------------------------------------------------------------------
DummyCurrentAndVoltageSensor::DummyCurrentAndVoltageSensor(QObject* parent,
                                                           double targetVoltage,
                                                           double targetCurrent,
                                                           double voltageError,
                                                           double currentError) :
    QObject(parent),
    m_TargetVoltage(targetVoltage),
    m_TargetCurrent(targetCurrent),
    m_VoltageError(voltageError),
    m_CurrentError(currentError)
{
    QTimer* timer = new QTimer();
    this->connect(timer, SIGNAL(timeout()), this, SLOT(onTick()));
    timer->start(200);
}

// ----------------------------------------------------------------------------
void DummyCurrentAndVoltageSensor::onTick()
{
    double voltage = m_TargetVoltage + (qrand() % 1000 - 500) * 0.001 * m_VoltageError;
    double current = m_TargetCurrent + (qrand() % 1000 - 500) * 0.001 * m_CurrentError;
    emit voltageMeasured(voltage);
    emit currentMeasured(current);
}
