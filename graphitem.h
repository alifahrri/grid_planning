#ifndef GRAPHITEM_H
#define GRAPHITEM_H

#include <QGraphicsItem>
#include <boost/thread.hpp>

#include "robotmap.h"

class GraphItem : public QGraphicsItem
{
public:
    GraphItem();
    GraphItem(RobotMap* map);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    QPainterPath shape() const;

private:
    QBrush getBrush(int index);
    QPen getPen(int index);
    void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;

private:
    RobotMap* rmap;
};

#endif // GRAPHITEM_H
