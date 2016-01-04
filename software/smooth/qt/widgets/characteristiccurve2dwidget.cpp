#include "characteristiccurve2dwidget.h"

#include <QColor>
#include <QDebug>

CharacteristicCurve2DWidget::CharacteristicCurve2DWidget(QWidget* parent) :
    QwtPlot(parent),
    curve(new QwtPlotCurve)
{
    static const double x[] = {1, 2, 5, 3};
    static const double y[] = {5, 2, 3, 4};
    //curve->setData(x, y, 4);
    curve->attach(this);
    this->setCanvasBackground(QColor(Qt::white));
    this->replot();
}

CharacteristicCurve2DWidget::~CharacteristicCurve2DWidget()
{
}
