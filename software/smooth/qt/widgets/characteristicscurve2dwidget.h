#ifndef CHARACTERISTICCURVE2DWIDGET_H
#define CHARACTERISTICCURVE2DWIDGET_H

#include "widgets/pvarrayplotwidgetbase.h"

#include <QSharedPointer>
#include <QMap>
#include <QString>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>

class QwtPlotCurve;
class IVCharacteristicsCurve;

class CharacteristicsCurve2DWidget :
        public QwtPlot,
        public PVArrayPlotWidgetBase
{
    Q_OBJECT

public:
    CharacteristicsCurve2DWidget(QWidget* parent=nullptr);
    ~CharacteristicsCurve2DWidget();

    virtual void addPVArray(const QString &name, QSharedPointer<PVArray> pvarray) override;
    virtual void removePVArray(const QString &name) override;
    virtual void replot() override;

private:
    QwtPlotCurve* curve;
    QMap<QString, QSharedPointer<IVCharacteristicsCurve>> m_Function;
};

#endif // CHARACTERISTICCURVE2DWIDGET_H
