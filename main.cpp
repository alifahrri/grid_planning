#include "mainwindow.h"

#include <QApplication>
#include <iostream>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <boost/thread/scoped_thread.hpp>

#include "breadthfirstsearch.h"
#include "astarsearch.h"
#include "robotmap.h"

#include <sys/socket.h>
#include <arpa/inet.h>

//#define FIELD_WIDTH 600
//#define FIELD_HEIGHT 400
#define GRID_SIZE 40

boost::mutex mutex;
bool running = false;

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  RobotMap *m1;
  if(argc == 3){
    m1 = new RobotMap(FIELD_WIDTH,FIELD_HEIGHT,atoi(argv[1]),argv[2]);
  }
  else
    m1 = new RobotMap(FIELD_WIDTH,FIELD_HEIGHT,GRID_SIZE,"astar");
  m1->graphSearch()->setDelay(false,20);
  boost::thread p(&RobotMap::loop,m1);
  mutex.lock();
  std::cout << "thread " << boost::this_thread::get_id() << " instanting main window..\n";
  mutex.unlock();
  MainWindow w(m1);
  mutex.lock();
  std::cout << "thread " << boost::this_thread::get_id() << " done..\n";
  mutex.unlock();
  w.showMaximized();

  a.exec();
  m1->stop();
  p.join();
  //    delete m1;
  return 0;
}
