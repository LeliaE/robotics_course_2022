#include <ArduinoSTL.h>
#include "Node.h"
#include "Grid.h"

#include<vector> 
#include<map> 
#include<algorithm> 
#include<string>

std::vector<Node> blacklist = {Node(0,3), Node(1,1), Node(7,6)};
Grid grid(0, 0, 7, 7, Node(7,7), blacklist);

void print_neighbours(std::vector<Node*> neighs) {
    //std::cout << "Neighbours: ";
    for (const auto& n : neighs) {
        //std::cout << n->print() << "   |   ";
    }
    //std::cout << std::endl;
}

void setup() {
    std::vector<Node> blacklist = {Node(0,3), Node(1,1), Node(7,6)};
    Grid grid(0, 0, 7, 7, Node(7,7), blacklist);
}

void update() {
 
}

void loop() {
  bool arrived = false;
  while(!arrived) {
        //std::cout << "Moving to: "<< grid.decide_move() << std::endl;
        grid.decide_move();
        grid.actually_move();
        //std::cout << "Arrived!" << std::endl;

        if(grid.currentNode->status == Node::GOAL) {
            arrived=true;
        }

    }
}
