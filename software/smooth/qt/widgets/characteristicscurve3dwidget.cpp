#include "widgets/characteristicscurve3dwidget.h"
#include "widgets/cellwidget.h"
#include "models/pvarray.h"
#include "models/pvchain.h"
#include "models/pvcell.h"
#include "plot/characteristicscurve3d.h"

#include <qwtplot3d/qwt3d_colorlegend.h>

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
void CharacteristicsCurve3DWidget::normaliseScale()
{
    double largestAxis = 0;
    double largestXAxis = 0;
    double largestYAxis = 0;
    double largestZAxis = 0;
    for(const auto& model : m_Function)
    {
        model->updateBoundingBox();

        // finds the largest axis among all functions
        double largestModelAxis = max(model->getBoundingRect().width(),
                                      model->getBoundingRect().height(),
                                      100); // exposure max
        largestAxis =  (largestModelAxis > largestAxis ? largestModelAxis : largestAxis);

        // finds the largest single axes among all functions
        largestXAxis = (model->getBoundingRect().height() > largestXAxis ?
                        model->getBoundingRect().height() : largestXAxis);
        largestYAxis = 100; // exposure is always 100
        largestZAxis = (model->getBoundingRect().width() > largestZAxis ?
                        model->getBoundingRect().width() : largestZAxis);
    }

    setScale(largestAxis / largestXAxis,
             largestAxis / largestYAxis,
             largestAxis / largestZAxis);
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

    model->setMesh(80, 80);
    model->setDomain(0, 3, 0, 100); // 0-3 amps, 0-100% exposure

    this->normaliseScale();
    setRotation(30,0,-15);
    setZoom(.5);

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

    this->normaliseScale();
    updateData();
    updateGL();
}
