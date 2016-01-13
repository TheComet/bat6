#include "widgets/characteristicscurve2dwidget.h"
#include "models/pvarray.h"
#include "plot/ivcharacteristicscurve.h"
#include "plot/powercurve.h"

#include <qwt/qwt_point_data.h>
#include <qwt/qwt_scale_engine.h>
#include <qwt/qwt_text_label.h>
#include <qwt/qwt_legend_data.h>
#include <qwt/qwt_legend_label.h>
#include <qwt/qwt_plot_grid.h>
#include <qwt/qwt_plot_legenditem.h>

#include <QColor>

// ----------------------------------------------------------------------------
CharacteristicsCurve2DWidget::CharacteristicsCurve2DWidget(QWidget* parent) :
    QwtPlot(parent),
    m_IVCurve(new QwtPlotCurve),
    m_PowerCurve(new QwtPlotCurve)
{
    this->setTitle("IV Characteristic Curve and Power");

    // IV curve
    m_IVCurve->attach(this);
    m_IVCurve->setTitle(QString("IV Characteristic"));
    m_IVCurve->setItemAttribute(QwtPlotItem::AutoScale, true);
    m_IVCurve->setPen(QPen(QColor(0, 0, 255)));

    // Power curve
    this->enableAxis(QwtPlot::yRight);
    m_PowerCurve->attach(this);
    m_PowerCurve->setTitle(QString("Power"));
    m_PowerCurve->setAxes(QwtPlot::xBottom, QwtPlot::yRight);
    m_PowerCurve->setPen(QPen(QColor(255, 0, 0)));

    // set axis titles
    this->setAxisTitle(QwtPlot::yLeft, QString("Current (I)"));
    this->setAxisTitle(QwtPlot::yRight, QString("Power (W)"));
    this->setAxisTitle(QwtPlot::xBottom, QString("Voltage (V)"));

    // Add a dotted grid
    QwtPlotGrid* grid = new QwtPlotGrid;
    grid->enableX(true);
    grid->enableY(true);
    grid->setMajorPen(QPen(Qt::black, 0, Qt::DotLine));
    grid->setMinorPen(QPen(Qt::gray, 0 , Qt::DotLine));
    grid->attach(this);

    // legend
    QwtPlotLegendItem* legend = new QwtPlotLegendItem();
    legend->attach(this);
    legend->setAlignment(Qt::Alignment(Qt::AlignRight | Qt::AlignTop));
    legend->setMaxColumns(1);

    // configure plot
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
    // TODO map array names to curve objects

    m_IVCurve->setData(new IVCharacteristicsCurve(pvarray));
    m_PowerCurve->setData(new PowerCurve(pvarray));
}

// ----------------------------------------------------------------------------
void CharacteristicsCurve2DWidget::removePVArray(const QString& name)
{
    // TODO
}

// ----------------------------------------------------------------------------
void CharacteristicsCurve2DWidget::autoScale()
{
    double maxPower = 0;
    double maxVoltage = 0;
    double maxCurrent = 0;

    IVCharacteristicsCurve* ivCurve = dynamic_cast<IVCharacteristicsCurve*>(m_IVCurve->data());
    PowerCurve* powerCurve = dynamic_cast<PowerCurve*>(m_PowerCurve->data());
    if(!ivCurve || !powerCurve)
        return;
    ivCurve->updateBoundingBox();
    powerCurve->updateBoundingBox();

    for(unsigned int i = 0; i != ivCurve->size(); ++i)
    {
        const QPointF& iv = ivCurve->sample(i);
        double power = iv.x() * iv.y();
        maxPower = (power > maxPower ? power : maxPower);
        maxVoltage = (ivCurve->getVoltageDomain() > maxVoltage ? ivCurve->getVoltageDomain() : maxVoltage);
        maxCurrent = (ivCurve->getCurrentDomain() > maxCurrent ? ivCurve->getCurrentDomain() : maxCurrent);
    }

    this->setAxisScale(QwtPlot::xBottom, 0, maxVoltage);
    this->setAxisScale(QwtPlot::yLeft, 0, maxCurrent);
    this->setAxisScale(QwtPlot::yRight, 0, maxPower);
}

// ----------------------------------------------------------------------------
void CharacteristicsCurve2DWidget::replot()
{
    this->autoScale();
    QwtPlot::replot();
}
