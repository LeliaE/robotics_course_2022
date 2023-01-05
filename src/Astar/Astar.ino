/*
  Program name: A* grid search program for Arduino Zumo
  Authors: Amelie Schneider (C++ master), Lelia Erscoi (algorithm architect),
           Hritika Kathuria (object detection technician)
  Date created: 08-12-2022
  Last updated: 05-01-2022
  Description: A program to allow a Zumo robot to navigate a grid by finding the
               next best move. Actively checks the path for obstacles to add the
               corresponding node to the blacklist. Program runs until finding the
               target node.
*/

////////////////////////
// Include and Define
////////////////////////

#include "ArduinoSTL.h"
#include "ZumoShield.h"
#include "Wire.h"
#include "Node.h"
#include "Grid.h"

#include "vector" 
#include "map" 
#include "algorithm" 
#include "string"

#define TURN_SPEED 275
#define TURN_LEFT_DURATION 400
#define TURN_RIGHT_DURATION 300

//  ******** PINS & VARIABLES ***********
int trigPin = 9;
int echoPin = 10;
long duration, cm, inches;
long objectDistance;
bool obstacleDetected = false;

//  ******** ZUMO ***********
ZumoMotors motors;
const int MAX_SPEED = 400;              // maximum speed of the Zumo motors
const float MOTOR_SPEED = MAX_SPEED/2;         // actual speed of the motors
Pushbutton button(ZUMO_BUTTON);

// ******** GRID *************
std::vector<Node> blacklist = {Node(0,4), Node(0,7), Node(1,3), Node(2,5), Node(3,0)
                              Node(3,2), Node(4,0), Node(4,2), Node(4,4), Node(5,6),
                              Node(6,2), Node(6,4)};
Grid grid(0, 0, 7, 7, Node(7,7), blacklist, Grid::DIRECTION_FACING::DOWN);

// ********** MOVE ***********
bool arrived = false;
bool moveDecided = false;

// ******* DISTANCE ****************************
const float MAX_DISTANCE = 50.0; // the size of one node so max the robot should go for one move
float distanceTravelled = 0.0;  // the distance travelled so far
float distanceTravelledBackwards = 0.0; 

// ********** TIME *******************
unsigned long timeStart;
long timeCurrent;
long timeTravelled;
long timeStop;

////////////////////////
// Functions
////////////////////////

// ****************** DETECTION ******************************
long microseconds_to_inches(long microseconds) {
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return (microseconds / 74 / 2);
}

long microseconds_to_centimeters(long microseconds){
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  return (microseconds / 29  2);
}

void get_object_distance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);

  // convert the time into a distance
 // Serial.print(duration);
 // Serial.print("...");
  // inches = microseconds_to_inches(duration);
  cm = microseconds_to_centimeters(duration);
  
  Serial.print(duration);
  Serial.print("duration, ");
  Serial.print(cm);
  Serial.print("cm");
 Serial.println();
}

void obstacle_detection() {
  get_object_distance();
  if(objectDistance < MAX_DISTANCE && objectDistance != 0) {
    obstacleDetected = true;
  }
}


// ******************** MOVEMENT *************************************
void go_forward() {
  // Moving forward
  motors.setSpeeds(MOTOR_SPEED, MOTOR_SPEED);

  // Calculating distance
  timeCurrent = millis();
  timeTravelled = timeCurrent - timeStart;
  distanceTravelled = (MOTOR_SPEED * timeTravelled)/1000;     
}

void go_back() {
  timeStop = millis();      //records time since start of reverse
  bool goBackwards = true;

  while (goBackwards) {
    timeCurrent = millis();
    timeTravelled = timeCurrent - timeStop;
    distanceTravelledBackwards = (MOTOR_SPEED * timeTravelled)/1000; 
    if (distanceTravelledBackwards >= distanceTravelled) {
      goBackwards = false;
    }
    motors.setSpeeds(-MOTOR_SPEED, -MOTOR_SPEED);
  }
  distanceTravelledBackwards = 0;
  stop();
}

void turn_left() {
  motors.setSpeeds(-MOTOR_SPEED, MOTOR_SPEED*2);
  //delay(TURN_LEFT_DURATION);
  stop();
}

void turn_right() {
  motors.setSpeeds(MOTOR_SPEED, -MOTOR_SPEED);
  delay(TURN_RIGHT_DURATION);
  stop();
}

void stop() {
  motors.setSpeeds(0,0);
  delay(300);
}

