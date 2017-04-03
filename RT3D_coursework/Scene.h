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
	//constructor - vert and frag filenames, vectors of textures (bmps) and meshes (objs), player texture and mesh (md2) and ttf file
	//vector of meshes can include md2 but leads to odd rendering in player character
	Scene(char * vertName, char * fragName, std::vector<char *> textureNames, std::vector<char *> meshNames, 
		char * playerTex, char * playerMesh, char * ttfName);
	~Scene() { delete renderer; delete player; }
	void addGameObject(GameObject gameObj);
	void addGameObject(std::string name, glm::vec3 pos, glm::vec3 scale, char * textureName, char * meshName);
	
	//day night cycle
	void updateLight(); 

	//spin collectables
	void updateCollectables(); 

	//render all game objects
	void renderScene(); 
	void updatePlayerR(GLfloat deltaR); 
	void movePlayerForward(GLfloat delta);
	void movePlayerRight(GLfloat delta);

	//used so that movement speed is not based on processor speed
	double getTimeScalar(); 

	//animation when not moving
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

	//used for day night cycle
	bool night = false; 

	//used for animation and UI
	bool gameWon = false; 

	//number of collectables left
	int collectables; 

	void initLights();

	//create the ground as first game object
	void initGameObjects(char * tex, char * mesh); 

	GameObject getGameObject(std::string gName);
	glm::vec3 moveForward(glm::vec3 pos, GLfloat angle, GLfloat d);
	glm::vec3 moveRight(glm::vec3 pos, GLfloat angle, GLfloat d);

	//check if player had last collided with a collectable - if so, remove from game objects vector
	void checkCollectableCollision();

	//check player collisions with all other game objects
	bool checkCollisions();

	//get index of game object in vector
	int getGameObjectIndex(std::string objName); 
	std::vector<HSAMPLE> audio;

};

#endif
