/*
  Program name: Node
  Authors: Amelie Schneider (C++ master), Lelia Erscoi (algorithm architect),
           Hritika Kathuria (object detection technician)
  Date created: 08-12-2022
  Last updated: 31-12-2022
  Description: Defining class for the nodes. A node is the smallest unit of the grid,
               has a position and a status.
*/
#ifndef NODE
#define NODE

class Node {
  public:
    enum STATUS_ENUM  {CLEAR=0, OBSTACLE=1, GOAL=99};

    const int x;
    const int y;
    STATUS_ENUM status;

    Node(int x, int y, STATUS_ENUM status = STATUS_ENUM::CLEAR) : x(x), y(y), status(status) {};

    bool is_clear() { return status == CLEAR; }

    bool is_goal() { return status == GOAL; }

    bool is_clear_or_goal() { return status == CLEAR || status == GOAL; }
    
    bool is_node(Node other) { return (this->x == other.x && this->y == other.y); }
};
#endif
