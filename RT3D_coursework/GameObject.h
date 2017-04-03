#pragma once
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include "Mesh.h"

//header and cpp based on header and cpp from games tech group project

#define DEG_TO_RADIAN 0.017453293

class GameObject {
public:
	GameObject(std::string nName, glm::vec3 nPos, glm::vec3 nScale, GLuint texId, Mesh meshId) {
		name = nName;
		pos = nPos;
		lastPos = nPos;
		scale = nScale;
		texture = texId;
		mesh = meshId;
		lastColl = "";
		rot = 0.0f;
		currentAnim = 0;
	}
	std::string getName() { return name; }
	glm::vec3 getPos() { return pos; }
	void setPos(glm::vec3 nPos) { lastPos = pos; pos = nPos; }
	glm::vec3 getLastPos() { return lastPos; }
	void setLastPos(glm::vec3 nLastPos) { lastPos = nLastPos; }
	glm::vec3 getScale() { return scale; }
	GLuint getTexture() { return texture; }
	Mesh getMesh() { return mesh; }
	std::string getLastCollision() { return lastColl; }
	void setLastCollision(std::string lastCollision) { lastColl = lastCollision; }
	GLfloat getRotation() { return rot; }
	void setRotation(GLfloat nRot) { rot = nRot; }
	int getCurrentAnim() { return currentAnim; }
	void setCurrentAnim(int anim) { currentAnim = anim; }
private:
	GLuint texture;
	Mesh mesh;
	glm::vec3 pos;
	glm::vec3 lastPos;
	glm::vec3 scale; //could add separate scale members for rendering and collision
	std::string name;
	std::string lastColl;
	GLfloat rot;
	int currentAnim;
};

#endif
