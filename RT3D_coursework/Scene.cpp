#include "Scene.h"

Scene::Scene(char * vertName, char * fragName, std::vector<char*> textureNames, std::vector<char*> meshNames, char * playerTex, char * playerMesh)
{
	renderer = new Renderer(vertName, fragName, textureNames, meshNames);
	renderer->addTexture(playerTex);
	renderer->addMesh(playerMesh);
	initLights();
	initGameObjects(textureNames[0], meshNames[0]);
	player = new GameObject("player", glm::vec3(8.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f), renderer->getTexture(playerTex), renderer->getMesh(playerMesh));
}

Scene::Scene(char * vertName, char * fragName, char * textureName, char * meshName, char * playerTex, char * playerMesh)
{
	renderer = new Renderer(vertName, fragName, textureName, meshName);
	renderer->addTexture(playerTex);
	renderer->addMesh(playerMesh);
	initLights();
	initGameObjects(textureName, meshName);
	player = new GameObject("player", glm::vec3(8.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f), renderer->getTexture(playerTex), renderer->getMesh(playerMesh));
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

void Scene::updateLight() {

	if (night) {
		lights.at("mainLight").ambient[0] += 0.001f;
		lights.at("mainLight").ambient[1] += 0.001f;
		lights.at("mainLight").ambient[2] += 0.001f;
	}
	else {
		lights.at("mainLight").ambient[0] -= 0.001f;
		lights.at("mainLight").ambient[1] -= 0.001f;
		lights.at("mainLight").ambient[2] -= 0.001f;
	}

	if (lights.at("mainLight").ambient[0] <= -0.25
		&& lights.at("mainLight").ambient[1] <= -0.25
		&& lights.at("mainLight").ambient[2] <= -0.25)
		night = true;
	else if (lights.at("mainLight").ambient[0] >= 0.2
		&& lights.at("mainLight").ambient[1] >= 0.2
		&& lights.at("mainLight").ambient[2] >= 0.2)
		night = false;

	//std::cout << lights.at("mainLight").ambient[0] << std::endl;

	//std::cout << lights.at("mainLight").ambient[1] << std::endl;

	//std::cout << lights.at("mainLight").ambient[2] << std::endl;
}

void Scene::renderScene()
{
	rt3d::setLight(renderer->getShader(), lights.at("mainLight")); // set in scene
	glm::vec4 temp = renderer->getStackTop() * lightPos;
	rt3d::setLightPos(renderer->getShader(), glm::value_ptr(temp));

	renderer->render(gameObjects, eye, at, up, player);

}

bool Scene::checkCollisions() {
	bool collided = false;
	for (int i = 0; i < gameObjects.size(); i++) {
		if (CollisionDetector::detectCollision(gameObjects[i], *player))
			collided = true;
	}
	return collided;
}

void Scene::updatePlayerR(GLfloat deltaR)
{
	player->setRotation(player->getRotation() + deltaR);
}

void Scene::movePlayerForward(GLfloat delta) {

	glm::vec3 temp = player->getPos();

	player->setPos(moveForward(player->getPos(), player->getRotation(), delta / getTimeScalar()));

	if (checkCollisions())
		player->setPos(temp);

	player->currentAnim = 1;
}

void Scene::movePlayerRight(GLfloat delta) {

	glm::vec3 temp = player->getPos();

	player->setPos(moveRight(player->getPos(), player->getRotation(), delta / getTimeScalar()));

	if (checkCollisions())
		player->setPos(temp);

	player->currentAnim = 1;
}

glm::vec3 Scene::moveForward(glm::vec3 pos, GLfloat angle, GLfloat d)
{
	return glm::vec3(pos.x + d*std::sin(angle*DEG_TO_RADIAN), pos.y, pos.z - d*std::cos(angle*DEG_TO_RADIAN));
}

glm::vec3 Scene::moveRight(glm::vec3 pos, GLfloat angle, GLfloat d)
{
	return glm::vec3(pos.x + d*std::cos(angle*DEG_TO_RADIAN), pos.y, pos.z + d*std::sin(angle*DEG_TO_RADIAN));
}


double Scene::getTimeScalar() {
	static unsigned int lastTicks = 0;

	int ticks = SDL_GetTicks();
	int ticksSince = ticks - lastTicks;

	double scalar = (double)ticksSince / (double) 60.0f;

	lastTicks = ticks;

	if (scalar < 0.2 || scalar > 1.0)
		scalar = 0.28333;

	return scalar;
}

void Scene::idleAnimation() {
	player->currentAnim = 0;
}

