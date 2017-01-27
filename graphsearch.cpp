#include "graphsearch.h"

#define DEFAULT_SIZE_X 10
#define DEFAULT_SIZE_Y 10

inline
int GraphSearch::to_single_index(std::pair<int,int> p)
{
  return p.first*column_count+p.second;
}

inline
bool GraphSearch::is_valid(std::pair<int, int> p)
{
  auto i = to_single_index(p);
  if((i<vertex_count) &&
     (i>0))
    return true;
  else
    return false;
}

inline
int GraphSearch::to_single_index(int x_, int y_)
{
  return x_*column_count+y_;
}

std::pair<int,int> GraphSearch::to_double_index(int idx)
{
  int x = idx/column_count;
  int y = idx%column_count;
  return std::make_pair(x,y);
}

static inline bool is_leftmost(int x_, int y_, int row_, int column_)
{
  return y_ == 0 ? true : false;
}

static inline bool is_rightmost(int x_, int y_, int row_, int column_)
{
  return y_ == (column_-1) ? true : false;
}

static inline bool is_uppermost(int x_, int y_, int row_, int column_)
{
  return x_ == 0 ? true : false;
}

static inline bool is_bottommost(int x_, int y_, int row_, int column_)
{
  return x_ == (row_-1) ? true : false;
}


GraphSearch::GraphSearch() :
  s(false),
  need_update(true)
{
  makeGrid(DEFAULT_SIZE_X,DEFAULT_SIZE_Y);
#ifdef USE_DELAY
  en_delay = false;
  delay_ms = 0;
#endif
}

GraphSearch::GraphSearch(int x, int y) :
  s(false),
  need_update(true)
{
  makeGrid(x,y);
#ifdef USE_DELAY
  en_delay = false;
  delay_ms = 0;
#endif
}

void GraphSearch::setObstacles(std::set<int> ob)
{
  need_update = true;
  obstacles = ob;
  for(int i : ob)
  {
    if(i>0 && i<vertex_count)
    {
      (*g)[i].state = Obstacle;
      obstacles.insert(i);
    }
  }
}

void GraphSearch::setObstacles(std::set<std::pair<int,int> > ob)
{
  need_update = true;
  obstacles.clear();
  for(std::pair<int,int> i : ob){
    int idx = to_single_index(i);
    if(idx>0 && idx<vertex_count)
    {
      (*g)[idx].state = Obstacle;
      obstacles.insert(to_single_index(i));
    }
  }
}

void GraphSearch::addObstacle(int ob)
{
  if((ob>=0) && (ob<vertex_count))
  {
    need_update = true;
    obstacles.insert(ob);
    (*g)[ob].state = Obstacle;
  }
}

void GraphSearch::addObstacles(std::set<int> ob)
{
  need_update = true;
  obstacles.insert(ob.begin(),ob.end());
  for(int i : ob)
    (*g)[i].state = Obstacle;
}

void GraphSearch::addObstacles(std::set<std::pair<int, int> > ob)
{
  need_update = true;
  for(std::pair<int,int> i : ob){
    int idx = to_single_index(i);
    if((idx<vertex_count) && (idx>=0))
    {
      (*g)[idx].state = Obstacle;
      obstacles.insert(idx);
    }
  }
}

void GraphSearch::addObstacles(std::pair<int, int> center, int radius)
{
  auto circle = filledCircle(center,radius);
  for(auto& c : circle)
    addObstacle(c);
}

void GraphSearch::addObstacle(std::pair<int, int> ob)
{
  int idx = to_single_index(ob);
  if((idx>=0)&&(idx<vertex_count))
  {
    need_update = true;
    (*g)[idx].state = Obstacle;
    obstacles.insert(idx);
  }
}

void GraphSearch::clearObstacle(int ob)
{
  need_update = true;
  if(obstacles.erase(ob))
    (*g)[ob].state = Unvisited;
}

void GraphSearch::clearObstacle(std::pair<int, int> ob)
{
  need_update = true;
  int idx = to_single_index(ob);
  if(obstacles.erase(idx))
    (*g)[idx].state = Unvisited;
}

void GraphSearch::clearObstacles(std::set<int> ob)
{
  need_update = true;
  for(int i: ob)
    if(obstacles.erase(i))
      (*g)[i].state = Unvisited;
}

void GraphSearch::clearObstacles(std::set<std::pair<int, int> > ob)
{
  need_update = true;
  for(std::pair<int,int> p : ob) {
    int i = to_single_index(p);
    if(obstacles.erase(i))
      (*g)[i].state = Unvisited;
  }
}

