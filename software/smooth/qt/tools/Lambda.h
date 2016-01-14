#ifndef LAMBDA_H
#define LAMBDA_H

#include <functional>
#include <QObject>

class Lambda : public QObject
{
    Q_OBJECT

public slots:
    void call() { if(fptr) fptr(); }

public:
    Lambda(std::function<void()> l, QObject* parent=0) : QObject(parent), fptr(l)
    {}

private:
    std::function<void()> fptr;
};

#endif // LAMBDA_H

