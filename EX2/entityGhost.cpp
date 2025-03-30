#include "entityGhost.h"

/* Erases the visual representation of the object from the board.
   Restores the character that was present at the object's position before it was drawn.
   If the character is a ghost or Mario, it replaces it with an empty space. */
void entityGhost::erase() const {

	char backgroundChar = pBoard->getChar(_position); // restore the character at the barrel's current position
	if (backgroundChar == (char)Characters::BIG_GHOST || backgroundChar == (char)Characters::GHOST || backgroundChar == (char)Characters::MARIO)
		draw((char)Characters::AIR);
	else
		draw(backgroundChar);
}

/* Returns the current position of the object as a Point. 
*/
Point entityGhost::getPosition() const {
	return _position;
}

/* Handles the object's explosion by erasing it from the board
   and marking it as exploded. */
void entityGhost::explosion() {
	erase();
	isExplode = true; // mark as exploded
}

/* Resets the explosion status of the object to indicate it is no longer exploded. 
*/
void entityGhost::setExplodeStatus() {
	isExplode = false;
}

/* Sets the position of the ghost entity to the provided position. 
*/
void entityGhost::setPosition(const Point& other) {
	_position.setPosition(other);
}

/* Sets the ghost entity's active status to true. 
*/
void entityGhost::setActiveStatus() {
	isActive = true;
}

/* Sets the collision status of the ghost entity. 
*/
void entityGhost::setCollision(bool status) {
	isColliding = status;
}

/* Gets the active status of the ghost.
   If the ghost is exploded, it will be set to inactive. */
bool entityGhost::getActiveStatus() {
	if (isExplode) {
		isActive = false;
	}
	return isActive;
}

/* Gets the current X direction of the ghost. 
*/
int entityGhost::getDirX() {
	return _dirX;
}

/* Randomly changes the X direction of the ghost with a 5% chance. 
*/
void entityGhost::randDirX() {
	
	
	/*if ((double)rand() / RAND_MAX >= 0.95) {
		_dirX = -_dirX;
	}*/
}

/* Sets the X direction of the ghost based on its current position and other conditions. 
*/
void entityGhost::setDirX() {
	int currX = _position.getX();
	int currY = _position.getY();
	Point nextPos = { currX + _dirX, currY };
	
	// If the ghost is at the bottom boundary, randomize its X direction
	if (currY == (int)Boundaries::MAX_Y - 1) {
		randDirX();
	}
	// If the ghost is colliding or the next position is not a valid floor, reverse the X direction
	else if (isColliding == true || !pBoard->isOnFloor(nextPos) || !pBoard->isValidPosition(nextPos)) {
		_dirX = -_dirX;
	}
	else {
		randDirX();
	}
}