std::set<int> GraphSearch::getObstacles()
{
  return obstacles;
}

void GraphSearch::makeGrid(int x, int y)
{
  vertex_count = x*y;
  row_count = y;
  column_count = x;
#ifdef DEBUG
  std::cout << "creating grid..\n";
  std::cout << "row : " << row_count << " column : " << column_count << '\n';
  std::cout << "vertex count : " << vertex_count << '\n';
#endif
  std::set<Edge> edge_vector;
  for(int i=0; i<row_count; ++i) {
    for(int j=0; j<column_count; ++j){
      if(!is_leftmost(i,j,row_count,column_count)) {
#ifdef DEBUG
        std::cout << i << "," << j << "(" << to_single_index(i,j) << ") is not leftmost, connecting to " <<
                     i << "," << j-1 << " (" << to_single_index(i,j-1) << ")\n";
#endif
        edge_vector.insert(Edge(to_single_index(i,j-1),to_single_index(i,j)));
      }

      if(!is_rightmost(i,j,row_count,column_count)) {
#ifdef DEBUG
        std::cout << i << "," << j << "(" << to_single_index(i,j) << ") is not rightmost, connecting to " <<
                     i << "," << j+1 << " (" << to_single_index(i,j+1) << ")\n";
#endif
        edge_vector.insert(Edge(to_single_index(i,j),to_single_index(i,j+1)));
      }

      if(!is_uppermost(i,j,row_count,column_count)) {
#ifdef DEBUG
        std::cout << i << "," << j << "(" << to_single_index(i,j) << ") is not uppermost, connecting to " <<
                     i-1 << "," << j << " (" << to_single_index(i-1,j) << ")\n";
#endif
        edge_vector.insert(Edge(to_single_index(i-1,j),to_single_index(i,j)));
      }

      if(!is_bottommost(i,j,row_count,column_count)) {
#ifdef DEBUG
        std::cout << i << "," << j << "(" << to_single_index(i,j) << ") is not bottommost, connecting to " <<
                     i+1 << "," << j << " (" << to_single_index(i+1,j) << ")\n";
#endif
        edge_vector.insert(Edge(to_single_index(i,j),to_single_index(i+1,j)));
      }

      if(!is_leftmost(i,j,row_count,column_count) && !is_uppermost(i,j,row_count,column_count)) {
#ifdef DEBUG
        std::cout << i << "," << j << "(" << to_single_index(i,j) << ") is not leftmost of uppermost, connecting to " <<
                     i-1 << "," << j-1 << " (" << to_single_index(i-1,j-1) << ")\n";
#endif
        edge_vector.insert(Edge(to_single_index(i-1,j-1),to_single_index(i,j)));
      }

      if(!is_leftmost(i,j,row_count,column_count) && !is_bottommost(i,j,row_count,column_count)) {
#ifdef DEBUG
        std::cout << i << "," << j << "(" << to_single_index(i,j) << ") is not leftmost or bottommost, connecting to " <<
                     i+1 << "," << j-1 << " (" << to_single_index(i+1,j-1) << ")\n";
#endif
        edge_vector.insert(Edge(to_single_index(i+1,j-1),to_single_index(i,j)));
      }

      if(!is_rightmost(i,j,row_count,column_count) && !is_uppermost(i,j,row_count,column_count)) {
#ifdef DEBUG
        std::cout << i << "," << j << "(" << to_single_index(i,j) << ") is not rightmost or uppermost, connecting to " <<
                     i-1 << "," << j+1 << " (" << to_single_index(i-1,j+1) << ")\n";
#endif
        edge_vector.insert(Edge(to_single_index(i-1,j+1),to_single_index(i,j)));
      }

      if(!is_rightmost(i,j,row_count,column_count) && !is_bottommost(i,j,row_count,column_count)) {
#ifdef DEBUG
        std::cout << i << "," << j << "(" << to_single_index(i,j) << ") is not rightmost or bottommost, connecting to " <<
                     i+1 << "," << j+1 << " (" << to_single_index(i+1,j+1) << ")\n";
#endif
        edge_vector.insert(Edge(to_single_index(i+1,j+1),to_single_index(i,j)));
      }
    }
  }

  std::cout << "creating graph...\n";
  g = new Graph{edge_vector.begin(), edge_vector.end(), vertex_count};
  std::cout << "vertices : " << boost::num_vertices(*g) << "; edges : " << boost::num_edges(*g) << ";\n";

  setStart(0,0);
  setGoal(row_count-1,column_count-1);

  parent_map.reserve(vertex_count);

#ifdef DEBUG
  std::cout << "graph : \n";
  for(int i=0; i<row_count; ++i){
    for(int j=0; j<column_count; ++j)
      std::cout << to_single_index(i,j) << " ";
    std::cout << '\n';
  }
#endif
}

