#include "bat6widget.h"
#include "ui_bat6widget.h"
#include "ui_cellwidget.h"

#include "widgets/consolewidget.h"
#include "widgets/cellwidget.h"
#include "widgets/characteristicscurve2dwidget.h"
#include "widgets/characteristicscurve3dwidget.h"

#include "models/pvarray.h"
#include "models/pvchain.h"
#include "models/pvcell.h"

#include "tools/Lambda.h"

#include "qwt/qwt_plot.h"
#include "qwt/qwt_plot_curve.h"

#include <QPushButton>
#include <QSplitter>
#include <QSpacerItem>
#include <QTableWidget>
#include <QScrollArea>

// ----------------------------------------------------------------------------
BAT6Widget::BAT6Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BAT6Widget),
    m_CellWidgetContainer(new QFrame(this)),
    m_Console(new ConsoleWidget(this)),
    m_cc2d(new CharacteristicsCurve2DWidget(this)),
    m_cc3d(new CharacteristicsCurve3DWidget(this)),
    m_AddCellButton(new QPushButton("Add Cell"))
{
    ui->setupUi(this);

    // add a splitter object into the "UI" tab.
    QSplitter* splitter = new QSplitter;
    ui->tabUiLayout->addWidget(splitter);

    // add a scroll area to the left side of the splitter - this is where
    // all of the cell widgets are displayed
    QScrollArea* scrollArea = new QScrollArea;
    splitter->addWidget(scrollArea);
    scrollArea->setWidget(m_CellWidgetContainer);

    // the scroll area gives a view into a QFrame object. Configure the
    // frame with a layout to contain the cell widgets
    m_CellWidgetContainer->setLayout(new QVBoxLayout);
    m_CellWidgetContainer->layout()->setAlignment(Qt::Alignment(Qt::AlignTop));
    m_CellWidgetContainer->layout()->setSizeConstraint(QLayout::SetMinAndMaxSize);

    // on the right side of the splitter are the plots - add a frame widget
    // to contain them
    QWidget* plotContainer = new QFrame;
    splitter->addWidget(plotContainer);
    QGridLayout* plotLayout = new QGridLayout;
    plotContainer->setLayout(plotLayout);

    // add 3D plot
    plotLayout->addWidget(m_cc3d);
    m_cc3d->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
    // add 2D plot
    plotLayout->addWidget(m_cc2d);
    m_cc2d->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);

    QList<int> sizes = splitter->sizes();
    sizes[0] = 150;
    sizes[1] = splitter->size().width() - sizes[0];
    splitter->setSizes(sizes);

    m_PVArray = QSharedPointer<PVArray>(new PVArray);
    PVChain pvchain;
    pvchain.addCell("Cell 1", PVCell(24, 3, 2));
    m_PVArray->addChain("Chain 1", pvchain);
    m_cc3d->addPVArray("Array 1", m_PVArray);
    m_cc3d->replot();

    m_cc2d->addPVArray("Array 1", m_PVArray);
    m_cc2d->replot();

    // configure console
    m_Console->setEnabled(true);
    ui->tabConsoleLayout->addWidget(m_Console);

    m_CellWidgetContainer->layout()->addWidget(m_AddCellButton);
    this->connect(m_AddCellButton, SIGNAL(released()), this, SLOT(onAddCellButtonReleased()));
    this->addCell();
}

// ----------------------------------------------------------------------------
BAT6Widget::~BAT6Widget()
{
}

// ----------------------------------------------------------------------------
void BAT6Widget::addCell()
{
    QLayout* layout = m_CellWidgetContainer->layout();

    // add a new cell widget
    QString name = "Cell " + QString::number(layout->count());
    CellWidget* cellWidget = new CellWidget(name);
    layout->addWidget(cellWidget);

    // connect the cell's "remove cell" button
    this->connect(cellWidget->ui->remove_cell, SIGNAL(released()), new Lambda([this, cellWidget]() {
            this->onRemoveCellButtonReleased(cellWidget);
    }, cellWidget), SLOT(call()));

    // add cell to model too - copy parameters from first cell
    auto chain = m_PVArray->getChains().find("Chain 1");
    const auto& templateCell = chain->getCells().find("Cell 1");
    if(templateCell == chain->getCells().end())
        chain->addCell(name, PVCell(6, 3, 2));
    else
        chain->addCell(name, PVCell(templateCell->getOpenCircuitVoltage(),
                                    templateCell->getShortCircuitCurrent(),
                                    templateCell->getDarkVoltage()));

    this->updateModel();
}

// ----------------------------------------------------------------------------
void BAT6Widget::removeCell(CellWidget* cell)
{
    if(m_CellWidgetContainer->layout()->count() <= 2)
        return;

    m_CellWidgetContainer->layout()->removeWidget(cell);
    cell->deleteLater();

    // remove cell from model too
    auto chain = m_PVArray->getChains().find("Chain 1");
    chain->getCells().remove(cell->getName());

    this->updateCellNames();
    this->updateModel();
}

// ----------------------------------------------------------------------------
void BAT6Widget::updateCellNames()
{
    QLayout* layout = m_CellWidgetContainer->layout();
    for(int i = 1; i < layout->count(); ++i)
    {
        QString name = "Cell " + QString::number(i);
        CellWidget* cell = qobject_cast<CellWidget*>(layout->itemAt(i)->widget());
        cell->setName(name);
    }

    this->updateModelCellNames();
}

// ----------------------------------------------------------------------------
void BAT6Widget::updateModelCellNames()
{
    PVChain newChain;
    int i = 1;
    for(auto& cell : m_PVArray->getChains().find("Chain 1")->getCells())
    {
        QString name = "Cell " + QString::number(i++);
        newChain.addCell(name, cell);
    }

    m_PVArray->removeChain("Chain 1");
    m_PVArray->addChain("Chain 1", newChain);
}

// ----------------------------------------------------------------------------
void BAT6Widget::updateModel()
{
    m_cc2d->replot();
    m_cc3d->replot();
}

// ----------------------------------------------------------------------------
void BAT6Widget::onAddCellButtonReleased()
{
    this->addCell();
}

// ----------------------------------------------------------------------------
void BAT6Widget::onRemoveCellButtonReleased(CellWidget* cell)
{
    this->removeCell(cell);
}
