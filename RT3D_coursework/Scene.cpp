#include "Scene.h"

Scene::Scene(char * vertName, char * fragName, std::vector<char*> textureNames, std::vector<char*> meshNames)
{
	renderer = new Renderer(vertName, fragName, textureNames, meshNames);
	initLights();
	initGameObjects(textureNames[0], meshNames[0]);
	player = new GameObject("player", glm::vec3(0.0f, 1.0f, -4.0f), 
		glm::vec3(1.0f, 1.0f, 1.0f), renderer->getTexture(textureNames[0]), renderer->getMesh(meshNames[0]));
}

Scene::Scene(char * vertName, char * fragName, char * textureName, char * meshName)
{
	renderer = new Renderer(vertName, fragName, textureName, meshName);
	initLights();
	initGameObjects(textureName, meshName);
	player = new GameObject("player", glm::vec3(0.0f, 1.0f, -4.0f),
		glm::vec3(1.0f, 1.0f, 1.0f), renderer->getTexture(textureName), renderer->getMesh(meshName));
}

void Scene::initLights()
{
	lights.insert({ "mainLight",{
		{ 0.2f, 0.2f, 0.2f, 1.0f }, // ambient
		{ 0.7f, 0.7f, 0.7f, 1.0f }, // diffuse
		{ 0.8f, 0.8f, 0.8f, 1.0f }, // specular
		{ 0.0f, 0.0f, 1.0f, 1.0f }  // position
	} });
	lightPos = { 0.0f, 2.0f, -6.0f, 1.0f };
}

void Scene::initGameObjects(char * tex, char * mesh)
{
	gameObjects.push_back(GameObject("ground", glm::vec3(10.0f, -0.1f, 10.0f), glm::vec3(20.0f, 0.1f, 20.0f), renderer->getTexture(tex), renderer->getMesh(mesh)));
}

GameObject Scene::getGameObject(char * gName)
{
	for (GameObject gObj : gameObjects) {
		if (gObj.getName() == gName)
			return gObj;
	}
	//return GameObject("null", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0, 0);
}

void Scene::addGameObject(GameObject gameObj)
{
	gameObjects.push_back(gameObj);
}

void Scene::addGameObject(char * name, glm::vec3 pos, glm::vec3 scale, char * textureName, char * meshName)
{
	GLuint texture = renderer->getTexture(textureName);
	Mesh mesh = renderer->getMesh(meshName);
	gameObjects.push_back(GameObject(name, pos, scale, texture, mesh));
}

void Scene::addGameObjects(GameObject gameObj, int copies, float diffX, float diffY, float diffZ)
{
	for (int i = 0; i < copies; i++) {
		gameObj.setPos(glm::vec3(gameObj.getPos().x + (i * diffX), 
			gameObj.getPos().y + (i * diffY), gameObj.getPos().z + (i * diffZ)));
		gameObjects.push_back(gameObj);
	}
}

void Scene::addGameObjects(char * name, glm::vec3 pos, glm::vec3 scale, char * textureName, char * meshName, int copies, float diffX, float diffY, float diffZ)
{
	GLuint texture = renderer->getTexture(textureName);
	Mesh mesh = renderer->getMesh(meshName);
	GameObject gameObj(name, pos, scale, texture, mesh);

	for (int i = 0; i < copies; i++) {
		gameObj.setPos(glm::vec3(gameObj.getPos().x + (i * diffX),
			gameObj.getPos().y + (i * diffY), gameObj.getPos().z + (i * diffZ)));
		gameObjects.push_back(gameObj);
	}
}

void Scene::renderScene()
{
	rt3d::setLight(renderer->getShader(), lights.at("mainLight")); // set in scene
	glm::vec4 temp = renderer->getStackTop() * lightPos;
	rt3d::setLightPos(renderer->getShader(), glm::value_ptr(temp));

	renderer->render(gameObjects, eye, at, up, player);

}

