#pragma once
#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>

class Mesh {
public:
	Mesh(GLuint mId, GLuint mIndexCount, char * mName) {
		meshId = mId;
		meshIndexCount = mIndexCount;
		meshName = mName;
	}
	~Mesh() {};
	Mesh() { meshId = -1; meshIndexCount = -1; meshName = '\0'; } //should never be used
	GLuint getMeshId() { return meshId; }
	GLuint getMeshIndexCount() { return meshIndexCount; }
	char * getMeshName() { return meshName; }
private:
	GLuint meshId;
	GLuint meshIndexCount;
	char * meshName;
};

#endif
