#include "plot/characteristicscurve3d.h"
#include "models/pvarray.h"

// ----------------------------------------------------------------------------
CharacteristicsCurve3D::CharacteristicsCurve3D(Qwt3D::SurfacePlot* pw,
                                               QSharedPointer<PVArray> pvarray) :
    Function(pw),
    PVModelFunctionBase(pvarray)
{
}

// ----------------------------------------------------------------------------
double CharacteristicsCurve3D::operator()(double current, double exposure)
{
    double exposureTemp = m_PVArray->getExposure();
    m_PVArray->setExposure(exposure * 0.01); // convert percent to absolute
    double voltage = m_PVArray->calculateVoltage(current);
    m_PVArray->setExposure(exposureTemp);
    return voltage;
}

// ----------------------------------------------------------------------------
void CharacteristicsCurve3D::updateBoundingBox()
{
    // we want to calculate the bounding box of an array where the exposure is
    // set to its maximum
    PVArray tempPVArray = m_PVArray.operator*();
    tempPVArray.setExposure(1);
    for(PVChain& chain : tempPVArray.getChains())
    {
        for(PVCell& cell : chain.getCells())
            cell.setExposure(1);
        chain.setExposure(1);
    }

    double maxVoltage = tempPVArray.calculateVoltage(0); // 0 amps = open circuit
    double maxCurrent = tempPVArray.calculateCurrent(0); // 0 volts = short circuit

    m_BoundingBox.setRect(0, 0, maxVoltage, maxCurrent);
}
