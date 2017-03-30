#include "Renderer.h"

Renderer::Renderer(char * vertName, char * fragName, std::vector<char *> textureNames, std::vector<char *> meshNames, char * ttfName)
{
	shaderProg = rt3d::initShaders(vertName, fragName);
	skyBoxProg = rt3d::initShaders("cubeMap.vert", "cubeMap.frag");
	textProg = rt3d::initShaders("textured.vert", "textured.frag");

	for (char * tex : textureNames) {
		addTexture(tex);
	}

	for (char * mesh : meshNames) {
		addMesh(mesh);
	}

	loadSkybox();

	glm::mat4 modelView(1.0);
	mvStack.push(modelView);

	initTTF(ttfName);

	//below could be in main
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST); // enable depth testing
	//
}

void Renderer::initTTF(char * ttfName) {
	// set up TrueType / SDL_ttf
	if (TTF_Init() == -1)
		std::cout << "TTF failed to initialise." << std::endl;

	textFont = TTF_OpenFont(ttfName, 24);
	if (textFont == NULL)
		std::cout << "Failed to open font." << std::endl;
}

void Renderer::loadSkybox() {
	const char *cubeTexFiles[6] = {
		"Town-skybox/Town_bk.bmp", "Town-skybox/Town_rt.bmp", "Town-skybox/Town_ft.bmp", "Town-skybox/Town_lf.bmp", "Town-skybox/Town_up.bmp", "Town-skybox/Town_up.bmp"
	};

	loadCubeMap(cubeTexFiles, &skybox[0]);
}

void Renderer::render(std::vector<GameObject> gameObjs, glm::vec3 eye, glm::vec3 at, glm::vec3 up, GameObject * player, std::vector<HUDObject> hud, rt3d::lightStruct mainLight)
{
	// set up projection matrix
	glm::mat4 projection(1.0);
	projection = glm::perspective(float(60.0f*DEG_TO_RADIAN), 800.0f / 600.0f, 1.0f, 200.0f);

	setCamera(eye, at, up, *player);

	renderSkyBox(projection);

	glUseProgram(shaderProg);

	rt3d::setLight(shaderProg, mainLight);
	glm::vec4 temp = mvStack.top() * glm::vec4(0.0f, 2.0f, -6.0f, 1.0f);
	rt3d::setLightPos(shaderProg, glm::value_ptr(temp));

	rt3d::setUniformMatrix4fv(shaderProg, "projection", glm::value_ptr(projection));
	for (auto gObj : gameObjs) {
		renderObject(gObj);
	}

	renderPlayer(*player);

	if (hud.size() > 0)
	{
		glUseProgram(textProg);
		for (HUDObject hudObj : hud)
			renderHUD(hudObj);
	}
}

void Renderer::renderHUD(HUDObject hudObj) {
	GLuint label = 0;
	label = textToTexture(hudObj.getDisplay().c_str(), label);

	glBindTexture(GL_TEXTURE_2D, label);

	mvStack.push(glm::mat4(1.0));
	mvStack.top() = glm::translate(mvStack.top(), hudObj.getPos());
	mvStack.top() = glm::scale(mvStack.top(), hudObj.getScale());
	rt3d::setUniformMatrix4fv(textProg, "projection", glm::value_ptr(glm::mat4(1.0)));
	rt3d::setUniformMatrix4fv(textProg, "modelview", glm::value_ptr(mvStack.top()));

	Mesh textMesh = getMesh("cube.obj");
	rt3d::drawIndexedMesh(textMesh.getMeshId(), textMesh.getMeshIndexCount(), GL_TRIANGLES);
	mvStack.pop();
}

GLuint Renderer::textToTexture(const char * str, GLuint textID) {
	GLuint texture = textID;
	TTF_Font * font = textFont;

	SDL_Surface * stringImage = TTF_RenderText_Blended(font, str, { 255, 255, 255 });

	if (stringImage == NULL) {
		std::cout << "String surface not created." << std::endl;
	}

	if (texture == 0) {
		glGenTextures(1, &texture);//This avoids memory leakage, only initialise //first time
	}

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, stringImage->w, stringImage->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, stringImage->pixels);
	glBindTexture(GL_TEXTURE_2D, NULL);

	SDL_FreeSurface(stringImage);
	return texture;
}

