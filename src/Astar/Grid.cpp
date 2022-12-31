#include "Grid.h"

Grid::Grid(int x, int y, int width, int height, Node goal, std::vector<Node> blacklist, DIRECTION_FACING facing) : 
  minX(x), minY(y), maxX(width), maxY(height), blacklist(blacklist), facing(facing) {
          
  for (int w = minX; w <= maxX; ++w) {
    for (int h = minY; h <= maxY; ++h) {
      Node *node = new Node(w,h);
      nodes.push_back(node);

      // define goal
      if(w == goal.x && h == goal.y) {
        node->status = Node::GOAL;
      }

      // define blacklisted nodes
      for (const auto& b : blacklist) { 
        if(node->is_node(b)) {
          node->status = Node::OBSTACLE;
        }
      }
    }
  }
  currentNode = this->at(minX, minY);
}

Node * Grid::at(int x, int y) {
  if(x > maxX || y > maxY) {
      // throw std::exception
  }

  for (const auto& node : nodes) {
    if(node->x == x && node->y == y) {
      return node;
    }
  }
  // throw std::exception
}

int Grid::get_current_x() {
  return currentNode->x;
}

int Grid::get_current_y(){
  return currentNode->y;
}

void Grid::add_approached_to_blacklist() {
  // Node* blacklisted = this->at(x,y);
  for (const auto& node : nodes) {
    if(node->is_node(*approachedNode)) {
      node->status = Node::OBSTACLE;
      break;
    }
  }
  this->blacklist.push_back(Node(approachedNode->x,approachedNode->y));
}

std::vector<Node*> Grid::get_neighbours() {
  std::vector<Node*> neighbours;

  // right
  if(currentNode->x+1 <= maxX) {
    Node* contestant = this->at(currentNode->x+1,currentNode->y);
    if(contestant->is_clear_or_goal()) { 
      neighbours.push_back(contestant);
    }
  }

  // left
  if(currentNode->x-1 >= minX) {
    Node* contestant = this->at(currentNode->x-1,currentNode->y);
    if(contestant->is_clear_or_goal()) { 
      neighbours.push_back(contestant);
    }
  }

  // bottom
  if(currentNode->y-1 >= minY) {
    Node* contestant = this->at(currentNode->x,currentNode->y-1);
    if(contestant->is_clear_or_goal()) { 
      neighbours.push_back(contestant);
    }
  }

  // top
  if(currentNode->y+1 <= maxY) {
    Node* contestant = this->at(currentNode->x,currentNode->y+1);
    if(contestant->is_clear_or_goal()) { 
      neighbours.push_back(contestant);
    }
  }
  return neighbours;
}

void Grid::update_distances_for_neighbours() {
  std::map<Node*, int> distances;
  for (const auto& neighbour : this->get_neighbours()) {
    // first up -> second right
    int heuristic_up = 0;

    // calculate going up path
    for(int x = neighbour->x; x <= maxX; x++) {
      if(this->at(x,neighbour->y)->is_clear_or_goal()) {
        heuristic_up += 1;
      } else {
          heuristic_up += 100;
      }
    }

    // calculate going right path
    for(int y = neighbour->y; y <= maxY; y++) {
      if(this->at(maxX,y)->is_clear_or_goal()) {
        heuristic_up += 1;
      } else {
          heuristic_up += 100;
      }
    }

    // first right -> second up
    int heuristic_right = 0;

    // calculate going right path
    for(int y = neighbour->y; y <= maxY; y++) {
      if(this->at(neighbour->x,y)->is_clear_or_goal()) {
        heuristic_right += 1;
      } else {
          heuristic_right += 100;
      }
    }

    // calculate going up path
    for(int x = neighbour->x; x <= maxX; x++) {
      if(this->at(x,maxY)->is_clear_or_goal()) {
        heuristic_right += 1;
      } else {
          heuristic_right += 100;
      }
    }
    int heuristic = std::min(heuristic_up, heuristic_right);
    distances[neighbour] = heuristic;
  }
  neighboursHeuristics = distances;
}

void Grid::decide_move(bool& move_decided) {
  update_distances_for_neighbours();

  auto it = min_element(neighboursHeuristics.begin(), neighboursHeuristics.end(),
      [](decltype(neighboursHeuristics)::value_type& l, decltype(neighboursHeuristics)::value_type& r) { return l.second < r.second; });

  approachedNode = it->first;
  move_decided = true;
}

void Grid::finish_move(bool& move_decided) {
  currentNode = approachedNode;
  move_decided = false;
}