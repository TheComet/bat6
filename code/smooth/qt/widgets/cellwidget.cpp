#include "cellwidget.h"
#include "ui_cellwidget.h"

CellWidget::CellWidget(const QString& cellName, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CellWidget),
    cellName(cellName)
{
    ui->setupUi(this);
}

CellWidget::~CellWidget()
{
}

void CellWidget::allowRemovingFromChain(bool allow)
{
    if(allow)
        ui->push_button_remove->show();
    else
        ui->push_button_remove->hide();
}

const QString& CellWidget::getName() const
{
    return cellName;
}

void CellWidget::on_intensity_valueChanged(int value)
{
    ui->label_intensity->setText(QString::number(value) + "%");
    double intensity = 0.01 * static_cast<double>(value);
    emit exposureChanged(*this, intensity);
}
