#ifndef GRID
#define GRID
#include "Node.h"
#include "vector" 
#include "map" 
#include "algorithm" 
#include "string"

class Grid {
  public:
    enum DIRECTION_FACING  {UP=0, DOWN=1, LEFT=2, RIGHT=3};

    int minX, minY;
    int maxX, maxY;
    std::vector<Node*> nodes;
    Node* currentNode = nullptr;
    Node* approachedNode = nullptr;
    std::vector<Node> blacklist;
    std::map<Node*, int> neighboursHeuristics;
    DIRECTION_FACING facing;

    Grid(int x = 0, int y = 0, int width = 7, int height = 7, Node goal = Node(7,7), std::vector<Node> blacklist = {},DIRECTION_FACING facing =DIRECTION_FACING::DOWN);
    
    Node * at(int x, int y);
    void add_approached_to_blacklist();
    std::vector<Node*> get_neighbours();
    void update_distances_for_neighbours();
    void decide_move(bool& move_decided);
    void finish_move(bool& move_decided);
    int get_current_x();
    int get_current_y();

};
#endif
