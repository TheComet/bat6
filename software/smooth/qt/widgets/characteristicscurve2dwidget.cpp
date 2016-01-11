#include "widgets/characteristicscurve2dwidget.h"
#include "models/pvarray.h"
#include "plot/ivcharacteristicscurve.h"

#include <qwt/qwt_point_data.h>
#include <qwt/qwt_scale_engine.h>

#include <QColor>
#include <QDebug>

// ----------------------------------------------------------------------------
CharacteristicsCurve2DWidget::CharacteristicsCurve2DWidget(QWidget* parent) :
    QwtPlot(parent),
    curve(new QwtPlotCurve)
{
    curve->attach(this);
    curve->setItemAttribute(QwtPlotItem::AutoScale, true);
    this->setAxisAutoScale(QwtPlot::xBottom);
    this->setCanvasBackground(QColor(Qt::white));
    this->replot();
}

// ----------------------------------------------------------------------------
CharacteristicsCurve2DWidget::~CharacteristicsCurve2DWidget()
{
}

// ----------------------------------------------------------------------------
void CharacteristicsCurve2DWidget::addPVArray(const QString& name, QSharedPointer<PVArray> pvarray)
{
    if(m_Function.contains(name))
        throw std::runtime_error("Failed to add array \"" +
                                 std::string(name.toLocal8Bit().constData()) +
                                 "\": Duplicate name");

    QSharedPointer<IVCharacteristicsCurve> model(new IVCharacteristicsCurve(pvarray));
    m_Function.insert(name, model);

    curve->setData(new IVCharacteristicsCurve(pvarray));
    this->setAxisScale(QwtPlot::xBottom, 0, 24);
    this->setAxisScale(QwtPlot::yLeft, 0, 3);
}

// ----------------------------------------------------------------------------
void CharacteristicsCurve2DWidget::removePVArray(const QString& name)
{
    m_Function.remove(name);
}

// ----------------------------------------------------------------------------
void CharacteristicsCurve2DWidget::replot()
{
    QwtPlot::replot();
}