void Renderer::setCamera(glm::vec3 &eye, glm::vec3 &at, glm::vec3 &up, GameObject &player)
{
	at = player.getPos(); //at is position of player
	eye = glm::vec3(at.x + (-8.0f)*std::sin(player.getRotation()*DEG_TO_RADIAN),
		at.y, at.z - (-8.0f)*std::cos(player.getRotation()*DEG_TO_RADIAN));
	eye.y += 3.0;
	mvStack.top() = glm::lookAt(eye, at, up);
}

void Renderer::renderObject(GameObject obj)
{
	glBindTexture(GL_TEXTURE_2D, obj.getTexture());
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), obj.getPos());
	mvStack.top() = glm::scale(mvStack.top(), obj.getScale());

	mvStack.top() = glm::rotate(mvStack.top(), float(obj.getRotation() * DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
	mvStack.top() = glm::rotate(mvStack.top(), float(270 * DEG_TO_RADIAN), glm::vec3(1.0f, 0.0f, 0.0f));
	mvStack.top() = glm::rotate(mvStack.top(), float(180 * DEG_TO_RADIAN), glm::vec3(0.0f, 0.0f, 1.0f));

	rt3d::setUniformMatrix4fv(shaderProg, "modelview", glm::value_ptr(mvStack.top()));

	rt3d::drawIndexedMesh(obj.getMesh().getMeshId(), obj.getMesh().getMeshIndexCount(), GL_TRIANGLES);

	mvStack.pop();
}

void Renderer::renderPlayer(GameObject obj) {
	tmpModel.Animate(obj.getCurrentAnim(), 0.1);
	rt3d::updateMesh(obj.getMesh().getMeshId(), RT3D_VERTEX, tmpModel.getAnimVerts(), tmpModel.getVertDataSize());

	glBindTexture(GL_TEXTURE_2D, obj.getTexture());
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), obj.getPos());
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(obj.getScale().x / 20, obj.getScale().y / 20, obj.getScale().z / 20));

	if (obj.getCurrentAnim() == 10) {
		mvStack.top() = glm::rotate(mvStack.top(), (float(180.0f * DEG_TO_RADIAN)), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else {
		mvStack.top() = glm::rotate(mvStack.top(), float(-obj.getRotation() * DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	mvStack.top() = glm::rotate(mvStack.top(), float(270 * DEG_TO_RADIAN), glm::vec3(1.0f, 0.0f, 0.0f));
	mvStack.top() = glm::rotate(mvStack.top(), float(90 * DEG_TO_RADIAN), glm::vec3(0.0f, 0.0f, 1.0f));
	rt3d::setUniformMatrix4fv(shaderProg, "modelview", glm::value_ptr(mvStack.top()));

	rt3d::drawMesh(obj.getMesh().getMeshId(), obj.getMesh().getMeshIndexCount(), GL_TRIANGLES);
	mvStack.pop();
}

void Renderer::addTexture(char * fName)
{
	//textures.push_back(loadBitmap(fName));
	textures.insert({ fName, loadBitmap(fName) });
}

void Renderer::addMesh(char * fName)
{

	std::string tmpStr(fName);

	int strLength = tmpStr.length();

	if (tmpStr.substr(strLength - 3, strLength) == "md2")
	{
		//md2model tmpModel;
		GLuint temp = tmpModel.ReadMD2Model(fName);
		meshes.push_back(Mesh(temp, tmpModel.getVertDataCount(), fName));
	}
	else {
		std::vector<GLfloat> verts;
		std::vector<GLfloat> norms;
		std::vector<GLfloat> tex_coords;
		std::vector<GLuint> indices;
		rt3d::loadObj(fName, verts, norms, tex_coords, indices);
		GLuint meshIndexCount = indices.size();
		meshes.push_back(Mesh(rt3d::createMesh(verts.size() / 3,
			verts.data(), nullptr, norms.data(),
			tex_coords.data(), meshIndexCount,
			indices.data()), meshIndexCount, fName));
	}

}

GLuint Renderer::getTexture(char * texName)
{
	return textures.at(texName);
}

Mesh Renderer::getMesh(char * meshName)
{
	//return meshes.at(meshName);
	for (auto mesh : meshes) {
		if (mesh.getMeshName() == meshName)
			return mesh;
	}

	return Mesh();
}

// A simple texture loading function
// lots of room for improvement - and better error checking!
GLuint Renderer::loadBitmap(char *fname)
{
	GLuint texID;
	glGenTextures(1, &texID); // generate texture ID

							  // load file - using core SDL library
	SDL_Surface *tmpSurface;
	tmpSurface = SDL_LoadBMP(fname);
	if (!tmpSurface)
	{
		std::cout << "Error loading bitmap" << std::endl;
	}

	// bind texture and set parameters
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	SDL_PixelFormat *format = tmpSurface->format;
	GLuint externalFormat, internalFormat;
	if (format->Amask) {
		internalFormat = GL_RGBA;
		externalFormat = (format->Rmask < format->Bmask) ? GL_RGBA : GL_BGRA;
	}
	else {
		internalFormat = GL_RGB;
		externalFormat = (format->Rmask < format->Bmask) ? GL_RGB : GL_BGR;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, tmpSurface->w, tmpSurface->h, 0,
		externalFormat, GL_UNSIGNED_BYTE, tmpSurface->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	SDL_FreeSurface(tmpSurface); // texture loaded, free the temporary buffer
	return texID;	// return value of texture ID
}

GLuint Renderer::loadCubeMap(const char * fname[6], GLuint * texID)
{
	glGenTextures(1, texID); // generate texture ID
	GLenum sides[6] = { GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y };
	SDL_Surface *tmpSurface;

	glBindTexture(GL_TEXTURE_CUBE_MAP, *texID); // bind texture and set parameters
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	GLuint externalFormat;
	for (int i = 0;i < 6;i++)
	{
		// load file - using core SDL library
		tmpSurface = SDL_LoadBMP(fname[i]);
		if (!tmpSurface)
		{
			std::cout << "Error loading bitmap" << std::endl;
			return *texID;
		}

		// skybox textures should not have alpha (assuming this is true!)
		SDL_PixelFormat *format = tmpSurface->format;
		externalFormat = (format->Rmask < format->Bmask) ? GL_RGB : GL_BGR;

		glTexImage2D(sides[i], 0, GL_RGB, tmpSurface->w, tmpSurface->h, 0,
			externalFormat, GL_UNSIGNED_BYTE, tmpSurface->pixels);
		// texture loaded, free the temporary buffer
		SDL_FreeSurface(tmpSurface);
	}
	return *texID;	// return value of texure ID, redundant really
}

void Renderer::renderSkyBox(glm::mat4 projection)
{

	// skybox as single cube using cube map
	glUseProgram(skyBoxProg);
	rt3d::setUniformMatrix4fv(skyBoxProg, "projection", glm::value_ptr(projection));
	glDepthMask(GL_FALSE); // make sure writing to update depth test is off
	glm::mat3 mvRotOnlyMat3 = glm::mat3(mvStack.top());
	mvStack.push(glm::mat4(mvRotOnlyMat3));
	glCullFace(GL_FRONT); // drawing inside of cube!
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox[0]);
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(1.5f, 1.5f, 1.5f));
	rt3d::setUniformMatrix4fv(skyBoxProg, "modelview", glm::value_ptr(mvStack.top()));
	Mesh cubeMesh = getMesh("cube.obj");
	rt3d::drawIndexedMesh(cubeMesh.getMeshId(), cubeMesh.getMeshIndexCount(), GL_TRIANGLES);
	mvStack.pop();
	glCullFace(GL_BACK); // drawing inside of cube!
						 // back to remainder of rendering
	glDepthMask(GL_TRUE); // make sure depth test is on
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

