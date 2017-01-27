#include "robotmap.h"
#include <QString>

typedef boost::chrono::high_resolution_clock Clock;
typedef boost::chrono::milliseconds Miliseconds;
typedef Clock::time_point TimePoint;
typedef Clock::duration Duration;

#define DEBUG

inline std::pair<int,int> RobotMap::toInt(std::pair<double, double> p)
{
  std::pair<int,int> ret;
  ret.second = round((p.first+w/2)/grid_size);
  ret.first = round((p.second+h/2)/grid_size);
  return ret;
}

inline std::pair<int,int> RobotMap::toInt(double x, double y)
{
  return toInt(std::make_pair(x,y));
}

inline std::pair<int,int> RobotMap::toInt(int idx)
{
  std::pair<int,int> ret;
  ret.first = idx % col_count;
  ret.second = idx / col_count + 1;
  return ret;
}

inline std::pair<double,double> RobotMap::toDouble(std::pair<double, double> p)
{
  std::pair<double,double> ret;
  ret.first = p.second*grid_size-w/2;
  ret.second = p.first*grid_size-h/2;
  return ret;
}

inline std::pair<double,double> RobotMap::toDouble(double x, double y)
{
  return toDouble(std::make_pair(x,y));
}

std::pair<double,double> RobotMap::vertexPosAt(int idx)
{
  std::pair<double,double> ret;
  std::pair<int,int> i_pair = toInt(idx);
  ret.first = i_pair.first*grid_size;// - w/2;
  ret.second = i_pair.second*grid_size;// - w/2;
  return ret;
}

RobotMap::RobotMap()
{

}

RobotMap::RobotMap(double w_, double h_, double gs, char* ar)
  : w(w_), h(h_), grid_size(gs), running(false), force_update(false)
{
  int gcwl = ceil((w/2)/grid_size);
  int gcwr = gcwl;
  col_count = gcwl*2+1;
  int gchl = ceil((h/2)/grid_size);
  int gchr = gchl;
  row_count = gchl*2+1;
  std::cout << "column : " << col_count << " row : " << row_count << '\n';
  if(QString(ar)==QString("bfs"))
    graph_search = new BreadthFirstSearch(col_count,row_count);
  else if(QString(ar)==QString("astar"))
    graph_search = new AStarSearch(col_count,row_count);
  else if(QString(ar)==QString("thetastar"))
    graph_search = new ThetaStarSearch(col_count,row_count);
  std::cout << "instanting graph search : " << col_count << "," << row_count << '\n';
  loop_rate = Miliseconds(20);
}

void RobotMap::loop()
{
  running = true;
  TimePoint t;
  Duration d;
  Miliseconds dt;
  Miliseconds rate = Miliseconds(20);
  while(running) {
    try {
      t = Clock::now();
      d = t.time_since_epoch();
      Miliseconds start_ms = boost::chrono::duration_cast<Miliseconds>(d);
      m.lock();
      if(force_update) {
        graph_result = graph_search->update(true);
        force_update = false;
      }
      else
        graph_result = graph_search->update();
      m.unlock();
      t = Clock::now();
      d = t.time_since_epoch();
      Miliseconds end_ms = boost::chrono::duration_cast<Miliseconds>(d);
      dt = end_ms-start_ms;
      if(std::get<0>(graph_result)==0 || std::get<0>(graph_result)==1)
        std::cout << std::get<0>(graph_result) << " update time : " << std::get<1>(graph_result) << '\n';
      //            lm.lock();
      if(dt < loop_rate)
        boost::this_thread::sleep_for(loop_rate-dt);
      //            lm.unlock();
    }
    catch(boost::thread_interrupted&) {
#ifdef DEBUG
      boost::lock_guard<Mutex> lock(m);
      std::cout << "thread interrupted..\n";
#endif
    }
  }
}

void RobotMap::setLoopRate(int rate_ms)
{
  //    lm.lock();
  loop_rate = Miliseconds(rate_ms);
  //    lm.unlock();
}

void RobotMap::setStart(double x, double y)
{
  std::cout << "[RobotMap] start : " << x << ", " << y << '\n';
  std::pair<int,int> p = toInt(x,y);
  m.lock();
  graph_search->setStart(p.first,p.second);
  m.unlock();
}

void RobotMap::setGoal(double x, double y)
{
  std::pair<int,int> p = toInt(x,y);
  m.lock();
  graph_search->setGoal(p.first,p.second);
  m.unlock();
}

void RobotMap::setObstacle(std::set<std::pair<double, double> > o)
{
  std::set<std::pair<int,int> > oi;
  for(std::pair<double,double> i : o)
    oi.insert(toInt(i));
  m.lock();
  graph_search->setObstacles(oi);
  m.unlock();
}

