#include "Scene.h"

Scene::Scene(char * vertName, char * fragName, std::vector<char*> textureNames, std::vector<char*> meshNames, char * playerTex, char * playerMesh, char * ttfName)
{
	renderer = new Renderer(vertName, fragName, textureNames, meshNames, ttfName);
	renderer->addTexture(playerTex);
	renderer->addMesh(playerMesh);
	initLights();
	initGameObjects(textureNames[0], meshNames[0]);
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
	gameObjects.push_back(GameObject("ground", glm::vec3(10.0f, -0.1f, 10.0f), glm::vec3(100.0f, 0.1f, 100.0f), renderer->getTexture(tex), renderer->getMesh(mesh)));
}

GameObject Scene::getGameObject(std::string gName)
{
	for (GameObject gObj : gameObjects) {
		if (gObj.getName() == gName)
			return gObj;
	}
}

void Scene::addGameObject(GameObject gameObj)
{
	gameObjects.push_back(gameObj);

	std::string nameStr = gameObj.getName();
	if (nameStr.substr(0, 11) == "collectable")
		collectables++;
}

void Scene::addGameObject(std::string name, glm::vec3 pos, glm::vec3 scale, char * textureName, char * meshName)
{
	GLuint texture = renderer->getTexture(textureName);
	Mesh mesh = renderer->getMesh(meshName);
	gameObjects.push_back(GameObject(name, pos, scale, texture, mesh));

	std::string nameStr = name;
	if (nameStr.substr(0, 11) == "collectable")
		collectables++;
}

int Scene::getGameObjectIndex(std::string objName) {
	for (int i = 0; i < gameObjects.size(); i++) {
		if (gameObjects[i].getName() == objName)
			return i;
	}
	return -1;
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

	if (lights.at("mainLight").ambient[0] <= 0.0
		&& lights.at("mainLight").ambient[1] <= 0.0
		&& lights.at("mainLight").ambient[2] <= 0.0)
		night = true;
	else if (lights.at("mainLight").ambient[0] >= 0.35
		&& lights.at("mainLight").ambient[1] >= 0.35
		&& lights.at("mainLight").ambient[2] >= 0.35)
		night = false;
}

void Scene::updateCollectables()
{
	if (collectables > 0) {
		for (int i = 0; i < gameObjects.size(); i++) {
			if (gameObjects[i].getName().substr(0, 11) == "collectable") {
				gameObjects[i].setRotation(gameObjects[i].getRotation() + 1.0f);
			}
		}
	}
	else {
		gameWon = true;
	}

}

void Scene::renderScene()
{
	if (!gameWon) {
		std::string collectablesStr("Collectables left: ");
		collectablesStr.append(std::to_string(collectables));
		hud.push_back(HUDObject(glm::vec3(-0.5f, -0.8f, 0.0f), glm::vec3(0.5f, 0.2f, 0.0f), collectablesStr));
	}
	else {
		hud.push_back(HUDObject(glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.25f, 0.25f, 0.0f), "You WON!"));
		hud.push_back(HUDObject(glm::vec3(0.0f, -0.6f, 0.0f), glm::vec3(0.5f, 0.25f, 0.0f), "Press R to restart"));
	}

	renderer->render(gameObjects, eye, at, up, player, hud, lights.at("mainLight"));

	hud.clear();

}

