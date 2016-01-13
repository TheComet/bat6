#include "plot/pvmodelfunctionbase.h"
#include "models/pvarray.h"

// ----------------------------------------------------------------------------
PVModelFunctionBase::PVModelFunctionBase(QSharedPointer<PVArray> pvarray) :
    m_PVArray(pvarray)
{
}

// ----------------------------------------------------------------------------
PVModelFunctionBase::~PVModelFunctionBase()
{
}
