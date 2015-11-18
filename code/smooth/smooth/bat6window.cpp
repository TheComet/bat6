#include "bat6window.h"
#include "ui_bat6window.h"

BAT6Window::BAT6Window(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BAT6Window)
{
    ui->setupUi(this);
}

BAT6Window::~BAT6Window()
{
}
