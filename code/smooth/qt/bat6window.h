#ifndef BAT6WINDOW_H
#define BAT6WINDOW_H

#include <QWidget>
#include <QScopedPointer>
#include <QButtonGroup>

namespace Ui {
    class BAT6Window;
}

class Console;
class Cell;

class BAT6Window : public QWidget
{
    Q_OBJECT

public:
    explicit BAT6Window(QWidget *parent = 0);
    ~BAT6Window();

private slots:
    void onReadData();

private:
    void openSerialPort();

    QScopedPointer<Ui::BAT6Window> ui;
    Console* console;
};

#endif // BAT6WINDOW_H
