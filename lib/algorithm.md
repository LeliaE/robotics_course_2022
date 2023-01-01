* CHECK RAW FOR PROPER VIEW *

variables:
NODE: x position, y position, status = {clear, target, obstacle}
GRID: height, width, blacklist of obstacles
ZUMO: speed, duration of move, sensor detection

pseudocode:
FOR ALL nodes in grid:
  setup node with x,y, and status 

WHILE (target not found):
  IF(move not decided):
    check current node
    IF(current node == target):
      STOP
    ELSE:
      FOR ALL neighbor nodes:
	check straight path for obstacles
        compute heuristic for clearest and shortest path
      head towards node with smallest heuristic
      move = decided
      check if obstacle has not appeared
      IF(new obstacle):
	move = not decided
        head back equal direction and head towards node with next smallest heuristic
	add node to blacklist
      ELSE:
	complete move to node
