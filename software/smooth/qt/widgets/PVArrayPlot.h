#ifndef PV_ARRAY_PLOT_H
#define PV_ARRAY_PLOT_H

#include <QString>
#include <QSharedPointer>

class PVArray;
class PVModelFunction;

class PVArrayPlot
{
public:
    void addPVArray(const QString& name, QSharedPointer<PVArray> pvarray);
    void removePVArray(const QString& name);
    QSharedPointer<PVArray> getPVArray(const QString& name);

    void replot();

private:
    QMap<QString, QSharedPointer<PVModelFunction>> pvfunction;
};

#endif // PV_ARRAY_PLOT_H
