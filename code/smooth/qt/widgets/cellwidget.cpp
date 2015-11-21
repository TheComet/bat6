#include "cellwidget.h"
#include "ui_cellwidget.h"

CellWidget::CellWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CellWidget)
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
