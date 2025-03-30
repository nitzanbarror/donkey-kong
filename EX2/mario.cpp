#include "mario.h"

/* This method erases Mario from the game board by restoring the background character. 
*/
void Mario::erase() const {
	char backgroundChar = pBoard->getChar(_position); // restore the character at the player's current position
	if (backgroundChar == (char)Characters::MARIO || backgroundChar == (char)Characters::GHOST || backgroundChar == (char)Characters::HAMMER)
		draw((char)Characters::AIR);
	else
		draw(backgroundChar);
}

/* This method sets Mario's direction based on the input key. 
*/
void Mario::setDir(Keys key) {
	switch (key) {
	case Keys::LEFT:
		handleHorizontal(Keys::LEFT);
		break;
	case Keys::RIGHT:
		handleHorizontal(Keys::RIGHT);
		break;
	case Keys::UP:
		handleUp();
		break;
	case Keys::DOWN:
		handleDown();
		break;
	case Keys::STAY:
		handleStay();
		break;
	default:
		break;
	}
}

/* This method moves Mario based on his current state (climbing, jumping, falling, etc.). 
*/
void Mario::move() {

	if (isClimbingUp) {
		climbUp();
	}
	else if (isJumping && !isFalling) {
		jump();
	}
	else if (isClimbingDown) {
		climbDown();
	}
	else {
		moveHorizontal();
	}

	isMarioInAir();
	if (isFalling) {
		freeFalling();
	}
	else {
		fallingCounter = 0;
	}
}

/*
This method handles Mario's upward movement, including climbing and jumping.
*/
void Mario::handleUp() {

	if (!isFalling) {
		_dirY = (int)Direction::NEGATIVE;

		if ((pBoard->isLadder(_position) && !isJumping) || isClimbingUp) {
			isClimbingUp = true;
			_dirX = (int)Direction::STAY;
		}
		else {
			isJumping = true;
		}
	}
}

/*
This method handles Mario climbing up a ladder.
*/
void Mario::climbUp() {
	int nextY = _position.getY() + _dirY;

	if (pBoard->getChar(_position) == (char)Characters::AIR) {
		_dirY = (int)Direction::STAY;
		isClimbingUp = false;
	}
	else {
		_position.setY(nextY);

	}
}

/*
This method handles Mario's jumping action.
*/
void Mario::jump() {
	int nextX = _position.getX() + _dirX;
	int nextY = _position.getY() + _dirY;
	Point nextPos = { nextX, nextY };

	if (jumpingCounter < 2) {
		if (pBoard->isValidPosition(nextPos)) {
			_position.setPosition(nextPos);
		}
		jumpingCounter++;
	}
	else {
		isJumping = false;
		jumpingCounter = 0;
		_dirY = (int)Direction::STAY;
		isFalling = true;
	}
}

/*
This method handles Mario's downward movement, including climbing down ladders.
*/
void Mario::handleDown() {
	int currX = _position.getX();
	int currY = _position.getY();
	Point nextPos = { currX, currY + 1 };
	_dirY = (int)Direction::POSITIVE;

	if (pBoard->isLadder(Point(_position.getX(), _position.getY() + 2)) || pBoard->isLadder(nextPos)) {
		isClimbingDown = true;
		isClimbingUp = false;

	}
	else {
		_dirX = _dirY = (int)Direction::STAY;
	}
}

/*
This method handles Mario climbing down a ladder.
*/
void Mario::climbDown() {
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
		isClimbingDown = false;
	}
}

/*
This method handles Mario staying in place.
*/
void Mario::handleStay() {
	isClimbingUp = false;
	isClimbingDown = false;

	if (pBoard->isValidPosition(_position)) {
		_dirX = (int)Direction::STAY;
		_dirY = (int)Direction::STAY;
	}
}

/*
This method handles Mario's horizontal movement.
*/
void Mario::handleHorizontal(Keys key) {
	if (!isClimbingUp && !isClimbingDown) {
		if (key == Keys::LEFT) {
			_dirX = (int)Direction::NEGATIVE;
		}
		else {
			_dirX = (int)Direction::POSITIVE;
		}
		_dirY = (int)Direction::STAY;
	}
}

/*
This method moves Mario horizontally.
*/
void Mario::moveHorizontal() {
	int nextX = _position.getX() + _dirX;
	int nextY = _position.getY() + _dirY;
	Point nextPos = { nextX, nextY };

	if (pBoard->isValidPosition(nextPos)) {
		_position.setPosition(nextPos);
	}
	else {
		_dirX = _dirY = (int)Direction::STAY;
	}
}

/*
This method checks if Mario is in the air.
*/
void Mario::isMarioInAir() {

	if (pBoard->isInAir(_position) && !isJumping) {
		isFalling = true;
	}
}

/*
This method sets Mario's position.
*/
void Mario::setPosition(const Point& other) {
	_position.setPosition(other);
}

/*
This method returns Mario's current position.
*/
Point Mario::getPosition() const {
	return _position;
}

/*
This method handles Mario's free falling.
*/
void Mario::freeFalling() {
	int currX = _position.getX();
	int currY = _position.getY();
	Point currPos = { currX, currY + 1 };

	if (pBoard->isValidPosition(currPos)) {
		_position.setY(currY + 1);
		fallingCounter++;
	}
	else {
		isFalling = false;
	}
}

/*
This method checks if Mario's fall is deadly.
*/
bool Mario::isFallDeadly() {
	if (fallingCounter >= 5 && (pBoard->isOnFloor(_position) || _position.getY() + 1 == (int)Boundaries::MAX_Y)) {
		fallingCounter = 0;
		return true;
	}
	else {
		return false;
	}

}



