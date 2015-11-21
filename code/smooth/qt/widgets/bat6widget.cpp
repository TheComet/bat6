#include "bat6widget.h"
#include "ui_bat6widget.h"
#include "consolewidget.h"
#include "cellwidget.h"
#include "plotwidget.h"

#include <QPushButton>
#include <QSplitter>
#include <QSpacerItem>
#include <QDebug>

BAT6Widget::BAT6Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BAT6Widget)
{
    ui->setupUi(this);

    // add a splitter object into the "UI" tab.
    QSplitter* splitter = new QSplitter;
    ui->layout_ui->addWidget(splitter);

    // on the left side of the splitter is the cell layout
    QWidget* cellContainer = new QWidget;
    splitter->addWidget(cellContainer);

    // on the right side of the splitter is the plot
    QWidget* plotContainer = new QWidget;
    splitter->addWidget(plotContainer);

    QGridLayout* plotLayout = new QGridLayout;
    plotContainer->setLayout(plotLayout);
    PlotWidget* plotWidget1 = new PlotWidget(false);
    plotLayout->addWidget(plotWidget1);
    PlotWidget* plotWidget2 = new PlotWidget(true);
    plotLayout->addWidget(plotWidget2);

    // the cell container uses a grid layout for positioning each cell
    layoutCells = new QGridLayout;
    cellContainer->setLayout(layoutCells);

    for(int i = 1; i != 5; ++i)
    {
        CellWidget* cellWidget = new CellWidget("cell " + QString::number(i));
        layoutCells->addWidget(cellWidget);
        connect(cellWidget, SIGNAL(exposureChanged(CellWidget,double)),
                plotWidget1, SLOT(onCellExposureChanged(CellWidget,double)));
        connect(cellWidget, SIGNAL(exposureChanged(CellWidget,double)),
                plotWidget2, SLOT(onCellExposureChanged(CellWidget,double)));
    }

    // configure console
    console = new ConsoleWidget;
    console->setEnabled(true);
    console->setLocalEchoEnabled(true);
    ui->layout_console->addWidget(console);
}

BAT6Widget::~BAT6Widget()
{
}

void BAT6Widget::openSerialPort()
{
}

void BAT6Widget::onReadData()
{
    QByteArray data; // = something, TODO
    console->putData(data);
}
