#include "widgets/characteristiccurve2dwidget.h"
#include "models/pvarray.h"

#include <qwt/qwt_point_data.h>

#include <QColor>
#include <QDebug>

class PVModelFunction : public QwtSeriesData<QPointF>
{
public:
    PVModelFunction(QSharedPointer<PVArray> pvarray) :
        m_PVArray(pvarray)
    {
    }

    virtual size_t size() const override
    {

    }

    virtual QPointF sample(size_t i) const override
    {
    }

    virtual QRectF boundingRect() const override
    {

    }

private:
    QSharedPointer<PVArray> m_PVArray;
};

// ----------------------------------------------------------------------------
CharacteristicCurve2DWidget::CharacteristicCurve2DWidget(QWidget* parent) :
    QwtPlot(parent),
    curve(new QwtPlotCurve)
{
    /*static const double x[] = {1, 2, 5, 3};
    static const double y[] = {1, 2, 3, 4};

    curve->setData(new QwtCPointerData(x, y, 4));
    curve->attach(this);*/
    this->setCanvasBackground(QColor(Qt::white));
    this->replot();
}

// ----------------------------------------------------------------------------
CharacteristicCurve2DWidget::~CharacteristicCurve2DWidget()
{
}

// ----------------------------------------------------------------------------
void CharacteristicCurve2DWidget::addPVArray(const QString& name, QSharedPointer<PVArray> pvarray)
{

}

// ----------------------------------------------------------------------------
void CharacteristicCurve2DWidget::removePVArray(const QString& name)
{

}

// ----------------------------------------------------------------------------
void CharacteristicCurve2DWidget::replot()
{
    QwtPlot::replot();
}
