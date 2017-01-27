#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(RobotMap *map, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    rmap(map)
{
    ui->setupUi(this);
    setCentralWidget(ui->graphicsView);

    fieldItem = new FieldItem();
    graphItem = new GraphItem(map);

    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    scene->addItem(fieldItem);
    scene->addItem(graphItem);
    auto background_color = QColor(Qt::darkGreen);
//    background_color.setBlue(200);
    scene->setBackgroundBrush(background_color);

    timer = new QTimer(this);
//    connect(timer,SIGNAL(timeout()),this->ui->graphicsView,SLOT(update()));
    connect(timer,SIGNAL(timeout()),this->scene,SLOT(update()));
    connect(ui->hw_spinbox,SIGNAL(valueChanged(double)),this,SLOT(setHWeight()));
    connect(ui->checkBox,SIGNAL(toggled(bool)),this,SLOT(setDelay()));
    connect(ui->spinBox,SIGNAL(valueChanged(int)),this,SLOT(setDelay()));
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(updateMap()));
    connect(ui->spinBox_2,SIGNAL(valueChanged(int)),this,SLOT(setMapRate(int)));
    connect(ui->spinBox_3,SIGNAL(valueChanged(int)),this,SLOT(setGuiRate(int)));

    timer->setSingleShot(false);
    timer->start(33);

    ui->graphicsView->setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing);

    ui->spinBox_2->setValue(round(1000.0/30.0));
    ui->spinBox_3->setValue(round(1000.0/33.0));
    setWindowTitle("Fukuro Path Planning");
}

void MainWindow::setHWeight()
{
    std::string s = rmap->graphSearch()->string(0,2);
    if(s == std::string("a-star")) {
        AStarSearch* astar = dynamic_cast<AStarSearch*>(rmap->graphSearch());
        astar->setHeuristicWeight(ui->hw_spinbox->value());
    }
    else if(s == std::string("theta-star")) {
        ThetaStarSearch* thetastar = dynamic_cast<ThetaStarSearch*>(rmap->graphSearch());
        thetastar->setHeuristicWeight(ui->hw_spinbox->value());
    }
}

void MainWindow::setDelay()
{

}

void MainWindow::setGuiRate(int rate)
{
    timer->setInterval(round(1000.0/rate));
}

void MainWindow::setMapRate(int rate)
{
    rmap->setLoopRate(round(1000.0/rate));
}

void MainWindow::updateMap()
{
    rmap->update();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::u()
{

}
