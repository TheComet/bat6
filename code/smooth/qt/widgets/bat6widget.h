#ifndef BAT6WIDGET_H
#define BAT6WIDGET_H

#include <QWidget>
#include <QScopedPointer>

namespace Ui {
    class BAT6Widget;
}

class QGridLayout;
class ConsoleWidget;
class Cell;

class BAT6Widget : public QWidget
{
    Q_OBJECT

public:
    explicit BAT6Widget(QWidget *parent = 0);
    ~BAT6Widget();

private slots:
    void onReadData();

private:
    void openSerialPort();

    QScopedPointer<Ui::BAT6Widget> ui;
    QGridLayout* layoutCells;
    ConsoleWidget* console;
};

#endif // BAT6WIDGET_H
