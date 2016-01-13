#ifndef BAT6WIDGET_H
#define BAT6WIDGET_H

#include <QWidget>
#include <QScopedPointer>

namespace Ui {
    class BAT6Widget;
}

class QFrame;
class ConsoleWidget;
class CellWidget;
class CharacteristicsCurve2DWidget;
class CharacteristicsCurve3DWidget;
class QPushButton;
class PVArray;

class BAT6Widget : public QWidget
{
    Q_OBJECT

public:
    explicit BAT6Widget(QWidget *parent = 0);
    ~BAT6Widget();

    void addCell();
    void removeCell(CellWidget* cell);

private:
    void updateCellNames();
    void updateModelCellNames();
    void updateModel();
    void autoScalePlots();

private slots:
    void onAddCellButtonReleased();
    void onRemoveCellButtonReleased(CellWidget* cell);
    void onOpenCircuitVoltageChanged(CellWidget* cell);
    void onShortCircuitCurrentChanged(CellWidget* cell);
    void onDarkVoltageChanged(CellWidget* cell);
    void onCellExposureChanged(CellWidget* cell);
    void onGlobalExposureChanged(int value);
    void onVoltageMeasured(double voltage);
    void onCurrentMeasured(double current);

private:
    QScopedPointer<Ui::BAT6Widget> ui;

    QSharedPointer<PVArray> m_PVArray;

    // pointers to various widgets we control
    QFrame* m_CellWidgetContainer;
    ConsoleWidget* m_Console;
    CharacteristicsCurve2DWidget* m_cc2d;
    CharacteristicsCurve3DWidget* m_cc3d;
    QPushButton* m_AddCellButton;
};

#endif // BAT6_WIDGET_H
