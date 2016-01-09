#include "characteristiccurve2dwidget.h"

#include <qwt/qwt_point_data.h>

#include <QColor>
#include <QDebug>

CharacteristicCurve2DWidget::CharacteristicCurve2DWidget(QWidget* parent) :
    QwtPlot(parent),
    curve(new QwtPlotCurve)
{
    static const double x[] = {1, 2, 5, 3};
    static const double y[] = {1, 2, 3, 4};

    curve->setData(new QwtCPointerData(x, y, 4));
    curve->attach(this);
    this->setCanvasBackground(QColor(Qt::white));
    this->replot();
}

CharacteristicCurve2DWidget::~CharacteristicCurve2DWidget()
{
}
