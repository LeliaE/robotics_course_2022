#ifndef GRID
#define GRID
#include "Node.h"
#include<vector> 
#include<map> 
#include<algorithm> 
#include<string>

class Grid {

    public:
        enum direction_facing  {UP=0, DOWN=1, LEFT=2, RIGHT=3};

        int min_x, min_y;
        int max_x, max_y;
        std::vector<Node*> nodes;
        Node* currentNode = nullptr;
        Node* approachedNode = nullptr;
        std::vector<Node> blacklist;
        std::map<Node*, int> neighbours_heuristics;
        direction_facing facing;

        Grid(int x = 0, int y = 0, int width = 7, int height = 7, Node goal = Node(7,7), std::vector<Node> blacklist = {}, direction_facing facing = direction_facing::DOWN);
        
        Node * at(int x, int y);
        void add_approached_to_blacklist();
        std::vector<Node*> get_neighbours();
        void update_distances_for_neighbours();
        void decide_move(bool& move_decided);
        void finish_move(bool& move_decided);

};

/*std::ostream& operator<<(std::ostream& os, Grid& grid)
{
    // indices
    for (int w = grid.min_x; w <= grid.max_x; ++w) {
        for (int h = grid.min_y; h <= grid.max_y; ++h) {
            os << grid.at(w,h)->status << " (" << grid.at(w,h)->print() << ")";
            // std::cout << 

            os << "\t\t";
        }
        os << '\n';
    }

    return os;
}*/

#endif
