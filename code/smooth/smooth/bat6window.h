#ifndef BAT6WINDOW_H
#define BAT6WINDOW_H

#include <QWidget>
#include <QScopedPointer>

namespace Ui {
    class BAT6Window;
}

class QSerialPort;
class Console;

class BAT6Window : public QWidget
{
    Q_OBJECT

public:
    explicit BAT6Window(QWidget *parent = 0);
    ~BAT6Window();

private slots:
    void onReadData();

private:
    QScopedPointer<Ui::BAT6Window> ui;
    QScopedPointer<QSerialPort> serial;
    QScopedPointer<Console> console;
};

#endif // BAT6WINDOW_H
