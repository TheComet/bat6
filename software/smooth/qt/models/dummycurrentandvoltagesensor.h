#ifndef DUMMY_CURRENT_AND_VOLTAGE_SENSOR_H
#define DUMMY_CURRENT_AND_VOLTAGE_SENSOR_H

#include <QObject>

class DummyCurrentAndVoltageSensor : public QObject
{
    Q_OBJECT

signals:
    void voltageMeasured(double voltage);
    void currentMeasured(double current);

public:
    DummyCurrentAndVoltageSensor(QObject* parent,
                                 double targetVoltage, double targetCurrent,
                                 double voltageError, double currentError);

private slots:
    void onTick();

private:
    double m_TargetVoltage;
    double m_TargetCurrent;
    double m_VoltageError;
    double m_CurrentError;
};

#endif // DUMMY_CURRENT_AND_VOLTAGE_SENSOR_H
