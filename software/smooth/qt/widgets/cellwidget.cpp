#include "cellwidget.h"
#include "ui_cellwidget.h"

// ----------------------------------------------------------------------------
CellWidget::CellWidget(const QString& cellName, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CellWidget),
    m_CellName(cellName)
{
    ui->setupUi(this);
    ui->group_box->setTitle(cellName);
}

// ----------------------------------------------------------------------------
CellWidget::~CellWidget()
{
}

// ----------------------------------------------------------------------------
void CellWidget::allowRemovingFromChain(bool allow)
{
    (void)allow;
}

// ----------------------------------------------------------------------------
void CellWidget::setName(const QString& name)
{
    m_CellName = name;
    ui->group_box->setTitle(name);
}

// ----------------------------------------------------------------------------
const QString& CellWidget::getName() const
{
    return m_CellName;
}

// ----------------------------------------------------------------------------
void CellWidget::on_intensity_valueChanged(int value)
{
    ui->label_intensity->setText(QString::number(value) + "%");
    double intensity = 0.01 * static_cast<double>(value);
    emit exposureChanged(this, intensity);
}
