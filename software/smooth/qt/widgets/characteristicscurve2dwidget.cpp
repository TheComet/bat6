#include "widgets/characteristicscurve2dwidget.h"
#include "models/pvarray.h"
#include "plot/ivcharacteristicscurve.h"
#include "plot/powercurve.h"

#include <qwt_point_data.h>
#include <qwt_scale_engine.h>
#include <qwt_text_label.h>
#include <qwt_legend_data.h>
#include <qwt_legend_label.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_legenditem.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>

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

    /*
     * TODO: This doesn't work yet
    QwtSymbol* sym = new QwtSymbol(QwtSymbol::Diamond, QBrush(Qt::red), QPen(Qt::red), QSize(5,5));
    QwtPlotMarker* marker=new QwtPlotMarker;
    marker->setSymbol(sym);
    marker->setValue(1, 10);
    marker->attach(this);
    */

    // configure plot
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
    // update bounding boxes of functions
    IVCharacteristicsCurve* ivCurve = dynamic_cast<IVCharacteristicsCurve*>(m_IVCurve->data());
    PowerCurve* powerCurve = dynamic_cast<PowerCurve*>(m_PowerCurve->data());
    if(!ivCurve || !powerCurve)
        return;
    ivCurve->updateBoundingBox();
    powerCurve->updateBoundingBox();

    this->setAxisScale(QwtPlot::xBottom, ivCurve->boundingRect().x(), ivCurve->boundingRect().width());
    this->setAxisScale(QwtPlot::yLeft, ivCurve->boundingRect().y(), ivCurve->boundingRect().height());
    this->setAxisScale(QwtPlot::yRight, powerCurve->boundingRect().y(), powerCurve->boundingRect().height());
}

// ----------------------------------------------------------------------------
void CharacteristicsCurve2DWidget::replot()
{
    QwtPlot::replot();
}
