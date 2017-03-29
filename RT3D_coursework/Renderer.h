#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <stack>
#include <unordered_map>
#include "rt3d.h"
#include "rt3dObjLoader.h"
#include "GameObject.h"
#include "HUDObject.h"
#include "md2model.h"
#include "SDL_ttf.h"

#define DEG_TO_RADIAN 0.017453293

class Renderer {
public:
	Renderer(char * vertName, char * fragName, std::vector<char *> textureNames, std::vector<char *> meshNames, char * ttfName);
	
	//Renderer(char * vertName, char * fragName, char * textureName, char * meshName);
	//~Renderer() { delete this; };
	void render(std::vector<GameObject> gameObjs, glm::vec3 eye, glm::vec3 at, glm::vec3 up, GameObject * player, std::vector<HUDObject> hud, rt3d::lightStruct mainLight);
	void addTexture(char * fName);
	void addMesh(char * fName);
	GLuint getShader() { return shaderProg; }
	GLuint getTexture(char * texName);
	Mesh getMesh(char * meshName);
	glm::mat4 getStackTop() { return mvStack.top(); }
private:
	GLuint shaderProg;
	GLuint skyBoxProg;
	GLuint textProg;
	GLuint skybox[5];
	std::unordered_map<char *, GLuint> textures;
	std::vector<Mesh> meshes;
	std::stack<glm::mat4> mvStack;
	GLuint loadBitmap(char * fName);
	GLuint loadCubeMap(const char *fname[6], GLuint *texID);
	GLuint textToTexture(const char * str, GLuint textID);
	void loadSkybox();
	void renderSkyBox(glm::mat4 projection);
	void renderObject(GameObject obj);
	void renderPlayer(GameObject obj);
	void setCamera(glm::vec3 &eye, glm::vec3 &at, glm::vec3 &up, GameObject &player);
	md2model tmpModel;
	TTF_Font * textFont;
	void initTTF(char * ttfName);
	void renderHUD(HUDObject hudObj);
};

#endif
