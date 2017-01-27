#include "fielditem.h"

#include <QPainter>
#include <QDebug>

FieldItem::FieldItem()
{
#ifndef MSL_FIELD
    centerCircleRadius = 1000;
    horizontalLine = 6000;
    verticalLine = 4000;
    arcRadius = 500;
    arcWidth = 350;
#else
    lines.push_back(QLineF(QPointF(XLINE1,YLINE1),
                           QPointF(XLINE7,YLINE1)));
    lines.push_back(QLineF(QPointF(XLINE1,YLINE6),
                           QPointF(XLINE7,YLINE6)));
    lines.push_back(QLineF(QPointF(XLINE1,YLINE1),
                           QPointF(XLINE1,YLINE6)));
    lines.push_back(QLineF(QPointF(XLINE7,YLINE1),
                           QPointF(XLINE7,YLINE6)));
    lines.push_back(QLineF(QPointF(XLINE4,YLINE1),
                           QPointF(XLINE4,YLINE6)));
    lines.push_back(QLineF(QPointF(XLINE2,YLINE3),
                           QPointF(XLINE2,YLINE4)));
    lines.push_back(QLineF(QPointF(XLINE6,YLINE3),
                           QPointF(XLINE6,YLINE4)));
    lines.push_back(QLineF(QPointF(XLINE3,YLINE2),
                           QPointF(XLINE3,YLINE5)));
    lines.push_back(QLineF(QPointF(XLINE5,YLINE2),
                           QPointF(XLINE5,YLINE5)));
    lines.push_back(QLineF(QPointF(XLINE2,YLINE3),
                           QPointF(XLINE1,YLINE3)));
    lines.push_back(QLineF(QPointF(XLINE2,YLINE4),
                           QPointF(XLINE1,YLINE4)));
    lines.push_back(QLineF(QPointF(XLINE3,YLINE2),
                           QPointF(XLINE1,YLINE2)));
    lines.push_back(QLineF(QPointF(XLINE3,YLINE5),
                           QPointF(XLINE1,YLINE5)));

    lines.push_back(QLineF(QPointF(XLINE6,YLINE3),
                           QPointF(XLINE7,YLINE3)));
    lines.push_back(QLineF(QPointF(XLINE6,YLINE4),
                           QPointF(XLINE7,YLINE4)));
    lines.push_back(QLineF(QPointF(XLINE5,YLINE2),
                           QPointF(XLINE7,YLINE2)));
    lines.push_back(QLineF(QPointF(XLINE5,YLINE5),
                           QPointF(XLINE7,YLINE5)));

    center_circle.setLeft(-CENTER_RADIUS);
    center_circle.setTop(-CENTER_RADIUS);
    center_circle.setHeight(2*CENTER_RADIUS);
    center_circle.setWidth(2*CENTER_RADIUS);

#endif
}

void FieldItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
#ifndef MSL_FIELD
    painter->setBrush(QColor(0,200,0));
    painter->setPen(QPen(QBrush(Qt::white),2));
    painter->drawRect(-horizontalLine/20,-verticalLine/20,horizontalLine/10,verticalLine/10);
    painter->drawEllipse(-centerCircleRadius/20,-centerCircleRadius/20,centerCircleRadius/10,centerCircleRadius/10);
    painter->setPen(QPen(QBrush(Qt::white),1));
    painter->drawLine(0,-verticalLine/20,0,verticalLine/20);
    painter->drawArc(-(horizontalLine/2+arcRadius)/10,-(arcRadius+arcWidth/2)/10,arcRadius/5,arcRadius/5,0,16*90);
    painter->drawArc(-(horizontalLine/2+arcRadius)/10,(-arcRadius+arcWidth/2)/10,arcRadius/5,arcRadius/5,0,-16*90);
    painter->drawLine((-horizontalLine/2+arcRadius)/10,-arcWidth/20,(-horizontalLine/2+arcRadius)/10,arcWidth/20);
    painter->drawArc((horizontalLine/2-arcRadius)/10,-(arcRadius+arcWidth/2)/10,arcRadius/5,arcRadius/5,16*90,16*90);
    painter->drawArc((horizontalLine/2-arcRadius)/10,(-arcRadius+arcWidth/2)/10,arcRadius/5,arcRadius/5,-16*90,-16*90);
    painter->drawLine((horizontalLine/2-arcRadius)/10,(-arcWidth/20),(horizontalLine/2-arcRadius)/10,(arcWidth/20));
#else
    painter->setPen(Qt::transparent);
    painter->setBrush(Qt::green);
    painter->drawRect(boundingRect());
    painter->setPen(QPen(Qt::white,3.3));
    painter->setBrush(Qt::transparent);
    painter->drawLines(lines);
    painter->drawEllipse(center_circle);
#endif
}

QRectF FieldItem::boundingRect() const {
#ifndef MSL_FIELD
    QRectF rect(-horizontalLine/20,-verticalLine/20,horizontalLine/10,verticalLine/10);
    return rect;
#else
    return QRectF(XLINE7-100,YLINE6-100,FIELD_WIDTH+200,FIELD_HEIGHT+200);
#endif
}

void FieldItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

}

void FieldItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

}

void FieldItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

}
