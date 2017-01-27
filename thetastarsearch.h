#ifndef THETASTARSEARCH_H
#define THETASTARSEARCH_H

#include "graphsearch.h"

typedef std::numeric_limits<int> num_limit;

class ThetaStarSearch : public GraphSearch
{
public:
    ThetaStarSearch();
    ThetaStarSearch(int x, int y);
    ~ThetaStarSearch(){}
    std::tuple<int,double> solve();
    void setHeuristicWeight(double hg) { h_gain = hg; }
    std::string string(int i, int option=0);

private:
    int heuristicCost(int idx);
    int heuristicCost(std::pair<int,int> i);
    int goalCost(int from, int to, bool neighbor=true);
    int goalCost(std::pair<int,int> f, std::pair<int,int> t, bool neighbbor=true);
    int getVertexCost(int v);
    void insertVertex(int v,bool has_los=false);
    void resetGraph();
    void resetVertex(int idx);
    void setVertexWeight(int i, int _g, int _h);
    void setVertexState(int idx, VertexState  _state);
    void setVertexParent(int idx, int _parent);

private:
    double h_gain;
    std::vector<int> openList;
    std::vector<int> closedList;
};

#endif // THETASTARSEARCH_H
