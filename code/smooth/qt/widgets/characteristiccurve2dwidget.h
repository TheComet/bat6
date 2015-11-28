#ifndef CHARACTERISTICCURVE2DWIDGET_H
#define CHARACTERISTICCURVE2DWIDGET_H

#include <QScopedPointer>
#include <qwt5/qwt_plot.h>
#include <qwt5/qwt_plot_curve.h>

class QwtPlotCurve;

class CharacteristicCurve2DWidget : public QwtPlot
{
    Q_OBJECT

public:
    CharacteristicCurve2DWidget(QWidget* parent=nullptr);
    ~CharacteristicCurve2DWidget();

private:
    QwtPlotCurve* curve;
};

#endif // CHARACTERISTICCURVE2DWIDGET_H
