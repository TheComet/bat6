#include "widgets/characteristicscurve3dwidget.h"
#include "widgets/cellwidget.h"
#include "models/pvarray.h"
#include "models/pvchain.h"
#include "models/pvcell.h"
#include "plot/ivcharacteristicscurve3d.h"
#include "plot/powercurve3d.h"

#include <QtGlobal>
#include <qwt3d_colorlegend.h>

#include <cmath>
#include <iostream>

// ----------------------------------------------------------------------------
// Returns the largest item in the function list. Only requires operator<()
#if __cplusplus <= 199711L
template <class T1, class T2>
T1 customMax(T1 arg1, T2 arg2) {
    return (arg1 < arg2 ? arg2 : arg1);
}
template <class T1, class T2, class T3>
T1 customMax(T1 arg1, T2 arg2, T3 arg3) {
    T1 otherMax = customMax(arg1, arg2);
    return (arg3 < otherMax ? otherMax : arg3);
}
#else
template <class T1, class T2>
T1 customMax(T1 arg1, T2 arg2) {
    return (arg1 < arg2 ? arg2 : arg1);
}
template <class T1, class... T2>
T1 customMax(T1 arg, T2&&... rest)
{
    T1 otherMax = max(std::forward<T2>(rest)...);
    return (arg < otherMax ? otherMax : arg);
}
#endif

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

    this->setCoordinateStyle(Qwt3D::BOX);
    this->setOrtho(false);

    this->setTitle(QString("Power(Voltage, Irradiation)"));

    std::cout << "hasOpenGL: " << this->format().hasOpenGL() << std::endl;
    std::cout << "hasOpenGLOverlays: " << this->format().hasOpenGLOverlays() << std::endl;
    std::cout << "openGLVersionFlags: " << this->format().openGLVersionFlags() << std::endl;
    std::cout << "Version: " << (const char*)glGetString(GL_VERSION) << std::endl;
    std::cout << "Renderer: " << (const char*)glGetString(GL_RENDERER) << std::endl;
    std::cout << "Vendor: " << (const char*)glGetString(GL_VENDOR) << std::endl;
    std::cout << "extensions: " << (const char*)glGetString(GL_EXTENSIONS) << std::endl;
}

// ----------------------------------------------------------------------------
void CharacteristicsCurve3DWidget::normaliseScale()
{
    double largestAxis = 0;
    double largestXAxis = 0;
    double largestYAxis = 0;
    double largestZAxis = 0;
    for(QMap<QString, QSharedPointer<Curve3DBase>>::iterator it = m_Function.begin();
        it != m_Function.end(); ++it)
    {
        QSharedPointer<Curve3DBase> model = it.value();

        // finds the largest axis among all functions
        double largestModelAxis = customMax(model->getBoundingRect().width(),
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

    this->setScale(largestAxis / largestXAxis,
                   largestAxis / largestYAxis,
                   largestAxis / largestZAxis);

    this->swapBuffers();
    this->swapBuffers();
    this->swapBuffers();
}

#include <GL/GLU.h>

void printOglError()
{
    GLenum glErr;
    glErr = glGetError();
    if(glErr != GL_NO_ERROR)
        std::cout << "Error: " << (const char*)gluErrorString(glErr);
    else
        std::cout << "No glError found";
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
    this->setRotation(30,0,-15);
    this->setZoom(.5);

    printOglError();
}

// ----------------------------------------------------------------------------
void CharacteristicsCurve3DWidget::removePVArray(const QString& name)
{
    m_Function.remove(name);
}

// ----------------------------------------------------------------------------
void CharacteristicsCurve3DWidget::replot()
{
    for(QMap<QString, QSharedPointer<Curve3DBase>>::iterator it = m_Function.begin();
        it != m_Function.end(); ++it)
        it.value()->create();

    updateData();
    updateGL();
}
