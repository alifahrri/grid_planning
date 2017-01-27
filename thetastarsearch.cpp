#include "thetastarsearch.h"
#include <boost/thread.hpp>
#include <boost/chrono.hpp>

typedef boost::chrono::high_resolution_clock Clock;
typedef boost::chrono::milliseconds Miliseconds;
typedef Clock::time_point TimePoint;
typedef Clock::duration Duration;

#define EUCLIDEAN_HEURISTIC

ThetaStarSearch::ThetaStarSearch() :
    GraphSearch(), h_gain(1.0)
{

}

ThetaStarSearch::ThetaStarSearch(int x, int y) :
    GraphSearch(x,y), h_gain(1.0)
{

}

inline int ThetaStarSearch::goalCost(int from, int to, bool neighbor)
{
    int ret = 0;
    std::pair<int,int> f = GraphSearch::to_double_index(from);
    std::pair<int,int> t = GraphSearch::to_double_index(to);
//    if((abs(f.first-t.first)+abs(f.second-t.second))==1)
//        ret = 10;
//    else if((abs(f.first-t.first)+abs(f.second-t.second))==2)
//        ret = 14;
//    else {
//    }
    double dx = (double)(abs(f.first-t.first));
    double dy = (double)(abs(f.second-t.second));
    ret = (int)(sqrt(dx*dx+dy*dy)*10);
    return ret;
}

inline int ThetaStarSearch::goalCost(std::pair<int, int> f, std::pair<int, int> t, bool neighbbor)
{
//    if((abs(f.first-t.first)+abs(f.second-t.second))==1)
//        return 10;
//    else if((abs(f.first-t.first)+abs(f.second-t.second))==2)
//        return 14;
//    else {
//    }
    double dx = (double)(abs(f.first-t.first));
    double dy = (double)(abs(f.second-t.second));
    return (int)(sqrt(dx*dx+dy*dy)*10);
}

inline int ThetaStarSearch::heuristicCost(int idx)
{
    auto i = GraphSearch::to_double_index(idx);
#ifdef EUCLIDEAN_HEURISTIC
    double dx_f = (double)(abs(i.first-goal.first));
    double dy_f = (double)(abs(i.second-goal.second));
    return (int)(sqrt(dx_f*dx_f+dy_f*dy_f)*h_gain*10);
#else
    return (abs(i.first-goal.first)+abs(i.second-goal.second))*(int)(h_gain*10.0);
#endif
}

inline int ThetaStarSearch::heuristicCost(std::pair<int, int> i)
{
#ifdef EUCLIDEAN_HEURISTIC
    auto dxf = (double)abs(i.first-goal.first);
    auto dyf = (double)abs(i.second-goal.second);
    return (int)(sqrt(dxf*dxf+dyf*dyf)*h_gain*10);
#else
    return (abs(i.first-goal.first)+abs(i.second-goal.second))*(int)(h_gain*10.0);
#endif
}

inline int ThetaStarSearch::getVertexCost(int v)
{
    return (*g)[v].astar_prop.f;
}

inline void ThetaStarSearch::insertVertex(int v, bool has_los)
{
    size_t idx;
    size_t os = openList.size();
    int vc = getVertexCost(v);
    for(idx=0; idx < os; idx++) {
        if(getVertexCost(openList[idx])<vc)
            break;
    }
    openList.insert(openList.begin()+idx,v);
}

inline void ThetaStarSearch::setVertexState(int idx, VertexState _state)
{
    (*g)[idx].state = _state;
}

inline void ThetaStarSearch::setVertexParent(int idx, int _parent)
{
    parent_map[idx] = _parent;
    setVertexWeight(idx,(*g)[_parent].astar_prop.g+goalCost(_parent,idx),heuristicCost(idx));
}

inline void ThetaStarSearch::setVertexWeight(int i, int _g, int _h)
{
    (*g)[i].astar_prop.g = _g;
    (*g)[i].astar_prop.h = _h;
    (*g)[i].astar_prop.f = (_g == num_limit::max()) || (_h == num_limit::max()) ?
                num_limit::max() : _g+_h;
}

inline void ThetaStarSearch::resetVertex(int idx)
{
    if((*g)[idx].state!= Obstacle) {
        (*g)[idx].state = Unvisited;
    }
    setVertexWeight(idx, num_limit::max(), num_limit::max());
}

