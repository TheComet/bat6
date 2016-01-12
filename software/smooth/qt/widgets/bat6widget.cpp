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
    ui(new Ui::BAT6Widget),
    cellWidgetContainer(new QFrame(this)),
    console(new ConsoleWidget(this)),
    cc2d(new CharacteristicsCurve2DWidget(this)),
    cc3d(new CharacteristicsCurve3DWidget(this))
{
    ui->setupUi(this);

    // add a splitter object into the "UI" tab.
    QSplitter* splitter = new QSplitter;
    ui->tabUiLayout->addWidget(splitter);

    // add a scroll area to the left side of the splitter - this is where
    // all of the cell widgets are displayed
    QScrollArea* scrollArea = new QScrollArea;
    splitter->addWidget(scrollArea);
    scrollArea->setWidget(cellWidgetContainer);

    // the scroll area gives a view into a QFrame object. Configure the
    // frame with a layout to contain the cell widgets
    cellWidgetContainer->setLayout(new QVBoxLayout);
    cellWidgetContainer->layout()->setAlignment(Qt::Alignment(Qt::AlignTop));
    cellWidgetContainer->layout()->setSizeConstraint(QLayout::SetMinAndMaxSize);

    // on the right side of the splitter are the plots - add a frame widget
    // to contain them
    QWidget* plotContainer = new QFrame;
    splitter->addWidget(plotContainer);
    QGridLayout* plotLayout = new QGridLayout;
    plotContainer->setLayout(plotLayout);

    // add 3D plot
    plotLayout->addWidget(cc3d);
    cc3d->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
    // add 2D plot
    plotLayout->addWidget(cc2d);
    cc2d->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);

    for(int i = 1; i != 5; ++i)
    {
        QString name = "Cell " + QString::number(i);
        CellWidget* cell = new CellWidget(name);
        cellWidgetContainer->layout()->addWidget(cell);
    }

    QList<int> sizes = splitter->sizes();
    sizes[0] = 140;
    sizes[1] = splitter->size().width() - sizes[0];
    splitter->setSizes(sizes);

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
    console->setEnabled(true);
    ui->tabConsoleLayout->addWidget(console);
}

// ----------------------------------------------------------------------------
BAT6Widget::~BAT6Widget()
{
}
