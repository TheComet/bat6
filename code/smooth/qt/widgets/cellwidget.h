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
signals:
    void exposureChanged(const CellWidget& cell, double intensity);
    void shortCircuitCurrentChanged(const CellWidget& cell, double shortCircuitCurrent);
    void openCircuitVoltageChanged(const CellWidget& cell, double openCircuitVoltage);
    void darkVoltageChange(const CellWidget& cell, double darkVoltage);

public:
    explicit CellWidget(const QString& cellName, QWidget *parent = 0);
    ~CellWidget();

    void allowRemovingFromChain(bool allow);
    const QString& getName() const;

private slots:
    void on_intensity_valueChanged(int value);

private:
    QScopedPointer<Ui::CellWidget> ui;
    QString cellName;
};

#endif // CELLWIDGET_H
