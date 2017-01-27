#include "astarsearch.h"
#include <algorithm>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>

typedef boost::chrono::high_resolution_clock Clock;
typedef boost::chrono::milliseconds Miliseconds;
typedef Clock::time_point TimePoint;
typedef Clock::duration Duration;

//#define DEBUG
//#define EUCLIDEAN_HEURISTIC
#define OCTILE_HEURISTIC

inline
int AStarSearch::goalCost(int from, int to)
{
    int ret = 0;
    std::pair<int,int> f = GraphSearch::to_double_index(from);
    std::pair<int,int> t = GraphSearch::to_double_index(to);
    if((abs(f.first-t.first)+abs(f.second-t.second))==1)
        ret = 10;
    else
        ret = 14;
    return ret;
}

inline
int AStarSearch::goalCost(std::pair<int, int> f, std::pair<int, int> t)
{
    if((abs(f.first-t.first)+abs(f.second-t.second))==1)
        return 10;
    else
        return 14;
}

inline
int AStarSearch::heuristicCost(int idx)
{
    std::pair<int,int> i = GraphSearch::to_double_index(idx);
#ifdef EUCLIDEAN_HEURISTIC
    double dx_f = (double)(abs(i.first-goal.first));
    double dy_f = (double)(abs(i.second-goal.second));
    return (int)(sqrt(dx_f*dx_f+dy_f*dy_f)*h_gain*10);
#elif defined(OCTILE_HEURISTIC)
    double dx_f = (double)(abs(i.first-goal.first));
    double dy_f = (double)(abs(i.second-goal.second));
    return (int)(((dx_f>dy_f?dx_f:dy_f)+(0.41)*(dx_f<dy_f?dx_f:dy_f))*h_gain*10);
#else
    return (abs(i.first-goal.first)+abs(i.second-goal.second))*(int)(h_gain*10.0);
#endif
}

inline
int AStarSearch::heuristicCost(std::pair<int, int> i)
{
#ifdef EUCLIDEAN_HEURISTIC
    auto dxf = (double)abs(i.first-goal.first);
    auto dyf = (double)abs(i.second-goal.second);
    return (int)(sqrt(dxf*dxf+dyf*dyf)*h_gain*10);
#elif defined(OCTILE_HEURISTIC)
    double dx_f = (double)(abs(i.first-goal.first));
    double dy_f = (double)(abs(i.second-goal.second));
    return (int)(((dx_f>dy_f?dx_f:dy_f)+(0.41)*(dx_f<dy_f?dx_f:dy_f))*h_gain*10);
#else
    return (abs(i.first-goal.first)+abs(i.second-goal.second))*(int)(h_gain*10.0);
#endif
}

inline
int AStarSearch::heuristicEstimate(int index)
{
    std::pair<int,int> p = GraphSearch::to_double_index(index);
    std::pair<int,int> g = GraphSearch::to_double_index(goal_index);
    int dx = p.first-g.first;
    int dy = p.second-g.second;
    return (int)(sqrt((double)(dx*dx+dy*dy)));
}

/*
bool AStarSearch::compareVertex(const int &a, const int &b) const
{
    return (*g)[a].astar_prop.f == (*g)[b].astar_prop.f ? (*g)[a].astar_prop.h < (*g)[b].astar_prop.h :
                                                          (*g)[a].astar_prop.f < (*g)[b].astar_prop.f;
}
*/

AStarSearch::AStarSearch()
    : GraphSearch(), h_gain(1.0)
{

}

AStarSearch::AStarSearch(int x, int y)
    : GraphSearch(x,y), h_gain(1.0)
{
#ifdef DEBUG
    std::cout << "creating astar search..\n";
#endif
    goal_value.resize(vertex_count);
    heuristic_value.resize(vertex_count);
    total_value.resize(vertex_count);
#ifndef BUNDLE_PROP_VERSION
    node.resize(vertex_count);
#endif
    for(int i=0; i<vertex_count; ++i) {
#ifndef BUNDLE_PROP_VERSION
        node[i] = Node(i);
#endif
    }
}

