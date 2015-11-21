#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QObject>
#include <qwtplot3d/qwt3d_surfaceplot.h>

class CellWidget;
class PVModule;

class PlotWidget : public Qwt3D::SurfacePlot
{
    Q_OBJECT
public:
    PlotWidget(bool voltageDependent);

private slots:
    void onCellExposureChanged(const CellWidget& cellWidget, double intensity);

private:
    PVModule* pvmodule;
};

#endif // PLOTWIDGET_H
