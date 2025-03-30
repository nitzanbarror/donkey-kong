#include "Barrel.h"

/* This method determines the barrel's horizontal direction based on
the type of floor character it is currently positioned on.
It updates the direction accordingly, allowing the barrel to move left or right. */
void Barrel::setDir() {
	int currX = _position.getX();
	int currY = _position.getY();
	Point floorChar = { currX, currY + 1 };

	if (pBoard->getChar(floorChar) == (char)FloorCharacters::LEFT) {
		_dirX = (int)Direction::NEGATIVE;
		prevFloorChar = (char)FloorCharacters::LEFT; // store previous floor character
	}
	else if (pBoard->getChar(floorChar) == (char)FloorCharacters::RIGHT) {
		_dirX = (int)Direction::POSITIVE;
		prevFloorChar = (char)FloorCharacters::RIGHT;
	}
	// handle previous floor direction if current floor is '='
	else if (isKeepingSameDir(floorChar)) {
		if (prevFloorChar == (char)FloorCharacters::LEFT) {
			_dirX = (int)Direction::NEGATIVE;
		}
		else {
			_dirX = (int)Direction::POSITIVE;
		}
	}
	// reset fall counter if barrel is on the floor
	if (pBoard->isOnFloor(_position)) {
		counterFall = 0;
	}
}

/* This method checks if the barrel should maintain its current direction.
It returns true if the current floor character matches conditions that
indicate no direction change is needed. */
bool Barrel:: isKeepingSameDir(Point floorChar) {
	return (
		pBoard->getChar(floorChar) == (char)FloorCharacters::PREV || // Continue previous direction
		pBoard->getChar(floorChar) == (char)Characters::WALL ||     // Maintain direction against a wall
		pBoard->getChar(floorChar) == (char)Characters::GHOST ||    // Maintain direction for ghost character
		pBoard->getChar(floorChar) == (char)Characters::HAMMER      // Maintain direction for hammer character
		);
}

/* This method moves the barrel to its next position based on its
current direction. It checks if the next position is valid before moving. */
void Barrel::move() {
	int nextX = _position.getX() + _dirX;
	int nextY = _position.getY() + _dirY;
	Point nextPos = { nextX, nextY };

	// move barrel if next position is valid
	if (pBoard->isValidPosition(nextPos)) {
		_position.setX(nextX); // update position horizontally
	}

	// handle falling
	freeFalling();
}

/* This method implements the free-falling mechanics for the barrel.
If the barrel is in mid-air, it will fall down one unit and increment
the fall counter. */
void Barrel::freeFalling() {
	int currX = _position.getX();
	int currY = _position.getY();
	Point nextPos = { currX, currY + 1 };

	_dirX = (int)Direction::STAY; // stop horizontal movement while falling
	if (pBoard->isInAir(_position)) {
		_position.setY(currY + 1);
		counterFall++;
	}
}

/* This method checks whether the barrel should become inactive.
A barrel becomes inactive if it has fallen for a certain duration
or has moved out of board boundaries. */
bool Barrel::getActiveStatus() {
	int currX = _position.getX();
	int currY = _position.getY();
	Point RnextPos = { currX + 1, currY }; // check next position
	Point LnextPos = { currX - 1, currY };

	// inactivate barrel based on falling duration or boundaries check
	if (currY == (int)Boundaries::MAX_Y - 1 || (counterFall >= 8 && pBoard->isOnFloor(_position)) || isExplode || !pBoard->isInBoundaries(RnextPos) || !pBoard->isInBoundaries(LnextPos)) {
		isActive = false;
	}
	return isActive;
}

/* This method returns whether the barrel has exploded. 
*/
bool Barrel::isBarrelExplode() const {
	return isExplode;
}

/* This method adds a new barrel to the array at a specific position
based on its index in the log. The initial position alternates between
left and right. */
void Barrel::setPosition(const Point& other) {
	_position.setPosition(other);
	isActive = true;
}

/* This method resets the barrel to its initial state by clearing its
position, resetting counters, and marking it as inactive and not exploded. */
void Barrel::setBarrel() {
	_position = {};
	counterFall = 0;
	isActive = false;
	isExplode = false;
	_dirX = 0;
	_dirY = 0;
}

/* Restores the background character at the barrel's position, 
replacing specific characters with AIR if needed. */
void Barrel::erase() const {
	char backgroundChar = pBoard->getChar(_position); // restore the character at the barrel's current position
	if (backgroundChar == (char)Characters::BIG_GHOST || backgroundChar == (char)Characters::GHOST || backgroundChar == (char)Characters::MARIO)
		draw((char)Characters::AIR);
	else
		draw(backgroundChar);
}

/* This method returns the current position of the barrel as a Point object. 
*/
Point Barrel::getPosition() const {
	return _position;
}

/* This method marks the barrel as exploded by erasing its current position
and updating its explosion status. */
void Barrel::explosion() {
	erase();
	isExplode = true; // mark as exploded
}

/* This method resets the explosion status of the barrel to indicate
that it is no longer in an exploded state. */
void Barrel::setExplodeStatus() {
	isExplode = false;
}