void turn_if_necessary() {
  if(grid.facing == Grid::DIRECTION_FACING::DOWN) {
    // approached node below
    if(grid.approachedNode->x == grid.currentNode->x && grid.approachedNode->y > grid.currentNode->y) {
        return;
    // approached node to the right
    } else if (grid.approachedNode->x > grid.currentNode->x && grid.approachedNode->y == grid.currentNode->y) {
        turn_left();
        grid.facing = Grid::DIRECTION_FACING::RIGHT;
        return;
    // approached node to the left
    } else if (grid.approachedNode->x < grid.currentNode->x && grid.approachedNode->y == grid.currentNode->y) {
        turn_right();
        grid.facing = Grid::DIRECTION_FACING::LEFT;
        return;
    // approached node above
    } else if (grid.approachedNode->x == grid.currentNode->x && grid.approachedNode->y < grid.currentNode->y) {
        turn_left();
        turn_left();
        grid.facing = Grid::DIRECTION_FACING::UP;
        return;
    }
  }
  if(grid.facing == Grid::DIRECTION_FACING::RIGHT) {
    // approached node below
    if(grid.approachedNode->x == grid.currentNode->x && grid.approachedNode->y > grid.currentNode->y) {
        turn_right();
        grid.facing = Grid::DIRECTION_FACING::DOWN;
        return;
    // approached node to the right
    } else if (grid.approachedNode->x > grid.currentNode->x && grid.approachedNode->y == grid.currentNode->y) {
        return;
    // approached node to the left
    } else if (grid.approachedNode->x < grid.currentNode->x && grid.approachedNode->y == grid.currentNode->y) {
        turn_right();
        turn_right();
        grid.facing = Grid::DIRECTION_FACING::LEFT;
        return;
    // approached node above
    } else if (grid.approachedNode->x == grid.currentNode->x && grid.approachedNode->y < grid.currentNode->y) {
        turn_left();
        grid.facing = Grid::DIRECTION_FACING::UP;
        return;
    }
  }
  if(grid.facing == Grid::DIRECTION_FACING::UP) {
    // approached node below
    if(grid.approachedNode->x == grid.currentNode->x && grid.approachedNode->y > grid.currentNode->y) {
        turn_right();
        turn_right();
        grid.facing = Grid::DIRECTION_FACING::DOWN;
        return;
    // approached node to the right
    } else if (grid.approachedNode->x > grid.currentNode->x && grid.approachedNode->y == grid.currentNode->y) {
        turn_right();
        grid.facing = Grid::DIRECTION_FACING::RIGHT;
        return;
    // approached node to the left
    } else if (grid.approachedNode->x < grid.currentNode->x && grid.approachedNode->y == grid.currentNode->y) {
        turn_left();
        grid.facing = Grid::DIRECTION_FACING::LEFT;
        return;
    // approached node above
    } else if (grid.approachedNode->x == grid.currentNode->x && grid.approachedNode->y < grid.currentNode->y) {
        return;
    }
  }
  if(grid.facing == Grid::DIRECTION_FACING::LEFT) {
    // approached node below
    if(grid.approachedNode->x == grid.currentNode->x && grid.approachedNode->y > grid.currentNode->y) {
        turn_left();
        grid.facing = Grid::DIRECTION_FACING::DOWN;
        return;
    // approached node to the right
    } else if (grid.approachedNode->x > grid.currentNode->x && grid.approachedNode->y == grid.currentNode->y) {
        turn_right();
        turn_right();
        grid.facing = Grid::DIRECTION_FACING::RIGHT;
        return;
    // approached node to the left
    } else if (grid.approachedNode->x < grid.currentNode->x && grid.approachedNode->y == grid.currentNode->y) {
        return;
    // approached node above
    } else if (grid.approachedNode->x == grid.currentNode->x && grid.approachedNode->y < grid.currentNode->y) {
        turn_right();
        grid.facing = Grid::DIRECTION_FACING::UP;
        return;
    }
  }
}

////////////////////////
// Arduino Functions
////////////////////////

void setup() {
  //Serial.print("Initializing...");
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);     
}

void loop() {
  obstacle_detection();
  // if(arrived) {
  //     // goal reached -> stop
  //     //Serial.print("Target reached!");
  //     stop();
  // }
  // else {
  //   if (!moveDecided) {
  //     // goal reached
  //     if(grid.currentNode->status == Node::GOAL) {
  //         arrived=true;
  //     }
  //     // decide where to go and turn facing the next square
  //     grid.decide_move(moveDecided);
  //     turn_if_necessary();
  //     timeStart = millis();
  //     //Serial.print("testtest");
  //   }
  //   else {
  //     go_forward();
  //     Serial.print("going forward!");

  //     // stop if arrived at next square
  //     if(distanceTravelled >= 50) {
  //       stop();
  //       Serial.println("Move has been decided.s");
  //       grid.finish_move(moveDecided);

  //       // Checking current coordinates
  //       //Serial.print("X: ");
  //       //Serial.print(grid.get_current_x());
  //       //Serial.print("y: ");
  //       //Serial.print(grid.get_current_y());
      
  //       timeTravelled = 0;
  //       distanceTravelled = 0;
  //     }
  //     // read sensor input
  //     obstacle_detection();
  //     // go back to last square if obstacle is detected along the way
  //     if(obstacleDetected){
  //         go_back();
  //         grid.add_approached_to_blacklist();
  //         moveDecided = false;
  //         obstacleDetected = false;
  //         //Serial.print("An obstacle was added to the blacklist.");
  //     }
  //   }
  // }
}