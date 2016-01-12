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
    if(m_Function.contains(name))
        throw std::runtime_error("Failed to add array \"" +
                                 std::string(name.toLocal8Bit().constData()) +
                                 "\": Duplicate name");

    QSharedPointer<IVCharacteristicsCurve> model(new IVCharacteristicsCurve(pvarray));
    m_Function.insert(name, model);

    m_IVCurve->setData(new IVCharacteristicsCurve(pvarray));
    m_PowerCurve->setData(new PowerCurve(pvarray));

    this->setAxisScale(QwtPlot::xBottom, 0, 24);
    this->setAxisScale(QwtPlot::yLeft, 0, 3);
    this->setAxisScale(QwtPlot::yRight, 0, 40);
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
