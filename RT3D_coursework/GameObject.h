#pragma once
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glm/glm.hpp>
#include <string>
#include <GL/glew.h>

//taken from games tech group project

class GameObject {
public:
	GameObject(std::string nName, glm::vec3 nPos, glm::vec3 nScale, GLuint texId, GLuint meshId) {
		name = nName;
		pos = nPos;
		startPos = nPos;
		scale = nScale;
		texture = texId;
		mesh = meshId;
		lastColl = "";
	}
	std::string getName() { return name; }
	glm::vec3 getPos() { return pos; }
	glm::vec3 getScale() { return scale; }
	GLuint getTexture() { return texture; }
	GLuint getMesh() { return mesh; }
	void setPos(glm::vec3 newPos) { pos = newPos; }
	std::string getLastCollision() { return lastColl; }
	void setLastCollision(std::string lastCollision) { lastColl = lastCollision; }
	void reset() { pos = startPos; lastColl = ""; }
private:
	GLuint texture;
	GLuint mesh;
	glm::vec3 pos;
	glm::vec3 startPos;
	glm::vec3 scale;
	std::string name;
	std::string lastColl;
};

#endif
