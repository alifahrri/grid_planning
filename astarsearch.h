#ifndef ASTARSEARCH_H
#define ASTARSEARCH_H

#include "graphsearch.h"

#include <limits>

#define BUNDLE_PROP_VERSION

typedef std::numeric_limits<int> num_limit;

class AStarSearch : public GraphSearch
{
public:
    AStarSearch();
    AStarSearch(int x, int y);
    ~AStarSearch(){}
    std::tuple<int,double> solve();
    void setHeuristicWeight(double hg) { h_gain = hg; }
    std::string string(int i, int option=0);
    struct Node {
        int idx;
        int g, h, f;
        Node() { g=num_limit::max(); h=num_limit::max(); f=num_limit::max(); idx=0; }
        Node(int i) { g=num_limit::max(); h=num_limit::max(); f=num_limit::max(); idx=i; }
        Node(int _g, int _h, int _f, int i) { g=_g; h=_h; f=_f; idx=i; }
        int total() { return f; }
        void reset() { g=num_limit::max(); h=num_limit::max(); f=num_limit::max(); }
        bool operator<(const Node& other) const { return f == other.f ? h < other.h : f < other.f; /*(other.g+other.h) < (g+h);*/ }
        bool operator>(const Node& other) const { return f == other.f ? h < other.h : f > other.f; /*(other.g+other.h) > (g+h);*/ }
        bool operator==(const Node& other) const { return idx == other.idx; }
    };

private:
    int heuristicEstimate(int index);
    int goalCost(int from, int to);
    int goalCost(std::pair<int,int> f, std::pair<int,int> t);
    int heuristicCost(int idx);
    int heuristicCost(std::pair<int,int> i);
    int getVertexCost(int v);
    void insertVertex(int v);
    void sortOpenList();
    void resetGraph();
    void resetVertex(int idx);
    void setVertexWeight(int i, int _g, int _h);
    void setVertexState(int idx, VertexState _state);
    void setVertexParent(int idx, int _parent);

private:
    double h_gain;
#ifdef BUNDLE_PROP_VERSION
    std::vector<int> openList;
    std::vector<int> closedList;
#else
    std::vector<Node> openList;
    std::vector<Node> closedList;
    std::priority_queue<Node, std::vector<Node>, std::less<std::vector<Node>::value_type> > pq;
    std::vector<Node> node;
#endif
    std::vector<int> goal_value;
    std::vector<int> heuristic_value;
    std::vector<int> total_value;
};

#endif // ASTARSEARCH_H
