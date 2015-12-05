#include "gmock/gmock.h"
#include "core/event.h"
#include "drv/hw.h"

using testing::Eq;

class dsPICGlobalEnvironment : public testing::Environment
{
public:
    virtual ~dsPICGlobalEnvironment() {}

    virtual void SetUp()
    {
        /* prevent waiting for PLL to lock */
        OSCCONbits.COSC = 0x01;
        OSCCONbits.LOCK = 1;

        /* prevent waiting for ADC cores */
        ADCON5Lbits.C0RDY = 1;
        ADCON5Lbits.C1RDY = 1;
        ADCAL0Lbits.CAL0RDY = 1;
        ADCAL0Lbits.CAL0RDY = 1;
        ADCAL0Lbits.CAL1RDY = 1;
        ADCAL0Lbits.CAL1RDY = 1;

        hw_init();
        drivers_init();
    }

    virtual void TearDown()
    {
        drivers_deinit();
    }
};

const testing::Environment* const memoryManagementEnvironment =
        testing::AddGlobalTestEnvironment(new dsPICGlobalEnvironment);
