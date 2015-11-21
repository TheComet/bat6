#include "bat6window.h"
#include "ui_bat6window.h"
#include "console.h"
#include "cell.h"
#include <QPushButton>

BAT6Window::BAT6Window(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BAT6Window)
{
    ui->setupUi(this);

    // configure console
    console = new Console;
    console->setEnabled(true);
    console->setLocalEchoEnabled(true);
    ui->grid_layout_console->addWidget(console);
}

BAT6Window::~BAT6Window()
{
}

void BAT6Window::openSerialPort()
{
}

void BAT6Window::onReadData()
{
    QByteArray data; // = something, TODO
    console->putData(data);
}