bool Scene::checkCollisions() {
	bool collided = false;
	for (int i = 0; i < gameObjects.size(); i++) {
		if (CollisionDetector::detectCollision(gameObjects[i], *player)) {
			collided = true;
		}
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

	if (checkCollisions()) {
		player->setPos(temp);
		checkCollectableCollision();
	}

	player->setCurrentAnim(1); //walking anim
}

void Scene::movePlayerRight(GLfloat delta) {

	glm::vec3 temp = player->getPos();

	player->setPos(moveRight(player->getPos(), player->getRotation(), delta / getTimeScalar()));

	if (checkCollisions()) {
		player->setPos(temp);
		checkCollectableCollision();
	}

	player->setCurrentAnim(1); //walking anim
}

glm::vec3 Scene::moveForward(glm::vec3 pos, GLfloat angle, GLfloat d)
{
	return glm::vec3(pos.x + d*std::sin(angle*DEG_TO_RADIAN), pos.y, pos.z - d*std::cos(angle*DEG_TO_RADIAN));
}

glm::vec3 Scene::moveRight(glm::vec3 pos, GLfloat angle, GLfloat d)
{
	return glm::vec3(pos.x + d*std::cos(angle*DEG_TO_RADIAN), pos.y, pos.z + d*std::sin(angle*DEG_TO_RADIAN));
}

void Scene::checkCollectableCollision() {
	std::string playerColl = player->getLastCollision();
	if (playerColl.substr(0, 11) == "collectable") {

		std::cout << "player collided with collectable\n";
		int index = getGameObjectIndex(player->getLastCollision());
		player->setLastCollision("");
		gameObjects.erase(gameObjects.begin() + index);
		collectables--;

		//Audio played on collision
		HCHANNEL ch = BASS_SampleGetChannel(audio[1], FALSE);
		BASS_ChannelSetAttribute(ch, BASS_ATTRIB_VOL, 10000.0);

		if (!BASS_ChannelPlay(ch, FALSE))
			std::cout << "Can't play sample" << std::endl;
	}
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
	if (gameWon) {
		player->setCurrentAnim(10); //gameboy animation
	}
	else {
		player->setCurrentAnim(0); //idle animation
	}

}

HSAMPLE Scene::loadAudio(char * filename) {
	HSAMPLE sam;
	if (sam = BASS_SampleLoad(FALSE, filename, 0, 0, 3, BASS_SAMPLE_OVER_POS))
		std::cout << "sample " << filename << " loaded!" << std::endl;
	else
	{
		std::cout << "Can't load sample";
		exit(0);
	}
	return sam;
}

void Scene::initSounds() {
	if (!BASS_Init(-1, 44100, 0, 0, NULL))
		std::cout << "Can't Inialize device";

	audio.push_back(loadAudio("Yoshi's Island Medley.wav"));
	//http://www.smashcustommusic.com/71268

	audio.push_back(loadAudio("yoshi_tongue.wav"));
	//http://www.zedge.net/ringtone/1124379/

	HCHANNEL ch = BASS_SampleGetChannel(audio[0], FALSE);
	BASS_ChannelSetAttribute(ch, BASS_ATTRIB_FREQ, 0);
	BASS_ChannelSetAttribute(ch, BASS_ATTRIB_VOL, 0.5);
	BASS_ChannelSetAttribute(ch, BASS_ATTRIB_PAN, -1);
	int flag = BASS_ChannelFlags(ch, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);
	std::cout << flag << std::endl;
}

void Scene::playBackgroundMusic() {
	HCHANNEL ch = BASS_SampleGetChannel(audio[0], TRUE);
	BASS_ChannelSetAttribute(ch, BASS_ATTRIB_VOL, 0.5);
	if (!BASS_ChannelPlay(ch, TRUE))
		std::cout << "Can't play sample - " << BASS_ErrorGetCode() << std::endl;
}

void Scene::updateCar()
{
	int carIndex = getGameObjectIndex("car");
	glm::vec3 newCarPos = gameObjects[carIndex].getPos();

	//car drives between two buildings
	if ((newCarPos.x - gameObjects[carIndex].getLastPos().x) >= 0) {
		newCarPos.x += 0.1f;

		if (gameObjects[carIndex].getRotation() != 270.0f)
			gameObjects[carIndex].setRotation(270.0f);

		if (newCarPos.x >= 50.0f) {
			newCarPos.x -= 0.2f;
		}
	}
	else {
		newCarPos.x -= 0.1f;

		if (gameObjects[carIndex].getRotation() != 90.0f)
			gameObjects[carIndex].setRotation(90.0f);

		if (newCarPos.x <= -50.0f) {
			newCarPos.x += 0.2f;
		}
	}

	if (!CollisionDetector::detectCollision(gameObjects[carIndex], *player)) {
		gameObjects[carIndex].setPos(newCarPos);
	}

}
