#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include <glm/glm.hpp>
#include <vector>
#include <stack>
#include "rt3d.h"
#include "GameObject.h"

class Renderer {
public:
	Renderer(char * vertName, char * fragName, std::vector<char *> textures);
	void render(std::vector<GameObject> gameObjs);
	void addTexture(char * fName);
	void addMesh(char * fName);
	GLuint getShader() { return shaderProg; }
private:
	GLuint shaderProg;
	std::vector<GLuint> textures;
	std::vector<GLuint> meshes;
	std::stack<glm::mat4> mvStack;
};

#endif
