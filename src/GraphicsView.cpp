#include "GraphicsView.hpp"
#include <QWheelEvent>

GraphicsView::GraphicsView(QWidget* parent)
    : QGraphicsView(parent)
{
    setRenderHint(QPainter::Antialiasing, false);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setOptimizationFlags(QGraphicsView::DontSavePainterState);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
}

void GraphicsView::wheelEvent(QWheelEvent* event)
{
    if (event->angleDelta().y() > 0)
        scale(1.1, 1.1);
    else
        scale(0.9, 0.9);
}

void GraphicsView::toggleDragMode(bool mode)
{
    if (mode)
        ; //setDragMode(QGraphicsView::RubberBandDrag);
    else
        ; //setDragMode(QGraphicsView::ScrollHandDrag);
}
