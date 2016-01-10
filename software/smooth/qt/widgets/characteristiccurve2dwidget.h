#ifndef CHARACTERISTICCURVE2DWIDGET_H
#define CHARACTERISTICCURVE2DWIDGET_H

#include "widgets/PVArrayPlot.h"

#include <QScopedPointer>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>

class QwtPlotCurve;

class CharacteristicCurve2DWidget :
        public QwtPlot,
        public PVArrayPlot
{
    Q_OBJECT

public:
    CharacteristicCurve2DWidget(QWidget* parent=nullptr);
    ~CharacteristicCurve2DWidget();

    virtual void addPVArray(const QString &name, QSharedPointer<PVArray> pvarray) override;
    virtual void removePVArray(const QString &name) override;
    virtual void replot() override;

private:
    QwtPlotCurve* curve;
};

#endif // CHARACTERISTICCURVE2DWIDGET_H
