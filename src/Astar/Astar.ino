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

//#define turnSpeed 275
//#define forwardSpeed 200
//#define forwardDuration 4000 //change duration
//#define turnLeftDuration 307
//#define turnRightDuration 300

int trigPin = 10;
int echoPin = 9;
long duration, distance_travelled;

ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);

std::vector<Node> blacklist = {Node(0,3), Node(1,1), Node(7,6)};
Grid grid(0, 0, 7, 7, Node(7,7), blacklist, Grid::direction_facing::DOWN);

bool arrived = false;
bool move_decided = false;

void print_neighbours(std::vector<Node*> neighs) {
    //std::cout << "Neighbours: ";
    for (const auto& n : neighs) {
        //std::cout << n->print() << "   |   ";
    }
    //std::cout << std::endl;
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

void go_forward() {
    // move forward
    // ++distance_travelled
}

void get_distance()
{
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(5);
    pinMode(echoPin, INPUT);
    duration = pulseIn(echoPin, HIGH);
    distance_travelled = (duration/2) * 0.0343;

    Serial.print("in,");
    Serial.print(cm);
    Serial.print("cm");
    Serial.println();

    delay(230);
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

void turn_if_necessary() {
    //
}

void setup() {
    std::vector<Node> blacklist = {Node(0,3), Node(1,1), Node(7,6)};
    Grid grid(0, 0, 7, 7, Node(7,7), blacklist);
}

void update() {
 
}

void loop() {
    if(arrived) {
        //stop
    }
    else {
        //std::cout << "Moving to: "<< grid.decide_move() << std::endl;
        if (!move_decided) {
            grid.decide_move(move_decided);
            turn_if_necessary();
        }
        else {
            go_forward();
            get_distance();
            if(distance_travelled >= 50) {
                // stop running
                grid.finish_move(move_decided);
            }
            // read sensor input
            // if sensor input -> detected
                go_back();
                grid.add_approached_to_blacklist();
                move_decided = false;
        }




        // start moving
        // keep track of distance
        // if object detected
            // go back distance travelled
            // restart decide_move()

        //grid.finish_move();

        if(grid.currentNode->status == Node::GOAL) {
            arrived=true;
        }
    }
}
