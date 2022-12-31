#include "ArduinoSTL.h"


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

#define turnSpeed 275
#define forwardSpeed 200
#define turnLeftDuration 307
#define turnRightDuration 300

//  ******** PINS & VARIABLES ***********
int trigPin = 9;
int echoPin = 10;
long duration, cm, inches;
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
float distanceTravelledBackwards = 0.0; 
float distanceReverse = 0.0;    // the distance it should go back to reach neutral position

// ********** TIME *******************
long timeStart;
long timeCurrent;
long timeTravelled;
long timeStop;


////////////////////////
// Functions
////////////////////////

// ****************** DETECTION ******************************
long microsecondsToInches(long microseconds) {
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds){
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  return microseconds / 29 / 2;
}

void get_object_distance()
{
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
  
    duration = pulseIn(echoPin, HIGH);
  
    // convert the time into a distance
    Serial.print(duration);
    inches = microsecondsToInches(duration);
    cm = microsecondsToCentimeters(duration);
    
    Serial.print(inches);
    Serial.print("in, ");
    Serial.print(cm);
    Serial.print("cm");
    Serial.println();
    
    delay(500);
}

void obstacle_detection() {
    get_object_distance();
    if(objectDistance < MAXDISTANCE) {
      obstacleDetected = true;
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

    timeStop = millis();
    bool go_backwards = true;
    while (go_backwards) {
      timeCurrent = millis();
      timeTravelled = timeCurrent - timeStop;
      distanceTravelledBackwards = (motorSpeed* timeTravelled)/1000; 
      if (distanceTravelledBackwards >= distanceTravelled ){
        go_backwards = false;
      }
      motors.setSpeeds(-motorSpeed, -motorSpeed);
    }
    distanceTravelledBackwards = 0;
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



////////////////////////
// Arduino Functions
////////////////////////

void setup() {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    Serial.begin(9600);
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
            grid.decide_move(move_decided);
            turn_if_necessary();
            timeStart = millis();
        }
        else {
            go_forward();

            // stop if arrived at next square
            if(distanceTravelled >= 50) {
                stop();
                grid.finish_move(move_decided);
                timeTravelled = 0;
                distanceTravelled = 0;
            }

            // read sensor input
            obstacle_detection();
            // go back to last square if obstacle is detected along the way
            if(obstacleDetected)
                go_back();
                grid.add_approached_to_blacklist();
                move_decided = false;
                obstacleDetected = false;
        }

    }
}
