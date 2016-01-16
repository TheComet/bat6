#ifndef CHARACTERISTICCURVE3DWIDGET_H
#define CHARACTERISTICCURVE3DWIDGET_H

#include "widgets/pvarrayplotwidgetbase.h"

#include <QObject>
#include <qwt3d_surfaceplot.h>

class PVArray;
class Curve3DBase;

class CharacteristicsCurve3DWidget :
        public Qwt3D::SurfacePlot,
        public PVArrayPlotWidgetBase
{
public:
    CharacteristicsCurve3DWidget(QWidget* parent=nullptr);

    void normaliseScale();

    virtual void addPVArray(const QString& name, QSharedPointer<PVArray> pvarray) override;
    virtual void removePVArray(const QString& name) override;
    virtual void replot() override;

private:
    QMap<QString, QSharedPointer<Curve3DBase>> m_Function;
};

#endif // CHARACTERISTICCURVE3DWIDGET_H
