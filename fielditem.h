#ifndef FIELDITEM_H
#define FIELDITEM_H

#define MSL_FIELD

#ifdef MSL_FIELD
#define FIELD_WIDTH 900
#define FIELD_HEIGHT 600

#define XLINE1 450
#define XLINE2 XLINE1-38
#define XLINE3 XLINE1-113
#define XLINE4 0
#define XLINE5 -(XLINE3)
#define XLINE6 -(XLINE2)
#define XLINE7 -(XLINE1)

#define YLINE1 300
#define YLINE2 213
#define YLINE3 138
#define YLINE4 -(YLINE3)
#define YLINE5 -(YLINE2)
#define YLINE6 -(YLINE1)

#define CENTER_RADIUS 100
#else
#define FIELD_WIDTH 600
#define FIELD_HEIGHT 400
#endif

#include <QGraphicsItem>

class FieldItem : public QGraphicsItem
{
public:
    FieldItem();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
    QRectF boundingRect() const Q_DECL_OVERRIDE;

private:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
#ifndef MSL_FIELD
    float centerCircleRadius;
    float horizontalLine;
    float verticalLine;
    float arcRadius;
    float arcWidth;
#else
    QVector<QLineF> lines;
    QRectF center_circle;
#endif
};

#endif // FIELDITEM_H
