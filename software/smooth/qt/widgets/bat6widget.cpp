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
    /*QSplitter* splitter = new QSplitter;
    ui->tabUiLayout->addWidget(splitter);

    // add a scroll area to the left side of the splitter - this is where
    // all of the cell widgets are displayed
    QScrollArea* scrollArea = new QScrollArea;
    splitter->addWidget(scrollArea);*/
    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setSizePolicy(QSizePolicy::Policy::Minimum,
                              QSizePolicy::Policy::Minimum);
    scrollArea->setMinimumWidth(280);
    ui->group_box_cells->setLayout(new QGridLayout);
    ui->group_box_cells->layout()->addWidget(scrollArea);
    scrollArea->setWidget(m_CellWidgetContainer);

    // the scroll area gives a view into a QFrame object. Configure the
    // frame with a layout to contain the cell widgets
    m_CellWidgetContainer->setLayout(new QVBoxLayout);
    m_CellWidgetContainer->layout()->setAlignment(Qt::Alignment(Qt::AlignTop));
    m_CellWidgetContainer->layout()->setSizeConstraint(QLayout::SetMinAndMaxSize);
    m_CellWidgetContainer->layout()->setSizeConstraint(QLayout::SetMinimumSize);

    // button for adding cells
    m_CellWidgetContainer->layout()->addWidget(m_AddCellButton);

    // add 3D plot
    ui->group_box_plots->setLayout(new QHBoxLayout);
    ui->group_box_plots->layout()->addWidget(m_cc3d);
    m_cc3d->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    // add 2D plot
    ui->group_box_plots->layout()->addWidget(m_cc2d);
    m_cc2d->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);

    // create model with no cells
    m_PVArray = QSharedPointer<PVArray>(new PVArray);
    m_PVArray->addChain("Chain 1", PVChain());

    // add model to plots
    m_cc3d->addPVArray("Array 1", m_PVArray);
    m_cc2d->addPVArray("Array 1", m_PVArray);

    // configure console
    m_Console->setEnabled(true);
    ui->tabConsoleLayout->addWidget(m_Console);

    // signals
    this->connect(m_AddCellButton, SIGNAL(released()), this, SLOT(onAddCellButtonReleased()));
    this->connect(ui->slider_global_exposure, SIGNAL(valueChanged(int)), this, SLOT(onGlobalExposureChanged(int)));

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
    // connect open circuit voltage spinbox
    this->connect(cellWidget->ui->open_circuit_voltage, SIGNAL(valueChanged(double)), new Lambda([this, cellWidget]() {
        this->onOpenCircuitVoltageChanged(cellWidget);
    }, cellWidget), SLOT(call()));
    // connect short circuit current spinbox
    this->connect(cellWidget->ui->short_circuit_current, SIGNAL(valueChanged(double)), new Lambda([this, cellWidget]() {
        this->onShortCircuitCurrentChanged(cellWidget);
    }, cellWidget), SLOT(call()));
    // connect dark voltage spinbox
    this->connect(cellWidget->ui->dark_voltage, SIGNAL(valueChanged(double)), new Lambda([this, cellWidget]() {
        this->onDarkVoltageChanged(cellWidget);
    }, cellWidget), SLOT(call()));
    // connect intensity slider
    this->connect(cellWidget->ui->intensity, SIGNAL(valueChanged(int)), new Lambda([this, cellWidget]() {
        this->onCellExposureChanged(cellWidget);
    }, cellWidget), SLOT(call()));

    // add cell to model too - copy parameters from first cell
    auto chain = m_PVArray->getChains().find("Chain 1");
    const auto& templateCell = chain->getCells().find("Cell 1");
    if(templateCell == chain->getCells().end())
    {
        chain->addCell(name, PVCell(6, 3, 2));
        cellWidget->ui->open_circuit_voltage->setValue(6);
        cellWidget->ui->short_circuit_current->setValue(3);
        cellWidget->ui->dark_voltage->setValue(2);
        cellWidget->ui->intensity->setValue(100);
    }
    else
    {
        chain->addCell(name, PVCell(templateCell->getOpenCircuitVoltage(),
                                    templateCell->getShortCircuitCurrent(),
                                    templateCell->getDarkVoltage()));
        cellWidget->ui->open_circuit_voltage->setValue(templateCell->getOpenCircuitVoltage());
        cellWidget->ui->short_circuit_current->setValue(templateCell->getShortCircuitCurrent());
        cellWidget->ui->dark_voltage->setValue(templateCell->getDarkVoltage());
        cellWidget->ui->intensity->setValue(templateCell->getExposure() * 100);
    }

    this->autoScalePlots();
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
    this->autoScalePlots();
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
void BAT6Widget::autoScalePlots()
{
    m_cc2d->autoScale();
    m_cc3d->normaliseScale();
}

// ----------------------------------------------------------------------------
void BAT6Widget::onAddCellButtonReleased()
{
    this->addCell();
}

// ----------------------------------------------------------------------------
void BAT6Widget::onRemoveCellButtonReleased(CellWidget* cellWidget)
{
    this->removeCell(cellWidget);
}

// ----------------------------------------------------------------------------
void BAT6Widget::onOpenCircuitVoltageChanged(CellWidget* cellWidget)
{
    auto cell = m_PVArray->getChains().find("Chain 1")->getCells().find(cellWidget->getName());
    cell->setOpenCircuitVoltage(cellWidget->ui->open_circuit_voltage->value());
    this->updateModel();
}

// ----------------------------------------------------------------------------
void BAT6Widget::onShortCircuitCurrentChanged(CellWidget* cellWidget)
{
    auto cell = m_PVArray->getChains().find("Chain 1")->getCells().find(cellWidget->getName());
    cell->setShortCircuitCurrent(cellWidget->ui->short_circuit_current->value());
    this->updateModel();
}

// ----------------------------------------------------------------------------
void BAT6Widget::onDarkVoltageChanged(CellWidget* cellWidget)
{
    auto cell = m_PVArray->getChains().find("Chain 1")->getCells().find(cellWidget->getName());
    cell->setDarkVoltage(cellWidget->ui->dark_voltage->value());
    this->updateModel();
}

// ----------------------------------------------------------------------------
void BAT6Widget::onCellExposureChanged(CellWidget* cellWidget)
{
    auto cell = m_PVArray->getChains().find("Chain 1")->getCells().find(cellWidget->getName());
    cell->setExposure(cellWidget->ui->intensity->value() * 0.01);
    this->updateModel();
}

// ----------------------------------------------------------------------------
void BAT6Widget::onGlobalExposureChanged(int value)
{
    m_PVArray->setExposure(value * 0.01);
    ui->label_global_exposure->setText(QString::number(value) + "%");
    this->updateModel();
}
