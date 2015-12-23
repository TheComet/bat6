#ifndef COM_UNIX_H
#define COM_UNIX_H

#include "models/COMInterface.h"

class COMUnix : public COMInterface
{
public:

    virtual bool open(const QString& comName) override;
    virtual void close() override;
};

#endif // COM_UNIX_H
