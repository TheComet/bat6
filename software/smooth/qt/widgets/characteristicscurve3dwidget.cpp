#include "widgets/characteristicscurve3dwidget.h"
#include "widgets/cellwidget.h"
#include "models/pvarray.h"
#include "models/pvchain.h"
#include "models/pvcell.h"
#include "plot/ivcharacteristicscurve3d.h"
#include "plot/powercurve3d.h"
#include "plot/crosssection.h"

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
    for (unsigned i=0; i!=coordinates()->axes.size(); ++i)
    {
        coordinates()->axes[i].setMajors(5);
        coordinates()->axes[i].setMinors(5);
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
        largestXAxis = (model->getBoundingRect().width() > largestXAxis ?
                        model->getBoundingRect().width() : largestXAxis);
        largestYAxis = 100; // exposure is always 100
        largestZAxis = (model->getBoundingRect().height() > largestZAxis ?
                        model->getBoundingRect().height() : largestZAxis);

        // update domain
        model->setDomain(model->getBoundingRect().x(),
                         model->getBoundingRect().x() + model->getBoundingRect().width(),
                         0, 100); // exposure 0 to 100%
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

    QSharedPointer<Curve3DBase> model(new PowerCurve3D(pvarray, this));
    m_Function.insert(name, model);

    this->normaliseScale();
    model->setMesh(60, 60);
    setRotation(30,0,-15);
    setZoom(.5);

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
