#ifndef PV_ARRAY_PLOT_H
#define PV_ARRAY_PLOT_H

#include <QString>
#include <QSharedPointer>

class PVArray;

class PVArrayPlotWidgetBase
{
public:
    /// Virtual destructor, this is a polymorphic class
    virtual ~PVArrayPlotWidgetBase() {}

    /*!
     * \brief Adds a new PVArray object to
     * \param name
     * \param pvarray
     */
    virtual void addPVArray(const QString& name, QSharedPointer<PVArray> pvarray) = 0;
    virtual void removePVArray(const QString& name) = 0;
    virtual void replot() = 0;
};

#endif // PV_ARRAY_PLOT_H
