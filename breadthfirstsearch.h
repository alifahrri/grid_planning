#ifndef BREADTHFIRSTSEARCH_H
#define BREADTHFIRSTSEARCH_H

#include "graphsearch.h"

class BreadthFirstSearch : public GraphSearch
{
public:
    BreadthFirstSearch();
    BreadthFirstSearch(int x, int y);
    ~BreadthFirstSearch(){}
    std::tuple<int,double> solve();
    std::string string(int i, int option=0);
};

#endif // BREADTHFIRSTSEARCH_H
