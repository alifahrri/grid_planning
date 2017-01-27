#include "forwardsearch.h"

#define DEFAULT_SIZE_X 1000
#define DEFAULT_SIZE_Y 1000

static inline int to_single_index(int x_, int y_, int row_, int column_)
{
    return x_*column_+y_;
}

static inline std::pair<int,int> to_double_index(int idx, int row_, int column_)
{
    int x = idx/column_;
    int y = idx%column_;
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

ForwardSearch::ForwardSearch()
{
    makeGrid(DEFAULT_SIZE_X,DEFAULT_SIZE_Y);
}

ForwardSearch::ForwardSearch(int x, int y)
{
    makeGrid(x,y);
}

void ForwardSearch::makeGrid(int x, int y)
{
    std::cout << "creating grid..\n";
    vertex_count = x*y;
    row_count = y;
    column_count = x;

    std::cout << "row : " << row_count << " column : " << column_count << '\n';
    std::cout << "vertex count : " << vertex_count << '\n';
    std::set<Edge> edge_vector;
    for(int i=0; i<row_count; ++i) {
        for(int j=0; j<column_count; ++j){
            if(!is_leftmost(i,j,row_count,column_count)) {
#ifdef DEBUG
                std::cout << i << "," << j << "(" << to_single_index(i,j,row_count,column_count) << ") is not leftmost, connecting to " <<
                             i << "," << j-1 << " (" << to_single_index(i,j-1,row_count,column_count) << ")\n";
#endif
                edge_vector.insert(Edge(to_single_index(i,j-1,row_count,column_count),to_single_index(i,j,row_count,column_count)));
            }

            if(!is_rightmost(i,j,row_count,column_count)) {
#ifdef DEBUG
                std::cout << i << "," << j << "(" << to_single_index(i,j,row_count,column_count) << ") is not rightmost, connecting to " <<
                             i << "," << j+1 << " (" << to_single_index(i,j+1,row_count,column_count) << ")\n";
#endif
                edge_vector.insert(Edge(to_single_index(i,j,row_count,column_count),to_single_index(i,j+1,row_count,column_count)));
            }

            if(!is_uppermost(i,j,row_count,column_count)) {
#ifdef DEBUG
                std::cout << i << "," << j << "(" << to_single_index(i,j,row_count,column_count) << ") is not uppermost, connecting to " <<
                             i-1 << "," << j << " (" << to_single_index(i-1,j,row_count,column_count) << ")\n";
#endif
                edge_vector.insert(Edge(to_single_index(i-1,j,row_count,column_count),to_single_index(i,j,row_count,column_count)));
            }

            if(!is_bottommost(i,j,row_count,column_count)) {
#ifdef DEBUG
                std::cout << i << "," << j << "(" << to_single_index(i,j,row_count,column_count) << ") is not bottommost, connecting to " <<
                             i+1 << "," << j << " (" << to_single_index(i+1,j,row_count,column_count) << ")\n";
#endif
                edge_vector.insert(Edge(to_single_index(i,j,row_count,column_count),to_single_index(i+1,j,row_count,column_count)));
            }

            if(!is_leftmost(i,j,row_count,column_count) && !is_uppermost(i,j,row_count,column_count)) {
#ifdef DEBUG
                std::cout << i << "," << j << "(" << to_single_index(i,j,row_count,column_count) << ") is not leftmost of uppermost, connecting to " <<
                             i-1 << "," << j-1 << " (" << to_single_index(i-1,j-1,row_count,column_count) << ")\n";
#endif
                edge_vector.insert(Edge(to_single_index(i-1,j-1,row_count,column_count),to_single_index(i,j,row_count,column_count)));
            }

            if(!is_leftmost(i,j,row_count,column_count) && !is_bottommost(i,j,row_count,column_count)) {
#ifdef DEBUG
                std::cout << i << "," << j << "(" << to_single_index(i,j,row_count,column_count) << ") is not leftmost or bottommost, connecting to " <<
                             i+1 << "," << j-1 << " (" << to_single_index(i+1,j-1,row_count,column_count) << ")\n";
#endif
                edge_vector.insert(Edge(to_single_index(i+1,j-1,row_count,column_count),to_single_index(i,j,row_count,column_count)));
            }

            if(!is_rightmost(i,j,row_count,column_count) && !is_uppermost(i,j,row_count,column_count)) {
#ifdef DEBUG
                std::cout << i << "," << j << "(" << to_single_index(i,j,row_count,column_count) << ") is not rightmost or uppermost, connecting to " <<
                             i-1 << "," << j+1 << " (" << to_single_index(i-1,j+1,row_count,column_count) << ")\n";
#endif
                edge_vector.insert(Edge(to_single_index(i-1,j+1,row_count,column_count),to_single_index(i,j,row_count,column_count)));
            }

            if(!is_rightmost(i,j,row_count,column_count) && !is_bottommost(i,j,row_count,column_count)) {
#ifdef DEBUG
                std::cout << i << "," << j << "(" << to_single_index(i,j,row_count,column_count) << ") is not rightmost or bottommost, connecting to " <<
                             i+1 << "," << j+1 << " (" << to_single_index(i+1,j+1,row_count,column_count) << ")\n";
#endif
                edge_vector.insert(Edge(to_single_index(i+1,j+1,row_count,column_count),to_single_index(i,j,row_count,column_count)));
            }
            /*
            if(!is_leftmost(i,j,row_count,column_count))
                edge_vector.push_back(Edge(to_single_index(i-1,j,row_count,column_count),to_single_index(i,j,row_count,column_count)));

            if(!is_rightmost(i,j,row_count,column_count))
                edge_vector.push_back(Edge(to_single_index(i,j,row_count,column_count),to_single_index(i+1,j,row_count,column_count)));

            if(!is_uppermost(i,j,row_count,column_count))
                edge_vector.push_back(Edge(to_single_index(i,j-1,row_count,column_count),to_single_index(i,j,row_count,column_count)));

            if(!is_bottommost(i,j,row_count,column_count))
                edge_vector.push_back(Edge(to_single_index(i,j,row_count,column_count),to_single_index(i,j+1,row_count,column_count)));

            if(!is_leftmost(i,j,row_count,column_count) && !is_uppermost(i,j,row_count,column_count))
                edge_vector.push_back(Edge(to_single_index(i-1,j-1,row_count,column_count),to_single_index(i,j,row_count,column_count)));

            if(!is_leftmost(i,j,row_count,column_count) && !is_bottommost(i,j,row_count,column_count))
                edge_vector.push_back(Edge(to_single_index(i-1,j+1,row_count,column_count),to_single_index(i,j,row_count,column_count)));

            if(!is_rightmost(i,j,row_count,column_count) && !is_uppermost(i,j,row_count,column_count))
                edge_vector.push_back(Edge(to_single_index(i+1,j-1,row_count,column_count),to_single_index(i,j,row_count,column_count)));

            if(!is_rightmost(i,j,row_count,column_count) && !is_bottommost(i,j,row_count,column_count))
                edge_vector.push_back(Edge(to_single_index(i+1,j+1,row_count,column_count),to_single_index(i,j,row_count,column_count)));
            */
        }
    }

    std::cout << "creating graph...\n";
    g = new Graph{edge_vector.begin(), edge_vector.end(), vertex_count};

    setStart(0,0);
    setGoal(row_count-1,column_count-1);

    parent_map.reserve(vertex_count);

#ifdef DEBUG
    std::cout << "graph : \n";
    for(int i=0; i<row_count; ++i){
        for(int j=0; j<column_count; ++j)
            std::cout << to_single_index(i,j,row_count,column_count) << " ";
        std::cout << '\n';
    }
#endif

    /*
//	GraphIterator v_it, v_end;
    GraphIterator a_it, a_end;
    VertexIterator v_it, v_end;
//	VertexIterator a_it, a_end;

    for(boost::tie(v_it,v_end) = boost::vertices(*g);
        v_it != v_end; ++v_it)
    {
        std::pair<int,int> d_idx = to_double_index(*v_it,row_count,column_count);
        std::cout << "\nadjacent vertices of " << *v_it << " (" << d_idx.first << "," << d_idx.second << ") : ";
        for(boost::tie(a_it,a_end) = boost::adjacent_vertices(*v_it,*g);
            a_it != a_end; ++a_it)
        {
            std::cout << *a_it << " ";
        }
    }
    std::cout << '\n';
    */

    std::cout << "solving.. \nstart_index : " << start_index << "\ngoal_index : " << goal_index << '\n';
    if(solve()){
        std::cout << "solved!\n";
        printSolution();
    }
    else
        std::cout << "solution not found\n";
}

void ForwardSearch::setStart(int x, int y)
{
    start = std::make_pair(x,y);
    start_index = to_single_index(x,y,row_count,column_count);
}

void ForwardSearch::setGoal(int x, int y)
{
    goal = std::make_pair(x,y);
    goal_index = to_single_index(x,y,row_count,column_count);
}

bool ForwardSearch::solve()
{
    int x;
#ifdef DEBUG
    std::pair<int,int> d_idx;
#endif
    GraphIterator v_it, v_end;
    q.push(start_index);
    (*g)[vertex(start_index,*g)].state = Alive;
    parent_map[start_index] = -1;
    while(!q.empty()) {
        x = q.front();
        if(x == goal_index)
            return true;
#ifdef DEBUG
        d_idx = to_double_index(x,row_count,column_count);
        std::cout << "adjacent vertices of " << x << " (" << d_idx.first << "," << d_idx.second << ") : ";
#endif
        for(boost::tie(v_it, v_end) = boost::adjacent_vertices(vertex(x,*g),*g);
            v_it != v_end; ++v_it) {
#ifdef DEBUG
            std::cout << *v_it << " ";
#endif
            if((*g)[*v_it].state == Unvisited) {
                (*g)[*v_it].state =  Alive;
                parent_map[*v_it] = x;
                q.push(*v_it);
            }
            else
                resolveDuplicate();
        }
#ifdef DEBUG
        std::cout << '\n';
#endif
        q.pop();
    }
    return false;
}

void ForwardSearch::printSolution()
{
    /*
    for(int i=0; i<row_count; i++){
        for(int j=0; j<column_count; j++)
            std::cout << parent_map[to_single_index(i,j,row_count,column_count)] << " ";
        std::cout << '\n';
    }
    */
    int i =  goal_index;
    std::cout << "solution : ";
    std::cout << goal_index << " ";
    while(parent_map[i] != -1){
        std::cout << parent_map[i] << " ";
        i = parent_map[i];
    }
    std::cout << '\n';
}

void ForwardSearch::resolveDuplicate()
{

}
