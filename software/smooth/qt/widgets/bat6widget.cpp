#include "bat6widget.h"
#include "ui_bat6widget.h"

#include "widgets/consolewidget.h"
#include "widgets/cellwidget.h"
#include "widgets/characteristicscurve2dwidget.h"
#include "widgets/characteristicscurve3dwidget.h"

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

// ----------------------------------------------------------------------------
BAT6Widget::BAT6Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BAT6Widget)
{
    ui->setupUi(this);

    // add a splitter object into the "UI" tab.
    QSplitter* splitter = new QSplitter;
    ui->tabUiLayout->addWidget(splitter);

    // add a scroll area to the left side of the splitter - this is where
    // all of the cell widgets are inserted
    QScrollArea* scrollArea = new QScrollArea;
    splitter->addWidget(scrollArea);
    scrollArea->setLayout(new QVBoxLayout);
    scrollArea->layout()->setAlignment(Qt::Alignment(Qt::AlignTop));

    for(int i = 1; i != 3; ++i)
    {
        QString name = "Cell " + QString::number(i);
        scrollArea->layout()->addWidget(new CellWidget(name));
    }

    // on the right side of the splitter are the plots - add a frame widget
    // to contain them
    QWidget* plotContainer = new QFrame;
    splitter->addWidget(plotContainer);
    QGridLayout* plotLayout = new QGridLayout;
    plotContainer->setLayout(plotLayout);

    // add 3D plot
    cc3d = new CharacteristicsCurve3DWidget();
    cc3d->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    plotLayout->addWidget(cc3d);
    // add 2D plot
    cc2d = new CharacteristicsCurve2DWidget();
    cc2d->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    plotLayout->addWidget(cc2d);

    QSharedPointer<PVArray> pvarray(new PVArray);
    PVChain pvchain;
    pvchain.addCell("cell 1", PVCell(6, 3, 2));
    pvchain.addCell("cell 2", PVCell(6, 3, 2));
    pvchain.addCell("cell 3", PVCell(6, 3, 2));
    pvchain.addCell("cell 4", PVCell(6, 3, 2));
    pvarray->addChain("chain 1", pvchain);
    cc3d->addPVArray("array 1", pvarray);
    cc3d->replot();

    cc2d->addPVArray("array 1", pvarray);
    cc2d->replot();

    // configure console
    console = new ConsoleWidget;
    console->setEnabled(true);
    ui->tabConsoleLayout->addWidget(console);
}

// ----------------------------------------------------------------------------
BAT6Widget::~BAT6Widget()
{
}

