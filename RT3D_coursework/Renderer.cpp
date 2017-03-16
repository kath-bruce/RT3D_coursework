#include "Renderer.h"

Renderer::Renderer(char * vertName, char * fragName, std::vector<char *> textureNames, std::vector<char *> meshNames)
{
	shaderProg = rt3d::initShaders(vertName, fragName);
	skyBoxProg = rt3d::initShaders("cubeMap.vert", "cubeMap.frag");

	for (char * tex : textureNames) {
		addTexture(tex);
	}

	for (char * mesh : meshNames) {
		addMesh(mesh);
	}

	glm::mat4 modelView(1.0);
	mvStack.push(modelView);

	//below could be in main
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST); // enable depth testing
	//
}

Renderer::Renderer(char * vertName, char * fragName, char * textureName, char * meshName)
{
	shaderProg = rt3d::initShaders(vertName, fragName);
	skyBoxProg = rt3d::initShaders("cubeMap.vert", "cubeMap.frag");

	//rt3d::lightStruct light = {
	//	{ 0.3f, 0.3f, 0.3f, 1.0f }, // ambient
	//	{ 1.0f, 1.0f, 1.0f, 1.0f }, // diffuse
	//	{ 1.0f, 1.0f, 1.0f, 1.0f }, // specular
	//	{ -10.0f, 10.0f, 10.0f, 1.0f }  // position
	//};

	//rt3d::setLight(shaderProg, light); 

	addTexture(textureName);

	addMesh(meshName);

	glm::mat4 modelView(1.0);
	mvStack.push(modelView);

	//below could be in main
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST); // enable depth testing
	//
}

void Renderer::render(std::vector<GameObject> gameObjs, glm::vec3 eye, glm::vec3 at, glm::vec3 up, GameObject * player)
{
	// set up projection matrix
	glm::mat4 projection(1.0);
	projection = glm::perspective(float(60.0f*DEG_TO_RADIAN), 800.0f / 600.0f, 1.0f, 50.0f);
	

	setCamera(eye, at, up, *player);

	renderSkyBox(projection);

	
	//glm::vec4 temp = mvStack.top() * glm::vec4(0.0f, 2.0f, -6.0f, 1.0f);
	//rt3d::setLightPos(shaderProg, glm::value_ptr(temp));

	glUseProgram(shaderProg);
	rt3d::setUniformMatrix4fv(shaderProg, "projection", glm::value_ptr(projection));
	for (auto gObj : gameObjs) {
		renderObject(gObj);
	}

	renderObject(*player);
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
	/*glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shaderProg, "textureUnit0"), 0);*/
	glBindTexture(GL_TEXTURE_2D, obj.getTexture());
	mvStack.push(mvStack.top());
	mvStack.top() = glm::translate(mvStack.top(), obj.getPos());
	mvStack.top() = glm::rotate(mvStack.top(), float(obj.getRotation() * DEG_TO_RADIAN), glm::vec3(0.0f, 1.0f, 0.0f));
	mvStack.top() = glm::rotate(mvStack.top(), float(180 * DEG_TO_RADIAN), glm::vec3(0.0f, 0.0f, 1.0f));
	mvStack.top() = glm::rotate(mvStack.top(), float(180 * DEG_TO_RADIAN), glm::vec3(1.0f, 0.0f, 0.0f));
	mvStack.top() = glm::scale(mvStack.top(), obj.getScale());
	rt3d::setUniformMatrix4fv(shaderProg, "modelview", glm::value_ptr(mvStack.top()));
	rt3d::drawIndexedMesh(obj.getMesh().getMeshId(), obj.getMesh().getMeshIndexCount(), GL_TRIANGLES);
	//
	mvStack.pop();
}

void Renderer::addTexture(char * fName)
{
	//textures.push_back(loadBitmap(fName));
	textures.insert({ fName, loadBitmap(fName) });
}

void Renderer::addMesh(char * fName)
{
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

	/*meshes.insert({ fName, std::make_pair(rt3d::createMesh(verts.size() / 3,
		verts.data(), nullptr, norms.data(),
		tex_coords.data(), meshIndexCount,
		indices.data()), meshIndexCount) });*/

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
	const char *cubeTexFiles[6] = {
		"Town-skybox/Town_up.bmp", "Town-skybox/Town_up.bmp", "Town-skybox/Town_up.bmp", "Town-skybox/Town_up.bmp", "Town-skybox/Town_up.bmp", "Town-skybox/Town_up.bmp"
	};
	GLuint skybox[5];
	//GLuint skyboxProgram = rt3d::initShaders("cubeMap.vert", "cubeMap.frag");

	loadCubeMap(cubeTexFiles, &skybox[0]);

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
	/*
	glUseProgram(skyBoxProg);				// skybox as single cube using cube map
	rt3d::setUniformMatrix4fv(skyBoxProg, "projection", glm::value_ptr(projection));
	glUseProgram(skyBoxProg);
	glDepthMask(GL_FALSE); // make sure writing to update depth test is off	
	rt3d::setUniformMatrix4fv(skyBoxProg, "projection", glm::value_ptr(projection));
	glm::mat3 mvRotOnlyMat3 = glm::mat3(mvStack.top());
	glDepthMask(GL_FALSE); // make sure writing to update depth test is off
	mvStack.push(glm::mat4(mvRotOnlyMat3));
	/*glm::mat3*//* mvRotOnlyMat3 = glm::mat3(mvStack.top());
	glCullFace(GL_FRONT); // drawing inside of cube!
	mvStack.push(glm::mat4(mvRotOnlyMat3));
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox[0]);
	glCullFace(GL_FRONT); // drawing inside of cube!
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(1.5f, 1.5f, 1.5f));
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox[0]);
	rt3d::setUniformMatrix4fv(skyBoxProg, "modelview", glm::value_ptr(mvStack.top()));
	mvStack.top() = glm::scale(mvStack.top(), glm::vec3(1.5f, 1.5f, 1.5f));
	Mesh cubeMesh = getMesh("cube.obj");
	rt3d::drawIndexedMesh(cubeMesh.getMeshId(), cubeMesh.getMeshIndexCount(), GL_TRIANGLES);
	rt3d::setUniformMatrix4fv(skyBoxProg, "modelview", glm::value_ptr(mvStack.top()));
	mvStack.pop();
	//Mesh cubeMesh = getMesh("cube.obj");
	rt3d::drawIndexedMesh(cubeMesh.getMeshId(), cubeMesh.getMeshIndexCount(), GL_TRIANGLES);
	glCullFace(GL_BACK); // drawing inside of cube!
	mvStack.pop();
	// back to remainder of rendering
	glCullFace(GL_BACK); // drawing inside of cube!

						 //glDepthMask(GL_TRUE); // make sure depth test is on									 // back to remainder of rendering
	glDepthMask(GL_TRUE); // make sure depth test is on
	*/
}