void GraphSearch::setStart(int x, int y)
{
  std::cout << "[GraphSearch] start : " << x << ", " << y << '\n';
  need_update = true;
  start = std::make_pair(x,y);
  start_index = to_single_index(x,y);
}

void GraphSearch::setGoal(int x, int y)
{
  need_update = true;
  goal = std::make_pair(x,y);
  goal_index = to_single_index(x,y);
}

void GraphSearch::printSolution()
{
  int i =  goal_index;
  std::cout << "solution : ";
  std::cout << goal_index << " ";
  while(parent_map[i] != -1){
    std::cout << parent_map[i] << " ";
    i = parent_map[i];
  }
  std::cout << '\n';
}

void GraphSearch::printObstacles()
{
  for(int i : obstacles)
    std::cout << i << " ";
  std::cout << '\n';
}

Graph* GraphSearch::graph()
{
  return g;
}

std::tuple<int,double> GraphSearch::update(bool force_update)
{
  if(force_update || need_update) {
    std::pair<int,int> nearest;
    std::pair<int,int> nearest_goal;
    bool in_obstacle = checkStart(nearest);
    bool goal_in_obstacle = checkGoal(nearest_goal);
    auto real_start = start;
    auto real_start_index = start_index;
    auto real_goal = goal;
    auto real_goal_index = goal_index;
    if(in_obstacle)
    {
      std::cout << "nearest : " << nearest.first << ", " << nearest.second << '\n';
      setStart(nearest.first,nearest.second);
    }
    if(goal_in_obstacle)
    {
      std::cout << "nearest goal : " << nearest_goal.first << ", " << nearest_goal.second << '\n';
      setGoal(nearest_goal.first,nearest_goal.second);
    }
    auto ret = solve();
    if(in_obstacle && s)
    {
      solution.insert(solution.begin(),to_double_index(real_start_index));
      start = real_start;
      start_index = real_start_index;
    }
    if(goal_in_obstacle && s)
    {
      solution.insert(solution.end(),to_double_index(real_goal_index));
      goal = real_goal;
      goal_index = real_goal_index;
    }
    return ret;
  }
  else
    return std::make_tuple(-1,0.0);
}

void GraphSearch::markSolutions()
{
  int i = goal_index;
  solution.clear();
  while(parent_map[i] != -1){
    (*g)[i].state = Solution;
    solution.insert(solution.begin(),to_double_index(i));
    i = parent_map[i];
  }
  solution.insert(solution.begin(),to_double_index(start_index));
  (*g)[start_index].state = Solution;
}

bool GraphSearch::checkStart(std::pair<int, int> &nearest)
{
  bool ret = false;
  if(obstacles.find(start_index)!=obstacles.end())
  {
    ret = true;
    std::cout << "start in obstacles\n";
    auto start_check = filledCircle(start,5);
    double dist = sqrt((double)(column_count*column_count+row_count*row_count));
    std::pair<int,int> nearest_point = start;
    for(auto& s : start_check)
    {
      std::cout << "s : " << s.first << ", " << s.second << ' ';
      if(is_valid(s))
      {
        auto s_index = to_single_index(s.first,s.second);
        if(obstacles.find(s_index)!=obstacles.end())
          continue;
        std::cout << "valid ";
        double dx = start.first-s.first;
        double dy = start.second-s.second;
        double d = sqrt(dx*dx+dy*dy);
        if(d<dist)
        {
          std::cout << "nearest";
          nearest_point = s;
          dist = d;
        }
      }
      else
        std::cout << "not valid";
      std::cout << '\n';
    }
    nearest = nearest_point;
  }
  return ret;
}

