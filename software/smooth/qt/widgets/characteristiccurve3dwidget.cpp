#include "characteristiccurve3dwidget.h"
#include "models/pvarray.h"
#include "models/pvchain.h"
#include "models/pvcell.h"
#include "widgets/cellwidget.h"

#include <QDebug>
#include <qwtplot3d/qwt3d_function.h>
#include <cmath>

using namespace Qwt3D;

class PVModelFunction : public Function
{
public:

    PVModelFunction(SurfacePlot* pw, QSharedPointer<PVArray> pvarray) :
        Function(pw),
        pvarray(pvarray)
    {
    }

    double operator()(double current, double temperature)
    {
        pvarray->setTemperature(temperature);
        return pvarray->calculateVoltage(current);
    }

    QSharedPointer<PVArray> pvarray;
};

// ----------------------------------------------------------------------------
CharacteristicCurve3DWidget::CharacteristicCurve3DWidget(QWidget* parent) :
    SurfacePlot(parent)
{
}

// ----------------------------------------------------------------------------
void CharacteristicCurve3DWidget::addPVArray(const QString& name, QSharedPointer<PVArray> pvarray)
{
    if(pvfunction.contains(name))
        throw std::runtime_error("Failed to add array \"" +
                                 std::string(name.toLocal8Bit().constData()) +
                                 "\": Duplicate name");

    QSharedPointer<PVModelFunction> model(new PVModelFunction(this, pvarray));
    pvfunction.insert(name, model);

    model->setMesh(100,100);
    model->setDomain(0, 3, 273, 500);

    //setRotation(30,0,15);
    setScale(100, 1, 100);
    setShift(0.15,0,0);
    setZoom(0.9);

    for (unsigned i=0; i!=coordinates()->axes.size(); ++i)
    {
        coordinates()->axes[i].setMajors(7);
        coordinates()->axes[i].setMinors(4);
    }

    coordinates()->axes[X1].setLabelString("Temperature (K)");
    coordinates()->axes[Y1].setLabelString("Voltage (V)");
    coordinates()->axes[Z1].setLabelString("Current (A)");

    setCoordinateStyle(BOX);
    //setOrtho(false);
}

// ----------------------------------------------------------------------------
void CharacteristicCurve3DWidget::removePVArray(const QString& name)
{
    pvfunction.remove(name);
}

// ----------------------------------------------------------------------------
void CharacteristicCurve3DWidget::replot()
{
    for(const auto& func : pvfunction)
        func->create();

    updateData();
    updateGL();
}
