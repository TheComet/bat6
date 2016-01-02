#ifndef BAT6WIDGET_H
#define BAT6WIDGET_H

#include <QWidget>
#include <QScopedPointer>

namespace Ui {
    class BAT6Widget;
}

class QGridLayout;
class ConsoleWidget;
class CellWidget;
class CharacteristicCurve2DWidget;
class CharacteristicCurve3DWidget;

class BAT6Widget : public QWidget
{
    Q_OBJECT

public:
    explicit BAT6Widget(QWidget *parent = 0);
    ~BAT6Widget();

private slots:
    void onCellExposureChanged(CellWidget* cellWidget, double exposure);
    void onReadData();

private:
    void openSerialPort();

    QScopedPointer<Ui::BAT6Widget> ui;
    QGridLayout* cellLayout;
    ConsoleWidget* console;
    CharacteristicCurve2DWidget* cc2d;
    CharacteristicCurve3DWidget* cc3d;
};

#endif // BAT6WIDGET_H
