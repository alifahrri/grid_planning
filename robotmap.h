#ifndef ROBOTMAP_H
#define ROBOTMAP_H

#include "breadthfirstsearch.h"
#include "astarsearch.h"
#include "thetastarsearch.h"
#include <boost/thread.hpp>
#include <boost/chrono.hpp>

//#define BRESENHAM_TEST

typedef boost::mutex Mutex;

class RobotMap
{
public:
    RobotMap();
    RobotMap(double w_, double h_, double gs, char* ar);
    ~RobotMap(){ }
    void loop();
    void setStart(double x, double y);
    void setGoal(double x, double y);
    void addObstacle(double x, double y);
    void addObstacle(std::pair<double,double> o);
    void addCircleObstacle(double x, double y, double r);
    void addSquareObstacle(double x, double y, double s);
    void clearObstacle(double x, double y);
    void clearObstacle(std::pair<double,double> o);
    void clearSquareObstacle(double x, double y, double s);
    void setObstacle(std::set<std::pair<double,double> > o);
    void stop();
    void update() { force_update = true; }
    double lastSolveTime();
    double width() { return w; }
    double height() { return h; }
    double gridSize() { return grid_size; }
    int row() { return row_count; }
    int column() {return col_count; }
    void setLoopRate(int rate_ms);
    Mutex* mutex() { return &m; }
    const Graph* graph();
    std::pair<double,double> startPos();
    std::pair<double,double> goalPos();
    GraphSearch* graphSearch() { return graph_search; }
    std::pair<double,double> vertexPosAt(int idx);
    std::vector<std::pair<double,double> >getSolution();

private:
    std::pair<int,int> toInt(std::pair<double,double> p);
    std::pair<int,int> toInt(double x, double y);
    std::pair<int,int> toInt(int idx);
    std::pair<double,double> toDouble(std::pair<double,double> p);
    std::pair<double,double> toDouble(double x, double y);

private:
    bool running;
    bool force_update;
    double w, h, grid_size;
    double last_update_time;
    int row_count, col_count;
    std::tuple<int,double> graph_result;
    boost::chrono::milliseconds loop_rate;
    Mutex m;
    Mutex lm;
    GraphSearch *graph_search;

#ifdef BRESENHAM_TEST
public:
    std::vector<std::pair<double,double> > bresenhamLine(std::pair<double,double> p0, std::pair<double,double> p1);
    std::vector<std::pair<double,double>> circle(std::pair<double,double> p0, double radius);
    std::vector<std::pair<double,double>> filledCircle(std::pair<double,double> p0, double radius);
#endif
};

#endif // ROBOTMAP_H
