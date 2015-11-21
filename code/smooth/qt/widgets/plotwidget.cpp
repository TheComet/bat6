#include "plotwidget.h"
#include "models/pvmodel.h"
#include "widgets/cellwidget.h"

#include <QDebug>
#include <qwtplot3d/qwt3d_function.h>
#include <cmath>

using namespace Qwt3D;

class PVModule : public Function
{
public:

    PVModule(SurfacePlot* pw, bool voltageDependent) :
        Function(pw),
        voltageDependent(voltageDependent)
    {
        chain.addCell("cell 1", PVCell(6, 3, 1.75));
        chain.addCell("cell 2", PVCell(6, 3, 1.75));
        chain.addCell("cell 3", PVCell(6, 3, 1.75));
        chain.addCell("cell 4", PVCell(6, 3, 1.75));
    }

    PVChain& getChain()
    {
        return chain;
    }

    double operator()(double exposure, double voltage)
    {
        chain.setExposure(0.01 * exposure);
        if(voltageDependent)
            return chain.calculateCurrent(voltage);
        else
            return chain.calculateVoltage(voltage);
    }
private:
    PVChain chain;
    bool voltageDependent;
};

PlotWidget::PlotWidget(bool voltageDependent)
{
    pvmodule = new PVModule(this, voltageDependent);

    pvmodule->setMesh(50,50);
    if(voltageDependent)
        pvmodule->setDomain(0.0, 100.0, 0.0, 24.0);
    else
        pvmodule->setDomain(0.0, 100.0, 0.0, 3.0);

    pvmodule->create();

    setRotation(30,0,15);
    if(voltageDependent)
        setScale(0.4,1,10);
    else
        setScale(0.4,10,1);
    setShift(0.15,0,0);
    setZoom(0.9);

    for (unsigned i=0; i!=coordinates()->axes.size(); ++i)
    {
        coordinates()->axes[i].setMajors(7);
        coordinates()->axes[i].setMinors(4);
    }

    coordinates()->axes[X1].setLabelString("exposure (%)");
    if(voltageDependent) {
        coordinates()->axes[Y1].setLabelString("current (A)");
        coordinates()->axes[Z1].setLabelString("voltage (V)");
    } else {
        coordinates()->axes[Y1].setLabelString("voltage (V)");
        coordinates()->axes[Z1].setLabelString("current (A)");
    }

    setCoordinateStyle(BOX);
    setOrtho(false);

    updateData();
    updateGL();
}

void PlotWidget::onCellExposureChanged(const CellWidget& cellWidget, double exposure)
{
    PVCell* cell = pvmodule->getChain().getCell(cellWidget.getName());
    if(!cell)
        return;
    cell->setExposure(exposure);
    pvmodule->create();
    updateData();
    updateGL();
}
