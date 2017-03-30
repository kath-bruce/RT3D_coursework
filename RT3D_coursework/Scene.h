#pragma once
#ifndef SCENE_H
#define SCENE_H

#include "Renderer.h"
#include "CollisionDetector.h"
#include "HUDObject.h"
#include "bass.h"
#include <vector>
#include <unordered_map>

class Scene {
public:
	Scene(char * vertName, char * fragName, std::vector<char *> textureNames, std::vector<char *> meshNames, char * playerTex, char * playerMesh, char * ttfName);
	//Scene(char * vertName, char * fragName, char * textureName, char * meshName, char * playerTex, char * playerMesh);
	~Scene() { delete renderer; delete player; }
	void addGameObject(GameObject gameObj);
	void addGameObject(std::string name, glm::vec3 pos, glm::vec3 scale, char * textureName, char * meshName);
	//void addGameObjects(GameObject gameObj, int copies, float diffX, float diffY, float diffZ);
	//void addGameObjects(std::string name, glm::vec3 pos, glm::vec3 scale, char * textureName,
		//char * meshName, int copies, float diffX, float diffY, float diffZ);
	void updateLight();
	void updateCollectables();
	void renderScene();
	void updatePlayerR(GLfloat deltaR);
	void movePlayerForward(GLfloat delta);
	void movePlayerRight(GLfloat delta);
	double getTimeScalar();
	void idleAnimation();
	HSAMPLE loadAudio(char * filename);
	void initSounds();
	void playBackgroundMusic();
	bool isGameWon() { return gameWon; }
	void updateCar();
private:
	Renderer * renderer;
	glm::vec3 eye{0.0f, 1.0f, 4.0f};
	glm::vec3 at{ 0.0f, 1.0f, 3.0f };
	glm::vec3 up{ 0.0f, 1.0f, 0.0f };
	std::vector<GameObject> gameObjects;
	std::vector<HUDObject> hud;
	GameObject * player;
	std::unordered_map<char *, rt3d::lightStruct> lights;
	glm::vec4 lightPos;
	bool night = false;
	bool gameWon = false;
	int collectables;
	std::unordered_map<char *, rt3d::materialStruct> materials;
	void initLights();
	void initGameObjects(char * tex, char * mesh);
	GameObject getGameObject(std::string gName);
	glm::vec3 moveForward(glm::vec3 pos, GLfloat angle, GLfloat d);
	glm::vec3 moveRight(glm::vec3 pos, GLfloat angle, GLfloat d);
	void checkCollectableCollision();
	bool checkCollisions();
	int getGameObjectIndex(std::string objName);
	std::vector<HSAMPLE> audio;

};

#endif
