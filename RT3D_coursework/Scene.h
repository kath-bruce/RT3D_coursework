#pragma once
#ifndef SCENE_H
#define SCENE_H

#include "Renderer.h"
#include <vector>

class Scene {
public:
	Scene();
	void addGameObject(GameObject gameObj);
	//void addGameObject(name, pos, scale, texture, mesh);
	void addGameObjects(GameObject gameObj, int copies, float diffX, float diffY, float diffZ);
	void renderScene();
private:
	Renderer renderer;
	std::vector<GameObject> gameObjects;
};

#endif
