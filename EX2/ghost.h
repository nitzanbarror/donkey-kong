#pragma once
#include "entityGhost.h"

class Ghost : public entityGhost {

public:
	Ghost(): entityGhost() {}
	
	void move() override;
	void draw() const override { entityGhost::draw((char)Characters::GHOST); }
};