bool GraphSearch::checkGoal(std::pair<int, int> &nearest)
{
  bool ret = false;
  if(obstacles.find(goal_index)!=obstacles.end())
  {
    ret = true;
    std::cout << "start in obstacles\n";
    auto goal_check = filledCircle(goal,5);
    double dist = sqrt((double)(column_count*column_count+row_count*row_count));
    std::pair<int,int> nearest_point = goal;
    for(auto& g : goal_check)
    {
      std::cout << "g : " << g.first << ", " << g.second << ' ';
      if(is_valid(g))
      {
        auto g_index = to_single_index(g.first,g.second);
        if(obstacles.find(g_index)!=obstacles.end())
          continue;
        std::cout << "valid ";
        double dx = goal.first-g.first;
        double dy = goal.second-g.second;
        double d = sqrt(dx*dx+dy*dy);
        if(d<dist)
        {
          std::cout << "nearest";
          nearest_point = g;
          dist = d;
        }
      }
      else
        std::cout << "not valid";
      std::cout << '\n';
    }
    nearest = nearest_point;
  }
  return ret;
}

inline
std::set<std::pair<int,int> > GraphSearch::bresenhamLines(std::pair<int, int> f, std::pair<int, int> t)
{
  std::set<std::pair<int,int> > ret;

  float dx = t.first - f.first;
  float sx = (dx<0) ? -1 : 1;
  float dy = t.second - f.second;
  float sy = (dy<0) ? -1 : 1;

  float slope, pitch;

  if(fabs(dy)<fabs(dx)) {
    slope = dy/dx;
    pitch = f.second - slope*f.first;
    for(int x0 = f.first; x0!=t.first; x0+=sx)
      ret.insert(std::make_pair(x0,(int)round(slope*x0+pitch)));
  }
  else {
    slope = dx/dy;
    pitch = f.first - slope*f.second;
    for(int y0 = f.second; y0!=t.second; y0+=sy)
      ret.insert(std::make_pair((int)round(slope*y0+pitch),y0));
  }
  ret.insert(std::make_pair(t.first,t.second));

  return ret;
}

std::set<std::pair<int, int> > GraphSearch::circle(std::pair<int, int> p0, int radius)
{
  std::set<std::pair<int,int>> ret;
  int x = radius;
  int y = 0;
  int err = 0;
  int& x0 = p0.first;
  int& y0 = p0.second;
  while(x>=y)
  {
    ret.insert(std::make_pair(x0+x, y0+y));
    ret.insert(std::make_pair(x0+y, y0+x));
    ret.insert(std::make_pair(x0-y, y0+x));
    ret.insert(std::make_pair(x0-x, y0+y));
    ret.insert(std::make_pair(x0-x, y0-y));
    ret.insert(std::make_pair(x0-y, y0-x));
    ret.insert(std::make_pair(x0+y, y0-x));
    ret.insert(std::make_pair(x0+x, y0-y));
    y+=1;
    if(err<=0)
      err += 2*y+1;
    else
    {
      x -= 1;
      err += 2*(y-x) + 1;
    }
  }
  return ret;
}

std::set<std::pair<int, int> > GraphSearch::filledCircle(std::pair<int, int> p0, int radius)
{
  std::set<std::pair<int,int>> ret;
  int x = radius;
  int y = 0;
  int err = 0;
  int& x0 = p0.first;
  int& y0 = p0.second;
  while(x>=y)
  {
    ret.insert(std::make_pair(x0+x, y0+y));
    ret.insert(std::make_pair(x0+y, y0+x));
    ret.insert(std::make_pair(x0-y, y0+x));
    ret.insert(std::make_pair(x0-x, y0+y));
    ret.insert(std::make_pair(x0-x, y0-y));
    ret.insert(std::make_pair(x0-y, y0-x));
    ret.insert(std::make_pair(x0+y, y0-x));
    ret.insert(std::make_pair(x0+x, y0-y));
    y+=1;
    if(err<=0)
      err += 2*y+1;
    else
    {
      x -= 1;
      err += 2*(y-x) + 1;
    }
  }
  for(int i=0; i<radius; i++)
  {
    for(int j=0; j<radius; j++)
    {
      if(ret.find(std::make_pair(x0+i,y0+j))!=ret.end())
        break;
      ret.insert(std::make_pair(x0+i,y0+j));
      ret.insert(std::make_pair(x0-i,y0-j));
      ret.insert(std::make_pair(x0+i,y0-j));
      ret.insert(std::make_pair(x0-i,y0+j));
    }
  }
  return ret;
}


bool GraphSearch::lineOfSight(std::pair<int, int> from, std::pair<int, int> to)
{
  auto set = bresenhamLines(from,to);
  for(auto p : set)
    if((*g)[to_single_index(p)].state == Obstacle)
      return false;
  return true;
}

std::vector<std::pair<int,int> > GraphSearch::getSolutions()
{
  return solution;
}
