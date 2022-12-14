#include <ArduinoSTL.h>
#include <ZumoShield.h>
#include <Wire.h>
#include "Node.h"
#include "Grid.h"

#include<vector> 
#include<map> 
#include<algorithm> 
#include<string>

// Constants
#define turnSpeed 275
#define forwardSpeed 200
#define turnLeftDuration 307
#define turnRightDuration 300

// Pins and Variables
int trigPin = 10;
int echoPin = 9;
int ledPin = 13;
long duration, distance_travelled, cm;
bool arrived = false;
bool move_decided = false;
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);

std::vector<Node> blacklist = {Node(0,3), Node(1,1), Node(7,6)};
Grid grid(0, 0, 7, 7, Node(7,7), blacklist, Grid::direction_facing::DOWN);


////////////////////////
// Movement Functions
////////////////////////

void go_forward() {
    // move forward
    // ++distance_travelled
}

void go_back() {
    // while (distance_travelled > 0) {
    //     // go back distance
    //     // --distance_travelled
    // }
    motors.setSpeeds(-forwardSpeed, -forwardSpeed);
    delay(duration);
    distance_travelled = 0;
    duration = 0;
    stop();
}

void turn_left(){
  motors.setSpeeds(-turnSpeed, turnSpeed);
  delay(turnLeftDuration);
  stop();
}

void turn_right(){
  motors.setSpeeds(turnSpeed, -turnSpeed);
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


////////////////////////
// Sensor Functions
////////////////////////

void get_distance()
{
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(5);
    pinMode(echoPin, INPUT);
    duration = pulseIn(echoPin, HIGH);
    cm = (duration/2) * 0.0343;

    Serial.print("in,");
    Serial.print(cm);
    Serial.print("cm");
    Serial.println();

    delay(230);
}

bool obstacle_detected() {
    get_distance();
    if(cm < 50) {
      return true;
    }
    return false;
}


////////////////////////
// Arduino Functions
////////////////////////

void setup() {
    pinMode(ledPin, HIGH);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

void update() {
 
}

void loop() {
    if(arrived) {
        // goal reached -> stop
        stop();
    }
    else {
        if (!move_decided) {
            // goal reached
            if(grid.currentNode->status == Node::GOAL) {
                arrived=true;
            } 
            // decide where to go and turn facing the next square
            else {
                grid.decide_move(move_decided);
                turn_if_necessary();
            }
        }
        else {
            // move forward and update travelled distance
            go_forward();
            
            // stop if arrived at next square
            if(distance_travelled >= 50) {
                stop();
                grid.finish_move(move_decided);
            }
            // go back to last square if obstacle is detected along the way
            if (obstacle_detected()) {
                go_back();
                grid.add_approached_to_blacklist();
                move_decided = false;
            }
        }

    }
}
