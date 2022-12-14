
////////////////////////
// Include and Define
////////////////////////

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
int trigPin = 13;
int echoPin = 12;
long duration, distance_travelled, cm, inches;
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

long microsecondsToInches(long microseconds) {
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds){
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  return microseconds / 29 / 2;
}

void get_distance()
{
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
  
    duration = pulseIn(echoPin, HIGH);
  
    // convert the time into a distance
    inches = microsecondsToInches(duration);
    cm = microsecondsToCentimeters(duration);
    
    Serial.print(inches);
    Serial.print("in, ");
    Serial.print(cm);
    Serial.print("cm");
    Serial.println();
    
    delay(500);
}

bool obstacle_detected() {
    get_distance();
    if(cm < 50 && cm != 0) {
      return true;
    }
    return false;
}


////////////////////////
// Arduino Functions
////////////////////////

void setup() {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    Serial.begin(9600);
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
            Serial.println("Hello");
            get_distance();
            
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
