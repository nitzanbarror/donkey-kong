#pragma once
#include "entityGhost.h"

class climbingGhost : public entityGhost {
	int _dirY;
	bool isClimbingUp;
	bool isClimbingDown;
	int prevDirX;
	int counter;
	
	void climbUp();
	void climbDown();

public:
	climbingGhost() : entityGhost(), _dirY(0), isClimbingUp(false), isClimbingDown(false), prevDirX(0), counter(0) {}
	
	void handleUp();
	void handleDown();
	virtual void move() override;
	virtual void draw() const override { entityGhost::draw((char)Characters::BIG_GHOST); }
};

