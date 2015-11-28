#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScopedPointer>
#include "bat6widget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_action_New_BAT6_connection_triggered();

private:
    QScopedPointer<Ui::MainWindow> ui;
    QScopedPointer<BAT6Widget> bat6Widget;
};

#endif // MAINWINDOW_H
