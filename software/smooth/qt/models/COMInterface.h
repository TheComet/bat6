#ifndef COM_INTERFACE_H
#define COM_INTERFACE_H

class COMInterface
{
public:
    virtual ~COMInterface();
    virtual bool open(const QString& comName) = 0;
    virtual void close() = 0;
};

#endif // COM_INTERFACE_H
