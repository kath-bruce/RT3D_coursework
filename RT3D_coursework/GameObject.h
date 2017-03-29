#pragma once
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include "Mesh.h"

//taken from games tech group project

#define DEG_TO_RADIAN 0.017453293

class GameObject {
public:
	GameObject(std::string nName, glm::vec3 nPos, glm::vec3 nScale, GLuint texId, Mesh meshId) {
		name = nName;
		pos = nPos;
		startPos = nPos;
		scale = nScale;
		texture = texId;
		mesh = meshId;
		lastColl = "";
		rot = 0.0f;
	}
	std::string getName() { return name; }
	glm::vec3 getPos() { return pos; }
	void setPos(glm::vec3 nPos) { pos = nPos; }
	glm::vec3 getScale() { return scale; }
	GLuint getTexture() { return texture; }
	Mesh getMesh() { return mesh; }
	std::string getLastCollision() { return lastColl; }
	void setLastCollision(std::string lastCollision) { lastColl = lastCollision; }
	void reset() { pos = startPos; lastColl = ""; }
	GLfloat getRotation() { return rot; }
	void setRotation(GLfloat nRot) { rot = nRot; }
	int getCurrentAnim() { return currentAnim; }
	int currentAnim = 0;
private:
	GLuint texture;
	Mesh mesh;
	glm::vec3 pos;
	glm::vec3 startPos;
	glm::vec3 scale; //could add separate scale members for rendering and collision
	std::string name;
	std::string lastColl;
	GLfloat rot;
};

#endif
