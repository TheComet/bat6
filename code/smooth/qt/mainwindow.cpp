#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QShortcut>
#include <QMdiSubWindow>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // create keyboard shortcuts
    QShortcut* shortcut;
    shortcut = new QShortcut(QKeySequence("Ctrl+N"), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(on_action_New_BAT6_connection_triggered()));
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_action_New_BAT6_connection_triggered()
{
    BAT6Window* bat6Window = new BAT6Window;
    ui->mdiArea->addSubWindow(bat6Window);
    bat6Window->show();
}
