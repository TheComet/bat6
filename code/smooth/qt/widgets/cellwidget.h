#ifndef CELLWIDGET_H
#define CELLWIDGET_H

#include <QScopedPointer>
#include <QWidget>

namespace Ui {
    class CellWidget;
}

class CellWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CellWidget(QWidget *parent = 0);
    ~CellWidget();

    void allowRemovingFromChain(bool allow);

private:
    QScopedPointer<Ui::CellWidget> ui;
};

#endif // CELLWIDGET_H
