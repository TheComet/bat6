#include "bat6widget.h"
#include "ui_bat6widget.h"

#include "widgets/consolewidget.h"
#include "widgets/cellwidget.h"
#include "widgets/characteristiccurve2dwidget.h"
#include "widgets/characteristiccurve3dwidget.h"

#include "models/pvarray.h"
#include "models/pvchain.h"
#include "models/pvcell.h"

#include "qaccordion/qaccordion.h"

#include "qwt/qwt_plot.h"
#include "qwt/qwt_plot_curve.h"

#include <QPushButton>
#include <QSplitter>
#include <QSpacerItem>
#include <QTableWidget>
#include <QDebug>

BAT6Widget::BAT6Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BAT6Widget)
{
    ui->setupUi(this);

    // add a splitter object into the "UI" tab.
    QSplitter* splitter = new QSplitter;
    ui->tabUiLayout->addWidget(splitter);

    // create a QFrame on the left side of the splitter and give it a layout
    /*
    QFrame* cellContainer = new QFrame;
    splitter->addWidget(cellContainer);
    cellLayout = new QGridLayout;
    cellContainer->setLayout(cellLayout);*/

    // add a scroll area and accordion widget to the left side of the splitter
    QScrollArea* scrollAreaTop = new QScrollArea;
    splitter->addWidget(scrollAreaTop);
    QAccordion* accordionTop = new QAccordion;
    scrollAreaTop->setWidget(accordionTop);
    scrollAreaTop->setWidgetResizable(true);

    for(int i = 0; i < 100; ++i)
    {
        accordionTop->addContentPane("Cell " + QString::number(i));
    }

    int paneIndex = accordionTop->addContentPane("Cell 1");
    /*QFrame* contentFrame = accordionTop->getContentPane(paneIndex)->getContentFrame();
    contentFrame->setLayout(new QGridLayout);
    contentFrame->layout()->addWidget(new CellWidget("Cell 1"));
    contentFrame->layout()->addWidget(new CellWidget("Cell 2"));
    contentFrame->layout()->addWidget(new CellWidget("Cell 3"));
    contentFrame->layout()->addWidget(new CellWidget("Cell 4"));*/


    // on the right side of the splitter is the plot
    QWidget* plotContainer = new QFrame;
    splitter->addWidget(plotContainer);
    QGridLayout* plotLayout = new QGridLayout;
    plotContainer->setLayout(plotLayout);

    // add the two plots, one does V(I) the other I(V)
    cc3d = new CharacteristicCurve3DWidget();
    cc3d->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    plotLayout->addWidget(cc3d);
    cc2d = new CharacteristicCurve2DWidget();
    cc2d->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    plotLayout->addWidget(cc2d);

    QSharedPointer<PVArray> pvarray(new PVArray);
    PVChain pvchain;
    pvchain.addCell("cell 1", PVCell(0.56, 2.41));
    pvchain.addCell("cell 2", PVCell(0.56, 2.41));
    pvchain.addCell("cell 3", PVCell(0.56, 2.41));
    pvchain.addCell("cell 4", PVCell(0.56, 2.41));
    pvarray->addChain("chain 1", pvchain);
    cc3d->addPVArray("array 1", pvarray);
    cc3d->replot();

    // add and connect the cell widgets
    /*
    for(int i = 1; i != 5; ++i)
    {
        CellWidget* cellWidget = new CellWidget("cell " + QString::number(i));
        cellLayout->addWidget(cellWidget);
        connect(cellWidget, SIGNAL(exposureChanged(CellWidget*,double)),
                this, SLOT(onCellExposureChanged(CellWidget*,double)));
    }*/

    // configure console
    console = new ConsoleWidget;
    console->setEnabled(true);
    ui->tabConsoleLayout->addWidget(console);
}

BAT6Widget::~BAT6Widget()
{
}

void BAT6Widget::openSerialPort()
{
}

void BAT6Widget::onCellExposureChanged(CellWidget* cellWidget, double exposure)
{
    cc3d->getPVArray("array 1")->getChain("chain 1")->getCell(cellWidget->getName())->setExposure(exposure);
    cc3d->replot();
}

void BAT6Widget::onReadData()
{
    QByteArray data; // = something, TODO
    console->putData(data);
}
