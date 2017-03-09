#pragma once
#ifndef SCENE_H
#define SCENE_H

#include "Renderer.h"
#include <vector>
#include <unordered_map>

class Scene {
public:
	Scene(char * vertName, char * fragName, std::vector<char *> textureNames, std::vector<char *> meshNames);
	Scene(char * vertName, char * fragName, char * textureName, char * meshName);
	~Scene() { delete renderer; delete player; }
	void addGameObject(GameObject gameObj);
	void addGameObject(char * name, glm::vec3 pos, glm::vec3 scale, char * textureName, char * meshName);
	void addGameObjects(GameObject gameObj, int copies, float diffX, float diffY, float diffZ);
	void addGameObjects(char * name, glm::vec3 pos, glm::vec3 scale, char * textureName, 
		char * meshName, int copies, float diffX, float diffY, float diffZ);
	void renderScene();
private:
	Renderer * renderer;
	glm::vec3 eye{0.0f, 1.0f, 4.0f};
	glm::vec3 at{ 0.0f, 1.0f, 3.0f };
	glm::vec3 up{ 0.0f, 1.0f, 0.0f };
	//glm::vec3(10.0f, -0.1f, 10.0f); //ground
	std::vector<GameObject> gameObjects;
	GameObject * player;
	//std::unordered_map<char *, GameObject> gameObjects;
	std::unordered_map<char *, rt3d::lightStruct> lights;
	glm::vec4 lightPos;
	std::unordered_map<char *, rt3d::materialStruct> materials;
	void initLights();
	void initGameObjects(char * tex, char * mesh);
	GameObject getGameObject(char * gName);
};

#endif
