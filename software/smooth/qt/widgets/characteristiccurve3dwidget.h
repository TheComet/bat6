#ifndef CHARACTERISTICCURVE3DWIDGET_H
#define CHARACTERISTICCURVE3DWIDGET_H

#include <QObject>
#include <qwtplot3d/qwt3d_surfaceplot.h>

class PVArray;
class PVModelFunction;

class CharacteristicCurve3DWidget : public Qwt3D::SurfacePlot
{
    Q_OBJECT
public:
    CharacteristicCurve3DWidget(QWidget* parent=nullptr);

    void addPVArray(const QString& name, QSharedPointer<PVArray> pvarray);
    void removePVArray(const QString& name);
    QSharedPointer<PVArray> getPVArray(const QString& name);

    void replot();

private:
    QMap<QString, QSharedPointer<PVModelFunction>> pvfunction;
};

#endif // CHARACTERISTICCURVE3DWIDGET_H
