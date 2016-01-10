#ifndef PV_ARRAY_PLOT_H
#define PV_ARRAY_PLOT_H

#include <QString>
#include <QSharedPointer>

class PVArray;
class PVModelFunction;

class PVArrayPlot
{
public:
    /// Virtual destructor, this is a polymorphic class
    virtual ~PVArrayPlot();

    /*!
     * \brief Adds a new PVArray object to
     * \param name
     * \param pvarray
     */
    void addPVArray(const QString& name, QSharedPointer<PVArray> pvarray);
    void removePVArray(const QString& name);
    QSharedPointer<PVArray> getPVArray(const QString& name);

    virtual void replot() = 0;

protected:
    QMap<QString, QSharedPointer<PVArray>> m_PVArrays;
};

#endif // PV_ARRAY_PLOT_H