void RobotMap::addObstacle(std::pair<double,double> o)
{
  auto i_pair = toInt(o);
  std::cout << "RobotMap addObstacle : " << i_pair.first << "," << i_pair.second << '\n';
  m.lock();
  graph_search->addObstacle(i_pair);
  m.unlock();
}

void RobotMap::addObstacle(double x, double y)
{
  auto i_pair = toInt(x,y);
  m.lock();
  graph_search->addObstacle(i_pair);
  m.unlock();
}

void RobotMap::addCircleObstacle(double x, double y, double r)
{
  auto center = toInt(x,y);
  m.lock();
  graph_search->addObstacles(center,(int)(r/grid_size));
  m.unlock();
}

void RobotMap::addSquareObstacle(double x, double y, double s)
{
  int c = round(s/grid_size);
  double x_=x-s/2.0, y_=y-s/2.0;
  std::set<std::pair<int,int> > olist;
  for(int i=0; i<c; i++) {
    y_+=grid_size;
    if(fabs(y_)>h)
      y_ = y_<0?-h:h;
    x_=x-s/2.0;
    for(int j=0; j<c; j++) {
      x_+=grid_size;
      if(fabs(x_)>w)
        x_ = x_<0?-w:w;
      //            auto p = toInt(x_,y);
      //            std::cout << "square obstacle : " << p.first << "," << p.second << '\n';
      if(fabs(x_)<=w/2.0&&fabs(y_)<=h/2.0)
        olist.insert(toInt(x_,y_));
    }
  }
  m.lock();
  graph_search->addObstacles(olist);
  m.unlock();
}

void RobotMap::clearObstacle(double x, double y){
  auto i_pair = toInt(x,y);
  m.lock();
  graph_search->clearObstacle(i_pair);
  m.unlock();
}

void RobotMap::clearObstacle(std::pair<double, double> o)
{
  auto i_pair = toInt(o);
  m.lock();
  graph_search->clearObstacle(i_pair);
  m.unlock();
}

void RobotMap::clearSquareObstacle(double x, double y, double s)
{
  int c = round(s/grid_size);
  double x_=x-s/2.0, y_=y-s/2.0;
  std::set<std::pair<int,int> > olist;
  for(int i=0; i<c; i++) {
    y_+=grid_size;
    if(fabs(y_)>h)
      y_ = y_<0?-h:h;
    x_=x-s/2.0;
    for(int j=0; j<c; j++) {
      x_+=grid_size;
      if(fabs(x_)>w)
        x_ = x_<0?-w:w;
      auto p = toInt(x_,y);
      std::cout << "square obstacle : " << p.first << "," << p.second << '\n';
      olist.insert(toInt(x_,y_));
    }
  }
  m.lock();
  graph_search->clearObstacles(olist);
  m.unlock();
}

void RobotMap::stop()
{
  running = false;
}

const Graph* RobotMap::graph()
{
  return graph_search->graph();
}

double RobotMap::lastSolveTime()
{
  return std::get<1>(graph_result);
}

std::pair<double,double> RobotMap::startPos()
{
  return toDouble(graph_search->getStart());
}

std::pair<double,double> RobotMap::goalPos()
{
  return toDouble(graph_search->getGoal());
}

std::vector<std::pair<double,double> > RobotMap::getSolution()
{
  std::vector<std::pair<double,double> > ret;
  if(graph_search->solved()) {
    auto sol = graph_search->getSolutions();
    for(auto p : sol)
      ret.push_back(toDouble(p));
  }
  return ret;
}

#ifdef BRESENHAM_TEST

std::vector<std::pair<double, double> > RobotMap::bresenhamLine(std::pair<double, double> p0, std::pair<double, double> p1)
{
  std::vector<std::pair<double,double> > ret;
  auto pts = graph_search->bresenhamLines(toInt(p0),toInt(p1));
  for(auto p : pts)
    ret.push_back(toDouble(p));
  return ret;
}

std::vector<std::pair<double, double> > RobotMap::circle(std::pair<double, double> p0, double radius)
{
  std::vector<std::pair<double,double>> ret;
  auto pts = graph_search->circle(toInt(p0),(int)(radius/grid_size));
  for(const auto& p : pts)
    ret.push_back(toDouble(p));
  return ret;
}

std::vector<std::pair<double, double> > RobotMap::filledCircle(std::pair<double, double> p0, double radius)
{
  std::vector<std::pair<double,double>> ret;
  auto pts = graph_search->filledCircle(toInt(p0),(int)(radius/grid_size));
  for(const auto& p : pts)
    ret.push_back(toDouble(p));
  return ret;
}

#endif
