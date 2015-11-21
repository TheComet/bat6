#include "bat6widget.h"
#include "ui_bat6widget.h"
#include "consolewidget.h"
#include "cellwidget.h"
#include "plotwidget.h"

#include <QPushButton>
#include <QSplitter>
#include <QSpacerItem>


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
    splitter->addWidget(new PlotWidget);

    // the cell container uses a grid layout for positioning each cell
    QGridLayout* cellContainerLayout = new QGridLayout;
    layoutCells = new QGridLayout;
    cellContainerLayout->addLayout(layoutCells, 0, 0, 1, 1);
    cellContainerLayout->addItem(new QSpacerItem(0, 1000), 1, 0);
    cellContainer->setLayout(cellContainerLayout);

    layoutCells->addWidget(new CellWidget);

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
