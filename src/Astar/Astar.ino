#include <ArduinoSTL.h>
#include <ZumoShield.h>
#include<Wire.h>
#include <arduino-timer.h>
#include "Node.h"
#include "Grid.h"

#include<vector> 
#include<map> 
#include<algorithm> 
#include<string>

#define LED 13

#define turnSpeed 275
#define forwardSpeed 200
#define forwardDuration 4000 //change duration
#define turnLeftDuration 307
#define turnRightDuration 300

int trigPin = 10;
int echoPin = 9;
long objectDistance;
bool obstacleDetected = false;

//  ******** ZUMO ***********
ZumoMotors motors;
const int MAXSPEED = 400; // maximum speed of the Zumo motors
float motorSpeed;         // actual speed of the motors
Pushbutton button(ZUMO_BUTTON);

// ******** GRID *************
std::vector<Node> blacklist = {Node(0,3), Node(1,1), Node(7,6)};
Grid grid(0, 0, 7, 7, Node(7,7), blacklist, Grid::direction_facing::DOWN);

// ********** MOVE ***********
bool arrived = false;
bool move_decided = false;

// ******* DISTANCE ****************************
const float MAXDISTANCE = 50.0; // the size of one node so max the robot should go for one move
float distanceTravelled = 0.0;  // the distance travelled so far
float distanceReverse = 0.0;    // the distance it should go back to reach neutral position

// ********** TIME *******************
long timeStart;
long timeCurrent;
long timeTravelled;

