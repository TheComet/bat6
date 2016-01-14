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
    void exposureChanged(CellWidget* cell, double intensity);
    void shortCircuitCurrentChanged(CellWidget* cell, double shortCircuitCurrent);
    void openCircuitVoltageChanged(CellWidget* cell, double openCircuitVoltage);
    void darkVoltageChange(CellWidget* cell, double darkVoltage);

public:
    explicit CellWidget(const QString& m_CellName, QWidget *parent = 0);
    ~CellWidget();

    void allowRemovingFromChain(bool allow);
    void setName(const QString& name);
    const QString& getName() const;

    QScopedPointer<Ui::CellWidget> ui;

private slots:
    void on_intensity_valueChanged(int value);

private:
    QString m_CellName;
};

#endif // CELLWIDGET_H
