#include "widgets/characteristicscurve3dwidget.h"
#include "widgets/cellwidget.h"
#include "models/pvarray.h"
#include "models/pvchain.h"
#include "models/pvcell.h"
#include "plot/characteristicscurve3d.h"

#include <cmath>

// ----------------------------------------------------------------------------
// Returns the largest item in the function list. Only requires operator<()
template <class T>
T max(T arg)
    { return arg; }

template <class T1, class... T2>
T1 max(T1 arg, T2&&... rest)
{
    T1 otherMax = max(std::forward<T2>(rest)...);
    return (arg < otherMax ? otherMax : arg);
}

// ----------------------------------------------------------------------------
CharacteristicsCurve3DWidget::CharacteristicsCurve3DWidget(QWidget* parent) :
    SurfacePlot(parent)
{
}

// ----------------------------------------------------------------------------
void CharacteristicsCurve3DWidget::addPVArray(const QString& name, QSharedPointer<PVArray> pvarray)
{
    if(m_Function.contains(name))
        throw std::runtime_error("Failed to add array \"" +
                                 std::string(name.toLocal8Bit().constData()) +
                                 "\": Duplicate name");

    QSharedPointer<CharacteristicsCurve3D> model(new CharacteristicsCurve3D(this, pvarray));

    m_Function.insert(name, model);

    model->setMesh(50, 50);
    model->setDomain(0, 3, 0, 100); // 0-3 amps, 0-100% exposure

    setRotation(30,0,-15);
    double largestAxis = max(model->getCurrentDomain(),
                             model->getExposureDomain(),
                             model->getVoltageDomain());
    setScale(largestAxis / model->getCurrentDomain(),
             largestAxis / model->getExposureDomain(),
             largestAxis / model->getVoltageDomain());
    setZoom(.25);

    for (unsigned i=0; i!=coordinates()->axes.size(); ++i)
    {
        coordinates()->axes[i].setMajors(7);
        coordinates()->axes[i].setMinors(4);
    }

    // TODO why the fuck isn't this working
    coordinates()->axes[Qwt3D::X1].setLabelString("Current (A)");
    coordinates()->axes[Qwt3D::Y1].setLabelString("Exposure (%)");
    coordinates()->axes[Qwt3D::Z1].setLabelString("Voltage (V)");
    coordinates()->axes[Qwt3D::X2].setLabelString("Current (A)");
    coordinates()->axes[Qwt3D::Y2].setLabelString("Exposure (%)");
    coordinates()->axes[Qwt3D::Z2].setLabelString("Voltage (V)");
    coordinates()->axes[Qwt3D::X3].setLabelString("Current (A)");
    coordinates()->axes[Qwt3D::Y3].setLabelString("Exposure (%)");
    coordinates()->axes[Qwt3D::Z3].setLabelString("Voltage (V)");

    setCoordinateStyle(Qwt3D::BOX);
    //setOrtho(false);
}

// ----------------------------------------------------------------------------
void CharacteristicsCurve3DWidget::removePVArray(const QString& name)
{
    m_Function.remove(name);
}

// ----------------------------------------------------------------------------
void CharacteristicsCurve3DWidget::replot()
{
    for(const auto& func : m_Function)
        func->create();

    updateData();
    updateGL();
}
