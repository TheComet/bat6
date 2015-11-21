#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include <QtGlobal>
#include <QGraphicsView>

class GraphWidget : public QGraphicsView
{
    Q_OBJECT

public:
    GraphWidget(QWidget* parent = 0);

    void itemMoved();

public slots:
    void shuffle();
    void zoomIn();
    void zoomOut();

protected:
    virtual void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
    virtual void timerEvent(QTimerEvent* event) Q_DECL_OVERRIDE;
#ifndef QT_NO_WHEELEVENT
    virtual void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;
#endif
    virtual void drawBackground(QPainter* painter, const QRectF& rect) Q_DECL_OVERRIDE;

    virtual void scaleView(qreal scaleFactor) Q_DECL_OVERRIDE;

private:
    int timerId;
    Node* centerNode;
};

#endif // GRAPHVIEW_H
