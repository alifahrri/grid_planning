#ifndef GRAPHSEARCH_H
#define GRAPHSEARCH_H

#include <iostream>
#include <iterator>
#include <utility>
#include <queue>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <string>
#include <boost/graph/adjacency_list.hpp>

#define USE_DELAY

enum VertexState {Unvisited, Dead, Alive, Obstacle, Solution};

struct AStarProperties {
    int g;
    int h;
    int f;
};

struct VertexProperties {
    VertexProperties() : state(Unvisited) {}
    VertexState state;
    AStarProperties astar_prop;
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, VertexProperties> Graph;
typedef boost::graph_traits<Graph>::adjacency_iterator GraphIterator;
typedef boost::property_map<Graph,boost::vertex_index_t>::type IndexMap;
typedef Graph::vertex_descriptor VertexDescriptor;
typedef Graph::vertex_iterator VertexIterator;
typedef std::pair<int,int> Edge;


class GraphSearch
{
public:
    GraphSearch();
    GraphSearch(int x, int y);
    ~GraphSearch(){delete g;}
    void setStart(int x, int y);
    void setGoal(int x, int y);
    int startIndex() { return start_index; }
    int goalIndex() { return goal_index; }
    virtual std::tuple<int,double> solve() = 0;
    virtual std::string string(int i, int option=0) = 0;
    std::tuple<int,double> update(bool force_update=false);
    std::vector<std::pair<int,int> >getSolutions();
    bool solved() { return s; }
    void printSolution();
    void addObstacle(int ob);
    void addObstacle(std::pair<int,int> ob);
    void addObstacles(std::set<int> ob);
    void addObstacles(std::set<std::pair<int,int>> ob);
    void addObstacles(std::pair<int,int> center, int radius);
    void setObstacles(std::set<int> ob);
    void setObstacles(std::set<std::pair<int, int> > ob);
    void clearObstacle(int ob);
    void clearObstacle(std::pair<int,int> ob);
    void clearObstacles(std::set<int> ob);
    void clearObstacles(std::set<std::pair<int,int> > ob);
    void printObstacles();
    std::pair<int,int> getStart() { return start; }
    std::pair<int,int> getGoal() { return goal; }
    std::set<int> getObstacles();
    Graph* graph();

protected:
    std::pair<int,int> start;
    std::pair<int,int> goal;
    std::vector<std::pair<int,int> > solution;
    std::queue<int> q;
    std::vector<int> parent_map;
    std::set<int> obstacles;
    std::set<int> visitedList;
    int start_index;
    int goal_index;
    int vertex_count;
    int edge_count;
    int row_count;
    int column_count;
    bool s;
    bool need_update;
    Graph *g;

protected:
    std::set<std::pair<int,int>> bresenhamLines(std::pair<int,int> f, std::pair<int,int> t);
    std::set<std::pair<int,int>> circle(std::pair<int,int> p0, int radius);
    std::set<std::pair<int,int>> filledCircle(std::pair<int,int> p0, int radius);
    bool lineOfSight(std::pair<int,int> from, std::pair<int,int> to);
    bool checkStart(std::pair<int,int>& nearest);
    bool checkGoal(std::pair<int,int>& nearest);
    void markSolutions();
    int to_single_index(int x_, int y_);
    int to_single_index(std::pair<int,int> p);
    bool is_valid(std::pair<int,int> p);
    std::pair<int,int> to_double_index(int idx);

private:
    void makeGrid(int x, int y);

#ifdef USE_DELAY
public:
    void setDelay(bool s, int d) {en_delay=s; delay_ms=d;}
protected:
    bool en_delay;
    int delay_ms;
#endif
};

#endif // GRAPHSEARCH_H