inline void ThetaStarSearch::resetGraph()
{
    for(int i=0; i<vertex_count; i++)
        parent_map[i] = -1;

    for(int i : closedList) {
        resetVertex(i);
    }
    for(int i : openList) {
        resetVertex(i);
    }
    setVertexWeight(start_index,0,heuristicCost(start_index));
    closedList.clear();
    openList.clear();
    openList.push_back(start_index);
    parent_map[start_index] = -1;
}

std::tuple<int,double> ThetaStarSearch::solve()
{
    TimePoint t = Clock::now();
    Duration d = t.time_since_epoch();
    Miliseconds start_ms = boost::chrono::duration_cast<Miliseconds>(d);
    Miliseconds end_ms;
    Miliseconds dt;

    int x = start_index;
    GraphIterator v_it, v_end;

    resetGraph();
    while(!openList.empty()) {
        x = openList.back();
        closedList.push_back(x);
        openList.pop_back();
        (*g)[x].state = Dead;
        if(x==goal_index) {
            markSolutions();
            s = true;
            need_update = false;
            t = Clock::now();
            d = t.time_since_epoch();
            end_ms = boost::chrono::duration_cast<Miliseconds>(d);
            dt = end_ms - start_ms;
            return std::make_tuple(1,dt.count()/1000.0);
        }
        auto c_vertex = to_double_index(x);
        for(boost::tie(v_it, v_end) = boost::adjacent_vertices(vertex(x,*g),*g);
            v_it != v_end; ++v_it) {
            switch((*g)[*v_it].state)
            {
            case Unvisited : {
                auto p_vertex = to_double_index(*v_it);
                int parent_of_s = (x==start_index) ? start_index : parent_map[x];
                auto ps_vertex = to_double_index(parent_of_s);
                if(lineOfSight(ps_vertex,p_vertex)) {
                    (*g)[*v_it].state = Alive;
                    parent_map[*v_it] = parent_of_s;
                    (*g)[*v_it].astar_prop.g = (*g)[parent_of_s].astar_prop.g + goalCost(ps_vertex,p_vertex);
                    (*g)[*v_it].astar_prop.h = heuristicCost(p_vertex);
                    (*g)[*v_it].astar_prop.f = (*g)[*v_it].astar_prop.g + (*g)[*v_it].astar_prop.h;
                    insertVertex(*v_it);
                }
                else {
                    (*g)[*v_it].state = Alive;
                    parent_map[*v_it] = x;
                    (*g)[*v_it].astar_prop.g = (*g)[x].astar_prop.g + goalCost(c_vertex,p_vertex);
                    (*g)[*v_it].astar_prop.h = heuristicCost(p_vertex);
                    (*g)[*v_it].astar_prop.f = (*g)[*v_it].astar_prop.g + (*g)[*v_it].astar_prop.h;
                    insertVertex(*v_it);
                }
                break;
            }
            case Alive: {
                auto p_vertex = to_double_index(*v_it);
                int tentative_goal = (*g)[x].astar_prop.g + goalCost(c_vertex,p_vertex);
                //                if(tentative_goal < (*g)[*v_it].astar_prop.g)
                //                    parent_map[*v_it] = x;
                break;
            }
            default : break;
            }
        }
#ifdef USE_DELAY
        if(en_delay)
            boost::this_thread::sleep_for(boost::chrono::milliseconds(delay_ms));
#endif
    }
    need_update = false;
    t = Clock::now();
    d = t.time_since_epoch();
    end_ms = boost::chrono::duration_cast<Miliseconds>(d);
    dt = end_ms - start_ms;
    s = false;
    return std::make_tuple(0,dt.count()/1000.0);
}

std::string ThetaStarSearch::string(int i, int option)
{
    std::string s;
    switch(option) {
    case 0: {
        s += "(";
        int g_ = (*g)[i].astar_prop.g;
        s += g_ == num_limit::max() ? "inf" : std::to_string(g_);
        s += ")";
        break;
    }
    case 1: {
        s += "(";
        auto p = to_double_index(i);
        s += std::to_string(p.first);
        s += ",";
        s += std::to_string(p.second);
        s += ")";
        break;
    }
    case 2: {
        s += "theta-star";
    }
    default :
        break;
    }
    return s;
}
