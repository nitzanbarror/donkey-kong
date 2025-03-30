#include "climbingGhost.h"
#include <cmath>

/* Moves the big ghost by handling climbing actions and horizontal movement.
*/
void climbingGhost::move() {
	handleDown();
	handleUp();
	entityGhost::setDirX();

	if (isClimbingUp) {
		climbUp();
	}
	else if (isClimbingDown) {
		climbDown();
	}
	else {
		prevDirX = _dirX;
	}

	int nextX = _position.getX() + _dirX;
	_position.setPosition({ nextX, _position.getY() });
}

/* Handles upward climbing logic if the big ghost is on or near a ladder.
   The counter ensures climbing occurs only after a certain delay (every 4 iterations). */
void climbingGhost::handleUp() {
	if (pBoard->isLadder(_position) && !isClimbingDown) {
		if (!isClimbingUp)
			counter++; // Increment the counter if not already climbing up
		if (counter % 4 == 0) { // Trigger climbing every 4th iteration
			isClimbingUp = true;
		}
	}
}

/* Executes upward climbing movement and adjusts direction as necessary. 
*/
void climbingGhost::climbUp() {
	_dirY = (int)Direction::NEGATIVE;
	_dirX = (int)Direction::STAY;
	int nextY = _position.getY() + _dirY;

	if (pBoard->getChar(_position) == (char)Characters::AIR) {
		_dirY = (int)Direction::STAY;
		_dirX = prevDirX;
		isClimbingUp = false;
	}
	else {
		_position.setY(nextY);
	}
}

/* Handles downward climbing logic if the big ghost is on or near a ladder.
   The counter ensures climbing occurs only after a certain delay (every 4 iterations). */
void climbingGhost::handleDown() {
	int currX = _position.getX();
	int currY = _position.getY();
	Point nextPos = { currX, currY + 1 };
	_dirY = (int)Direction::POSITIVE;

	if ((pBoard->isLadder(Point(_position.getX(), _position.getY() + 2)) || pBoard->isLadder(nextPos)) && !isClimbingUp) {
		if (!isClimbingDown)
			counter++; // Increment the counter if not already climbing down
		if (counter % 4 == 0) { // Trigger climbing every 4th iteration
			isClimbingDown = true;
		}
	}
}

/* Executes downward climbing movement and adjusts direction as necessary. 
*/
void climbingGhost::climbDown() {
	_dirY = (int)Direction::POSITIVE;
	_dirX = (int)Direction::STAY;
	
	int currX = _position.getX();
	int currY = _position.getY();
	int nextY = currY + _dirY;
	Point nextPos = { currX, nextY };
	Point underLadderPos = { currX, nextY + 1 };


	if (pBoard->isOnFloor(_position) && pBoard->isLadder(underLadderPos)) {
		_position.setY(nextY);
	}
	else if (pBoard->isValidPosition(nextPos)) {
		_position.setY(nextY);
	}
	else {
		_dirY = (int)Direction::STAY;
		_dirX = prevDirX;
		isClimbingDown = false;
	}
}