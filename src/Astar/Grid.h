/*
  Program name: Grid
  Authors: Amelie Schneider (C++ master), Lelia Erscoi (algorithm architect),
           Hritika Kathuria (object detection technician)
  Date created: 08-12-2022
  Last updated: 31-12-2022
  Description: Defining class for the grid. Has functionality for neighbor nodes
               and finding the next best move based on a heuristic.
*/

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
    float distanceTravelled = 0.0;  // the distance travelled so far
    float distanceTravelledBackwards = 0.0; 

    Grid(int x = 0, int y = 0, int width = 7, int height = 7, Node goal = Node(7,7),DIRECTION_FACING facing =DIRECTION_FACING::DOWN);
    
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
