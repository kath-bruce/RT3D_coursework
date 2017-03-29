#include "CollisionDetector.h"

bool CollisionDetector::detectCollision(GameObject &objA, GameObject &objB)
{
	if (objA.getPos().x + objA.getScale().x > objB.getPos().x - objB.getScale().x
		&& objA.getPos().x - objA.getScale().x < objB.getPos().x + objB.getScale().x) 
	{
		if (objA.getPos().z + objA.getScale().z > objB.getPos().z - objB.getScale().z
			&& objA.getPos().z - objA.getScale().z < objB.getPos().z + objB.getScale().z)
		{
			if (objA.getPos().y + objA.getScale().y > objB.getPos().y - objB.getScale().y
				&& objA.getPos().y - objA.getScale().y < objB.getPos().y + objB.getScale().y) 
			{
				objA.setLastCollision(objB.getName());
				objB.setLastCollision(objA.getName());

				//std::cout << "collided with " << objA.getName() << "\n";

				return true;
			}
		}
	}
	return false;
}