std::tuple<int, double> AStarSearch::solve()
{
#ifdef DEBUG
    std::cout << "solving..\n";
    std::cout << "goal index : " << goal_index << '\n';
#endif
    TimePoint t = Clock::now();
    Duration d = t.time_since_epoch();
    Miliseconds start_ms = boost::chrono::duration_cast<Miliseconds>(d);
    Miliseconds end_ms;
    Miliseconds dt;

    int x = start_index;
    GraphIterator v_it, v_end;

    resetGraph();

#ifdef BUNDLE_PROP_VERSION
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
                (*g)[*v_it].state = Alive;
                parent_map[*v_it] = x;
                (*g)[*v_it].astar_prop.g = (*g)[x].astar_prop.g + goalCost(c_vertex,p_vertex);
                (*g)[*v_it].astar_prop.h = heuristicCost(p_vertex);
                (*g)[*v_it].astar_prop.f = (*g)[*v_it].astar_prop.g + (*g)[*v_it].astar_prop.h;
                insertVertex(*v_it);
                break;
            }
            case Alive: {
                auto p_vertex = to_double_index(*v_it);
                int tentative_goal = (*g)[x].astar_prop.g + goalCost(c_vertex,p_vertex);
                if(tentative_goal < (*g)[*v_it].astar_prop.g)
                    parent_map[*v_it] = x;
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
#else
    while(!openList.empty()) {
        //        std::sort(openList.begin(), openList.end(), std::greater<Node>());
        Node n = openList.back();
        x = n.idx;
        (*g)[x].state = Dead;
        openList.pop_back();
        closedList.push_back(n);
        if(x == goal_index) {
            markSolutions();
            s = true;
            need_update = false;
            t = Clock::now();
            d = t.time_since_epoch();
            end_ms = boost::chrono::duration_cast<Miliseconds>(d);
            dt = end_ms - start_ms;
            return std::make_tuple(1,dt.count()/1000.0);
        }
        for(boost::tie(v_it, v_end) = boost::adjacent_vertices(vertex(x,*g),*g);
            v_it != v_end; ++v_it) {
            switch((*g)[*v_it].state)
            {
            case Unvisited : {
                (*g)[*v_it].state = Alive;
                parent_map[*v_it] = x;
                node[*v_it].g = node[x].g + goalCost(x,*v_it);
                node[*v_it].h = heuristicCost(*v_it);
                node[*v_it].f = node[*v_it].g + node[*v_it].h;
                //                openList.push_back(node[*v_it]);
                insertVertex(*v_it);
                break;
            }
            case Alive: {
                int tentative_goal = node[x].g + goalCost(x,*v_it);
                if(tentative_goal < node[*v_it].g)
                    parent_map[*v_it] = x;
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
#endif
    need_update = false;
    t = Clock::now();
    d = t.time_since_epoch();
    end_ms = boost::chrono::duration_cast<Miliseconds>(d);
    dt = end_ms - start_ms;
    s = false;
    return std::make_tuple(0,dt.count()/1000.0);
}

std::string AStarSearch::string(int i, int option)
{
    std::string s;
    switch(option) {
    case 0: {
        s += "(";
#ifdef BUNDLE_PROP_VERSION
        int g_ = (*g)[i].astar_prop.g;
        s += g_ == num_limit::max() ? "inf" : std::to_string(g_);
        s += ")";
#else
        s += node[i].g == num_limit::max() ? "inf" : std::to_string(node[i].g) ;
        s += ")";
#endif
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
        s += "a-star";
    }
    default :
        break;
    }
    return s;
}

inline
int AStarSearch::getVertexCost(int v)
{
#ifdef BUNDLE_PROP_VERSION
    return (*g)[v].astar_prop.f;
#else
    return node[v].f;
#endif
}

inline
void AStarSearch::insertVertex(int v)
{
#ifdef BUNDLE_PROP_VERSION
    size_t idx;
    size_t os = openList.size();
    int vc = getVertexCost(v);
    for(idx=0; idx < os; idx++) {
        if(getVertexCost(openList[idx])<vc)
            break;
    }
    openList.insert(openList.begin()+idx,v);
#else
    size_t idx;
    size_t os = openList.size();
    int vc = node[v].f;
    for(idx=0; idx < os; idx++) {
        if(openList[idx].f < vc)
            break;
    }
    openList.insert(openList.begin()+idx,node[v]);
#endif
}

inline
void AStarSearch::setVertexState(int idx, VertexState _state)
{
    (*g)[idx].state = _state;
}

inline
void AStarSearch::setVertexParent(int idx, int _parent)
{
    parent_map[idx] = _parent;
    setVertexWeight(idx,(*g)[_parent].astar_prop.g+goalCost(_parent,idx),heuristicCost(idx));
}

inline
void AStarSearch::setVertexWeight(int i, int _g, int _h)
{
    (*g)[i].astar_prop.g = _g;
    (*g)[i].astar_prop.h = _h;
    (*g)[i].astar_prop.f = (_g == num_limit::max()) || (_h == num_limit::max()) ?
                num_limit::max() : _g+_h;
}

inline
void AStarSearch::resetVertex(int idx)
{
    if((*g)[idx].state!= Obstacle) {
        (*g)[idx].state = Unvisited;
    }
    setVertexWeight(idx, num_limit::max(), num_limit::max());
}

inline
void AStarSearch::resetGraph()
{
    for(size_t i=0; i<vertex_count; i++)
        parent_map[i] = -1;
    int x = start_index;

#ifdef BUNDLE_PROP_VERSION
    for(int i : closedList) {
        resetVertex(i);
    }
    for(int i : openList) {
        resetVertex(i);
    }
    setVertexWeight(start_index,0,heuristicCost(start_index));
#else
    for(Node n : closedList) {
        if((*g)[n.idx].state!=Obstacle) {
            (*g)[n.idx].state = Unvisited;
            //            node[n.idx] = Node(n.idx);
            node[n.idx].reset();
        }
    }
    for(Node n : openList) {
        if((*g)[n.idx].state!= Obstacle) {
            (*g)[n.idx].state = Unvisited;
            //            node[n.idx] = Node(n.idx);
            node[n.idx].reset();
        }
    }
    node[x].g=0;
    node[x].h=heuristicCost(x);
    node[x].f=node[x].g+node[x].h;
#endif
    closedList.clear();
    openList.clear();
#ifdef BUNDLE_PROP_VERSION
    openList.push_back(start_index);
#else
    openList.push_back(node[x]);
#endif
    parent_map[x] = -1;
}
