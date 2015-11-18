#ifndef BAT6WINDOW_H
#define BAT6WINDOW_H

#include <QWidget>
#include <QScopedPointer>

namespace Ui {
class BAT6Window;
}

class BAT6Window : public QWidget
{
    Q_OBJECT

public:
    explicit BAT6Window(QWidget *parent = 0);
    ~BAT6Window();

private:
    QScopedPointer<Ui::BAT6Window> ui;
};

#endif // BAT6WINDOW_H
