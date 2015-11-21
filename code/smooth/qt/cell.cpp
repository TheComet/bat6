#include "cell.h"
#include "ui_cell.h"

Cell::Cell(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Cell)
{
    ui->setupUi(this);
}

Cell::~Cell()
{
}

void Cell::allowRemovingFromChain(bool allow)
{
    if(allow)
        ui->push_button_remove->show();
    else
        ui->push_button_remove->hide();
}
