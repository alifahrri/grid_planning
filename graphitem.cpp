#include "graphitem.h"
#include "graphsearch.h"

#include <QPainter>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>

//#define DEBUG_GRAPHITEM

inline QBrush GraphItem::getBrush(int index)
{
    if(index == rmap->graphSearch()->startIndex())
        return(QBrush(Qt::red));
    else if(index == rmap->graphSearch()->goalIndex())
        return(QBrush(Qt::blue));
    switch((*(rmap->graph()))[index].state) {
    case Unvisited:
        return QBrush(QColor(0,0,255,127));
    case Dead:
        return QBrush(QColor(127,127,127,255));
        //        return QBrush(QColor(127,127,127,127));
    case Alive:
        return QBrush(QColor(127,255,127,127));
    case Solution:
        return QBrush(QColor(255,255,255,255));
        //		return QBrush(QColor(255,255,255,127));
    case Obstacle:
        return QBrush(QColor(0,0,0,255));
    default:
        return QBrush(Qt::white);
    }
}

inline QPen GraphItem::getPen(int index)
{
    if(index == rmap->graphSearch()->startIndex())
        return(QPen(Qt::red));
    else if(index == rmap->graphSearch()->goalIndex())
        return(QPen(Qt::blue));

    switch((*(rmap->graph()))[index].state) {
    case Unvisited:
        return QPen(QColor(0,0,255,127));
    case Dead:
        return QPen(QColor(127,127,127,255));
        //        return QBrush(QColor(127,127,127,127));
    case Alive:
        return QPen(QColor(127,255,127,127));
    case Solution:
        return QPen(QColor(255,255,255,255));
        //		return QBrush(QColor(255,255,255,127));
    case Obstacle:
        return QPen(QColor(0,0,0,255));
    default:
        return QPen(Qt::white);
    }
}

GraphItem::GraphItem()
{

}

GraphItem::GraphItem(RobotMap *map)
    : rmap(map)
{

}

void GraphItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    int nVertices = boost::num_vertices(*(rmap->graph()));
#ifdef DEBUG_GRAPHITEM
    std::cout << "updating graph item \nnumber of vertices : " << nVertices << '\n';
#endif
    QRectF r(-(rmap->column()*rmap->gridSize())/2,(rmap->row()*rmap->gridSize())/2,
             rmap->gridSize(),rmap->gridSize());
//    QPointF pt(-rmap->column()*rmap->gridSize()/2,(rmap->row()*rmap->gridSize()/2));
    QPointF pt;

    QVector<QRectF> unvisited_rects, solution_rects, dead_rects, alive_rects, obstacle_rects;
    QVector<QPointF> unvisited_pts, solution_pts, dead_pts, alive_pts, obstacle_pts;
    for(int i=0; i<nVertices; i++)
    {
        std::pair<double,double> p = rmap->vertexPosAt(i);
        QRectF rect = r.translated(p.first,-p.second);
//        QPointF ptf = pt + QPointF(p.first,-p.second);
        switch((*rmap->graph())[i].state) {
        case Unvisited:
            unvisited_rects.append(rect);
//            unvisited_pts.append(ptf);
            break;
        case Dead:
            dead_rects.append(rect);
//            dead_pts.append(ptf);
            break;
        case Alive:
            alive_rects.append(rect);
//            alive_pts.append(ptf);
            break;
        case Solution:
            solution_rects.append(rect);
//            solution_pts.append(ptf);
            break;
        case Obstacle:
            obstacle_rects.append(rect);
//            obstacle_pts.append(ptf);
            break;
        default:
            if((i == rmap->graphSearch()->startIndex()) || i == rmap->graphSearch()->goalIndex())
            {
                solution_rects.append(rect);
//                solution_pts.append(ptf);
            }
            break;
        }
    }
    painter->setPen(Qt::darkGreen);
    auto unvisited_color = QColor(Qt::green);
    unvisited_color.setAlphaF(0.3);
    painter->setBrush(QBrush(unvisited_color));
    painter->drawRects(unvisited_rects);
    painter->setBrush(QBrush(QColor(127,127,127,127)));
    painter->drawRects(dead_rects);
    painter->setBrush(QBrush(QColor(127,255,127,127)));
    painter->drawRects(alive_rects);
    painter->setBrush(QBrush(QColor(255,255,255,255)));
    painter->drawRects(solution_rects);
    painter->setBrush(QBrush(QColor(0,0,0,255)));
    painter->setPen(Qt::white);
    painter->drawRects(obstacle_rects);
    auto start = rmap->vertexPosAt(rmap->graphSearch()->startIndex());
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::red);
    painter->drawRect(r.translated(start.first,-start.second));
    painter->setBrush(Qt::blue);
    auto goal = rmap->vertexPosAt(rmap->graphSearch()->goalIndex());
    painter->drawRect(r.translated(goal.first,-goal.second));
    painter->setPen(Qt::black);
    auto sol = rmap->getSolution();
    if(sol.size()>1) {
        auto s0 = sol[0];
        for(auto s : sol){
            painter->drawLine(QPointF(s0.first-rmap->gridSize()/2,-s0.second),QPointF(s.first-rmap->gridSize()/2,-s.second));
            s0 = s;
        }
    }
}

void GraphItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    switch(event->button()) {
    case Qt::LeftButton : {
        if(event->modifiers() == Qt::ControlModifier) {
            rmap->clearSquareObstacle(event->pos().x(),-event->pos().y(),5*rmap->gridSize());
        }
        else if(event->modifiers() == Qt::ShiftModifier) {
//            rmap->addSquareObstacle(event->pos().x(),-event->pos().y(),5*rmap->gridSize());
            rmap->addCircleObstacle(event->pos().x(),-event->pos().y(),5*rmap->gridSize());
        }
        else if(event->modifiers() == Qt::NoModifier) {
            rmap->setStart(event->pos().x(),-event->pos().y());
        }
        break;
    }
    case Qt::RightButton : {
        if(event->modifiers() == Qt::ControlModifier) {

        }
        else if(event->modifiers() == Qt::ShiftModifier) {

        }
        else if(event->modifiers() == Qt::NoModifier) {
            rmap->setGoal(event->pos().x(),-event->pos().y());
        }
        break;
    }
    default : break;
    }
}

void GraphItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

}

void GraphItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

}

QRectF GraphItem::boundingRect() const
{
    return QRectF(-(rmap->width())/2,-rmap->height()/2,rmap->width(),rmap->height());
}

QPainterPath GraphItem::shape() const
{
    QPainterPath p;
    p.addRect(boundingRect());
    return p;
}
