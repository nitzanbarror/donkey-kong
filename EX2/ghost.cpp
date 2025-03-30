#include "ghost.h"


/* This method moves the ghost in its current direction by updating its X position.
   It calculates the next position based on the current direction and updates the ghost's position.
*/
void Ghost::move() {
    entityGhost::setDirX();

    int nextX = _position.getX() + _dirX;
    _position.setPosition({ nextX, _position.getY() });

}


