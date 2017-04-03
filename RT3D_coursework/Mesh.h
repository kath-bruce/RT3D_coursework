#pragma once
#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>

//used for rendering game object meshes
class Mesh {
public:
	Mesh(GLuint mId, GLuint mIndexCount, char * mName) {
		meshId = mId;
		meshIndexCount = mIndexCount;
		meshName = mName;
	}
	//should never be used
	Mesh() { meshId = -1; meshIndexCount = -1; meshName = '\0'; } 
	GLuint getMeshId() { return meshId; }
	GLuint getMeshIndexCount() { return meshIndexCount; }
	char * getMeshName() { return meshName; }
private:
	GLuint meshId;
	GLuint meshIndexCount;
	char * meshName;
};

#endif
