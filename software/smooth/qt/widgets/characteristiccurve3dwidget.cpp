#include "characteristiccurve3dwidget.h"
#include "models/pvarray.h"
#include "models/pvchain.h"
#include "models/pvcell.h"
#include "widgets/cellwidget.h"

#include <qwtplot3d/qwt3d_function.h>
#include <cmath>

using namespace Qwt3D;

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
class PVModelFunction3D : public Function
{
public:
    PVModelFunction3D(SurfacePlot* pw, QSharedPointer<PVArray> pvarray) :
        Function(pw),
        m_PVArray(pvarray)
    {
        this->updateBoundingBox();
    }

    double getVoltageDomain() const
        { return m_MaxVoltage - m_MinVoltage; }
    double getCurrentDomain() const
        { return m_MaxCurrent - m_MinCurrent; }
    double getExposureDomain() const
        { return m_MaxExposure - m_MinExposure; }

    double operator()(double current, double exposure)
    {
        double exposureTemp = m_PVArray->getExposure();
        m_PVArray->setExposure(exposure * 0.01); // convert percent to absolute
        double voltage = m_PVArray->calculateVoltage(current);
        m_PVArray->setExposure(exposureTemp);
        return voltage;
    }

    void updateBoundingBox()
    {
        PVArray tempPVArray = m_PVArray.operator*();

        // calculate maximum voltage by setting the exposure of all cells to maximum
        tempPVArray.setExposure(1);
        for(PVChain& chain : tempPVArray.getChains())
        {
            for(PVCell& cell : chain.getCells())
                cell.setExposure(1);
            chain.setExposure(1);
        }

        m_MaxVoltage = tempPVArray.calculateVoltage(0); // 0 amps = open circuit
        m_MinVoltage = 0.0;
        m_MaxCurrent = tempPVArray.calculateCurrent(0); // 0 volts = short circuit
        m_MinCurrent = 0.0;
        m_MaxExposure = 100.0;
        m_MinExposure = 0.0;
    }

private:
    QSharedPointer<PVArray> m_PVArray;
    double m_MinVoltage;
    double m_MaxVoltage;
    double m_MinCurrent;
    double m_MaxCurrent;
    double m_MinExposure;
    double m_MaxExposure;
};

// ----------------------------------------------------------------------------
CharacteristicCurve3DWidget::CharacteristicCurve3DWidget(QWidget* parent) :
    SurfacePlot(parent)
{
}

// ----------------------------------------------------------------------------
void CharacteristicCurve3DWidget::addPVArray(const QString& name, QSharedPointer<PVArray> pvarray)
{
    if(m_Function.contains(name))
        throw std::runtime_error("Failed to add array \"" +
                                 std::string(name.toLocal8Bit().constData()) +
                                 "\": Duplicate name");

    QSharedPointer<PVModelFunction3D> model(new PVModelFunction3D(this, pvarray));

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

    coordinates()->axes[X1].setLabelString("Current (A)");
    coordinates()->axes[Y1].setLabelString("Exposure (%)");
    coordinates()->axes[Z1].setLabelString("Voltage (A)");

    setCoordinateStyle(BOX);
    //setOrtho(false);
}

// ----------------------------------------------------------------------------
void CharacteristicCurve3DWidget::removePVArray(const QString& name)
{
    m_Function.remove(name);
}

// ----------------------------------------------------------------------------
void CharacteristicCurve3DWidget::replot()
{
    for(const auto& func : m_Function)
        func->create();

    updateData();
    updateGL();
}
