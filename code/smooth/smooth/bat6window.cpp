#include "bat6window.h"
#include "ui_bat6window.h"
#include "console.h"

#include <QSerialPort>

BAT6Window::BAT6Window(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BAT6Window),
    serial(new QSerialPort(this)),
    console(new Console)
{
    ui->setupUi(this);

    // configure console
    console->setEnabled(false);
    ui->grid_layout_console->addWidget(console);

    // connect signal for when data is ready to be read
    connect(serial, SIGNAL(readReady()), this, SLOT(onReadData()));
}

BAT6Window::~BAT6Window()
{
}

void BAT6Window::openSerialPort()
{
    SettingsDialog::Settings p = settings->settings();
    serial->setPortName(p.name);
    serial->setBaudRate(p.baudRate);
    serial->setDataBits(p.dataBits);
    serial->setParity(p.parity);
    serial->setStopBits(p.stopBits);
    serial->setFlowControl(p.flowControl);
    if (serial->open(QIODevice::ReadWrite))
    {
        console->setEnabled(true);
        console->setLocalEchoEnabled(p.localEchoEnabled);
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        ui->actionConfigure->setEnabled(false);
        showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                          .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                          .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), serial->errorString());

        showStatusMessage(tr("Open error"));
    }
}

void BAT6Window::onReadData()
{
    QByteArray data = serial->readAll();
    console->putData(data);
}
