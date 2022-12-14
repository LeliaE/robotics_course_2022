#ifndef NODE
#define NODE
//#include "libraries/avr-stl-1.1.1/avr-stl/include/pnew.cpp"
//#include<pnew.cpp>
//#include <string>

class Node {

    public:
        enum status_enum  {CLEAR=0, OBSTACLE=1, GOAL=99};

        const int x;
        const int y;
        status_enum status;

        Node(int x, int y, status_enum status = status_enum::CLEAR) : x(x), y(y), status(status) {};

        /*std::string print() {
            std::string str;
            str += std::to_string(x);
            str += ",";
            str += std::to_string(y);
            return str;
        }*/

        bool is_clear() {
            return status == CLEAR;
        }

        bool is_goal() {
            return status == GOAL;
        }

        bool is_clear_or_goal() {
            return status == CLEAR || status == GOAL;
        }
        
        bool is_node(Node other) {
            return (this->x == other.x && this->y == other.y);
        }
};

#endif
