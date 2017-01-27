#ifndef FORWARDSEARCH_H
#define FORWARDSEARCH_H

#include <iostream>
#include <utility>
#include <queue>
#include <boost/graph/adjacency_list.hpp>

enum VertexState {Unvisited, Dead, Alive};

struct VertexProperties {
    VertexProperties() : state(Unvisited) {}
    VertexState state;
};

typedef boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS, VertexProperties> Graph;
typedef Graph::vertex_descriptor VertexDescriptor;
typedef boost::graph_traits<Graph>::adjacency_iterator GraphIterator;
typedef Graph::vertex_iterator VertexIterator;
typedef boost::property_map<Graph,boost::vertex_index_t>::type IndexMap;
typedef std::pair<int,int> Edge;


class ForwardSearch
{
public:
    ForwardSearch();
    ForwardSearch(int x, int y);
    ~ForwardSearch(){delete g;}
    void setStart(int x, int y);
    void setGoal(int x, int y);
    bool solve();
    void printSolution();

private:
    std::pair<int,int> start;
    std::pair<int,int> goal;
    std::vector<std::pair<int,int> > solution;
    std::queue<int> q;
    std::vector<int> parent_map;
    int start_index;
    int goal_index;
    int vertex_count;
    int edge_count;
    int row_count;
    int column_count;
    Graph *g;

private:
    void makeGrid(int x, int y);
    virtual void resolveDuplicate();
};

#endif // FORWARDSEARCH_H
