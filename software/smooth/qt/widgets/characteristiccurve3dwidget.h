#ifndef CHARACTERISTICCURVE3DWIDGET_H
#define CHARACTERISTICCURVE3DWIDGET_H

#include "widgets/PVArrayPlot.h"

#include <QObject>
#include <qwtplot3d/qwt3d_surfaceplot.h>

class PVArray;
class PVModelFunction;

class CharacteristicCurve3DWidget :
        public Qwt3D::SurfacePlot,
        public PVArrayPlot
{
    Q_OBJECT
public:
    CharacteristicCurve3DWidget(QWidget* parent=nullptr);

    virtual void addPVArray(const QString& name, QSharedPointer<PVArray> pvarray) override;
    virtual void removePVArray(const QString& name) override;
    virtual void replot() override;

private:
    QMap<QString, QSharedPointer<PVModelFunction>> pvfunction;
};

#endif // CHARACTERISTICCURVE3DWIDGET_H
