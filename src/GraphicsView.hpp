#ifndef GRAPHICSVIEW_HPP
#define GRAPHICSVIEW_HPP

#include <QGraphicsView>

class GraphicsView : public QGraphicsView {
    Q_OBJECT
public:
    explicit GraphicsView(QWidget* parent = nullptr);

signals:

public slots:
    void toggleDragMode(bool mode);

protected Q_SLOTS:
    void wheelEvent(QWheelEvent* event);
};

#endif // GRAPHICSVIEW_HPP
