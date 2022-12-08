#include <EEPROM.h>
#include<iostream> 
#include<list> 
#include<tuple> 
// if 
const int ROW = 8;
const int COL = 8;
const int NODES = 64;
minValue = 90000;

typedef unsigned tuple node; // create a tuple node with x and y 

list<int> queue;   // The nodes to be visited
list<int> FValues; // the heuristics linked to the queue nodes
list<int> visited; // The nodes we have visited
int grid[ROW][COL];
// status  0 = clear, 1 = obstacle, 2= goal

// TO DO!
// make everything distance-related
// make blacklist of obstacles.
void setup() {
                                // initialize queue as empty
                                // initialize visited as empty
  queue.insert =                // put the (1,1) node in the queue and move in
  
  // start search 

  
}

void loop() {
  while queue not empty:
    // currentNode is topNode()
    if(currentNode is Goal){
      stop;
    }
    else{
    //   addNeighbors();
      
      for node in queue:
        F = computeHeuristic();
        if(F< minValue){
          minValue = F;
        }
        // take the node that gave minValue
        // step(node with smallest min Value); 
    }
}

void step(minValueNode){
  // make robot move
  //update coordinates
}

void getTopNode(){
  //remove first in queue
}


void addNeighbors(){ // if obstacle no queue
                      // take first from queue
                      // take +1 node in each direction if possible 
                      // so for (1,2) we have (2,1) = LEFT, (2,3) = RIGHT,
                      //                      (1,1) = DOWN, (3,1) = UP
                      // add to queue 
}

// gives the function to compare nodes
void computeHeuristic(){     

  F = 1 + computeDistance(node, goal); // always 1+ because grid
}

// calculates how far we are from the goal based on the position we are in
void computeDistance(node, goal){   
// with obstacle add an iff
// if it one node appears as an obstacle 
// check how far obstacle is then dist*100
// take step where not obstacle per direction, add up
  length = goal.x - node.x;
  height = goal.y - node.y;
  distance = lenght + height;  
}

// This functions writes in memory the number of nodes
// and the goal position.
// 0 is a node
// 1 is the goal
void createNodes()
{
  for(int i = 0; i < NODES; i++){
    EEPROM.write(i,0);    
  }
  // Goal is at 8th row and 8th col 
  EEPROM.write(64,1);
}