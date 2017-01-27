#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>

#include <QTimer>

#include "robotmap.h"

#include "fielditem.h"
#include "graphitem.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(RobotMap* map, QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *scene;
    FieldItem *fieldItem;
    GraphItem *graphItem;
    QTimer *timer;
    RobotMap *rmap;

private slots:
    void setHWeight();
    void setDelay();
    void setGuiRate(int rate);
    void setMapRate(int rate);
    void updateMap();
    void u();
};

#endif // MAINWINDOW_H
