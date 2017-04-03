#ifndef COLLISIONDETECTOR_H
#define COLLISIONDETECTOR_H

#include <iostream>
#include "GameObject.h"

//header and cpp file taken from games tech group project

class CollisionDetector {
public:
	static bool detectCollision(GameObject &objA, GameObject &objB);
};

#endif
