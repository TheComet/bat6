#include "plot/pvmodelfunctionbase.h"
#include "models/pvarray.h"

// ----------------------------------------------------------------------------
PVModelFunctionBase::PVModelFunctionBase(QSharedPointer<PVArray> pvarray) :
    m_PVArray(pvarray)
{
    this->updateBoundingBox();
}

// ----------------------------------------------------------------------------
PVModelFunctionBase::~PVModelFunctionBase()
{
}

// ----------------------------------------------------------------------------
void PVModelFunctionBase::updateBoundingBox()
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
    m_MaxCurrent = tempPVArray.calculateCurrent(0); // 0 volts = short circuit
    m_MinVoltage = 0.0;
    m_MinCurrent = 0.0;
    m_MaxExposure = 100.0;
    m_MinExposure = 0.0;
}
