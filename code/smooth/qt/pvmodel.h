#ifndef PVMODEL_H
#define PVMODEL_H

#include <QList>

class PVCell
{
public:
    PVCell();

    double calculateVoltage(double current) const;
    double calculateCurrent(double voltage) const;

private:
    double shortCircuitCurrent;
    double openCircuitVoltage;
    double darkVoltage;
    double exposure;
};

class PVChain
{
public:
    PVChain();

    double calculateVoltage(double current) const;
    double calculateCurrent(double voltage) const;

private:
    double calculateSeriesCurrent(const QVector<double>& cellVoltages) const;
    double calculateIndividualCellVoltages(double current, QVector<double>& cellVoltages) const;

    QList<PVCell> cellChain;
};

class PVPanel
{
public:
    PVPanel();

    double calculateVoltage(double current) const;
    double calculateCurrent(double voltage) const;

private:
    QList<PVChain> cellArray;
};

#endif // PVMODEL_H