//*********************************************************
void setup() {
    std::vector<Node> blacklist = {Node(0,3), Node(1,1), Node(7,6)};
    Grid grid(0, 0, 7, 7, Node(7,7), blacklist);
    pinMode(ledPin, HIGH);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

void loop() {
    if(arrived) {
        stop();
    }
    else {
        //std::cout << "Moving to: "<< grid.decide_move() << std::endl;
        if (!move_decided) {
            // stop on arrival
            if(grid.currentNode->status == Node::GOAL) {
                arrived=true;
            }
            grid.decide_move(move_decided);
            turn_if_necessary();
        }
        timeStart = millis();
        else {
            go_forward();
            if(distance_travelled >= 50) {
                stop();
                grid.finish_move(move_decided);
                timeTravelled = 0;
                distanceTravelled = 0;
            }
            // read sensor input
            // if sensor input -> detected 
            obstacle_detection()
            if(obstacleDetected)
                go_back();
                grid.add_approached_to_blacklist();
                move_decided = false;
        }

    }
}

void print_neighbours(std::vector<Node*> neighs) {
    //std::cout << "Neighbours: ";
    for (const auto& n : neighs) {
        //std::cout << n->print() << "   |   ";
    }
    //std::cout << std::endl;
}

// ****************** DETECTION ******************************
void get_object_distance()
{
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(5);
    pinMode(echoPin, INPUT);
    duration = pulseIn(echoPin, HIGH);
    objectDistance = (duration/2) * 0.0343;

    Serial.print(objectDistance);
    Serial.print("cm");
    Serial.println();
    delay(230);
}

void obstacle_detection() {
    get_object_distance();
    if(objectDistance < MAXDISTANCE) {
      obstacle = true;
    }
}


// ******************** MOVEMENT *************************************
void go_forward() {
    // Setting up speed
    int breakFactor = 2;                // use this to increase/decrease speed
    motorSpeed = MAXSPEED/breakFactor;  // setting motor speed

    // Moving forward
    motors.setSpeeds(motorSpeed, motorSpeed);

    // Calculating distance
    timeCurrent = millis();
    timeTravelled = timeCurrent - timeStart;
    distanceTravelled = (motorSpeed* timeTravelled)/1000;     
}

void go_back() {
   while (distanceTravelled > 0.0) {
    motors.setSpeeds(-motorSpeed, -motorSpeed);
    delay();
   }
    distance_travelled = 0;
    timeTravelled = 0;
    stop();
}

void turn_left(){
  motors.setSpeeds(-motorSpeed, motorSpeed);
  delay(turnLeftDuration);
  stop();
}

void turn_right(){
  motors.setSpeeds(motorSpeed, -motorSpeed);
  delay(turnRightDuration);
  stop();
}

void stop(){
  motors.setSpeeds(0,0);
  delay(300);
}


void turn_if_necessary() {
    if(grid.facing == Grid::direction_facing::DOWN) {
        // approached node below
        if(grid.approachedNode->x == grid.currentNode->x && grid.approachedNode->y > grid.currentNode->y) {
            return;
        // approached node to the right
        } else if (grid.approachedNode->x > grid.currentNode->x && grid.approachedNode->y == grid.currentNode->y) {
            turn_left();
            grid.facing = Grid::direction_facing::RIGHT;
            return;
        // approached node to the left
        } else if (grid.approachedNode->x < grid.currentNode->x && grid.approachedNode->y == grid.currentNode->y) {
            turn_right();
            grid.facing = Grid::direction_facing::LEFT;
            return;
        // approached node above
        } else if (grid.approachedNode->x == grid.currentNode->x && grid.approachedNode->y < grid.currentNode->y) {
            turn_left();
            turn_left();
            grid.facing = Grid::direction_facing::UP;
            return;
        }
    }
    if(grid.facing == Grid::direction_facing::RIGHT) {
        // approached node below
        if(grid.approachedNode->x == grid.currentNode->x && grid.approachedNode->y > grid.currentNode->y) {
            turn_right();
            grid.facing = Grid::direction_facing::DOWN;
            return;
        // approached node to the right
        } else if (grid.approachedNode->x > grid.currentNode->x && grid.approachedNode->y == grid.currentNode->y) {
            return;
        // approached node to the left
        } else if (grid.approachedNode->x < grid.currentNode->x && grid.approachedNode->y == grid.currentNode->y) {
            turn_right();
            turn_right();
            grid.facing = Grid::direction_facing::LEFT;
            return;
        // approached node above
        } else if (grid.approachedNode->x == grid.currentNode->x && grid.approachedNode->y < grid.currentNode->y) {
            turn_left();
            grid.facing = Grid::direction_facing::UP;
            return;
        }
    }
    if(grid.facing == Grid::direction_facing::UP) {
        // approached node below
        if(grid.approachedNode->x == grid.currentNode->x && grid.approachedNode->y > grid.currentNode->y) {
            turn_right();
            turn_right();
            grid.facing = Grid::direction_facing::DOWN;
            return;
        // approached node to the right
        } else if (grid.approachedNode->x > grid.currentNode->x && grid.approachedNode->y == grid.currentNode->y) {
            turn_right();
            grid.facing = Grid::direction_facing::RIGHT;
            return;
        // approached node to the left
        } else if (grid.approachedNode->x < grid.currentNode->x && grid.approachedNode->y == grid.currentNode->y) {
            turn_left();
            grid.facing = Grid::direction_facing::LEFT;
            return;
        // approached node above
        } else if (grid.approachedNode->x == grid.currentNode->x && grid.approachedNode->y < grid.currentNode->y) {
            return;
        }
    }
    if(grid.facing == Grid::direction_facing::LEFT) {
        // approached node below
        if(grid.approachedNode->x == grid.currentNode->x && grid.approachedNode->y > grid.currentNode->y) {
            turn_left();
            grid.facing = Grid::direction_facing::DOWN;
            return;
        // approached node to the right
        } else if (grid.approachedNode->x > grid.currentNode->x && grid.approachedNode->y == grid.currentNode->y) {
            turn_right();
            turn_right();
            grid.facing = Grid::direction_facing::RIGHT;
            return;
        // approached node to the left
        } else if (grid.approachedNode->x < grid.currentNode->x && grid.approachedNode->y == grid.currentNode->y) {
            return;
        // approached node above
        } else if (grid.approachedNode->x == grid.currentNode->x && grid.approachedNode->y < grid.currentNode->y) {
            turn_right();
            grid.facing = Grid::direction_facing::UP;
            return;
        }
    }
}


