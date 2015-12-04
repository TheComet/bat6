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
        event_init();
        hw_init();
    }

    virtual void TearDown()
    {
    }
};

const testing::Environment* const memoryManagementEnvironment =
        testing::AddGlobalTestEnvironment(new dsPICGlobalEnvironment);

