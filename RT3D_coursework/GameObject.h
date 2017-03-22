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
	GameObject(char * nName, glm::vec3 nPos, glm::vec3 nScale, GLuint texId, Mesh meshId) {
		name = nName;
		pos = nPos;
		startPos = nPos;
		scale = nScale;
		texture = texId;
		mesh = meshId;
		lastColl = "";
		rot = 0.0f;
	}
	~GameObject() {};
	char * getName() { return name; }
	glm::vec3 getPos() { return pos; }
	void setPos(glm::vec3 nPos) { pos = nPos; }
	glm::vec3 getScale() { return scale; }
	GLuint getTexture() { return texture; }
	Mesh getMesh() { return mesh; }
	char * getLastCollision() { return lastColl; }
	void setLastCollision(char * lastCollision) { lastColl = lastCollision; }
	void reset() { pos = startPos; lastColl = ""; }
	GLfloat getRotation() { return rot; }
	void setRotation(GLfloat nRot) { rot = nRot; }
	int getCurrentAnim() { return currentAnim; }
	int currentAnim = 0;
private:
	GLuint texture;
	//GLuint mesh;
	Mesh mesh;
	glm::vec3 pos;
	glm::vec3 startPos;
	glm::vec3 scale;
	char * name;
	char * lastColl;
	GLfloat rot;
	glm::vec3 moveForward(glm::vec3 cam, GLfloat angle, GLfloat d) {
		return glm::vec3(cam.x + d*std::sin(angle*DEG_TO_RADIAN),
			cam.y, cam.z - d*std::cos(angle*DEG_TO_RADIAN));
	}

	glm::vec3 moveRight(glm::vec3 pos, GLfloat angle, GLfloat d) {
		return glm::vec3(pos.x + d*std::cos(angle*DEG_TO_RADIAN),
			pos.y, pos.z + d*std::sin(angle*DEG_TO_RADIAN));
	}
};

#endif
