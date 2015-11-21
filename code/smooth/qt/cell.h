#ifndef CELL_H
#define CELL_H

#include <QScopedPointer>
#include <QWidget>

namespace Ui {
    class Cell;
}

class Cell : public QWidget
{
    Q_OBJECT

public:
    explicit Cell(QWidget *parent = 0);
    ~Cell();

    void allowRemovingFromChain(bool allow);

private:
    QScopedPointer<Ui::Cell> ui;
};

#endif // CELL_H
