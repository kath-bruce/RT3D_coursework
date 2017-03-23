
#if _DEBUG
#pragma comment(linker, "/subsystem:\"console\" /entry:\"WinMainCRTStartup\"")
#endif

#include "rt3d.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stack> //need this???
#include <vector>
#include "Scene.h"

Scene * scene;

// Set up rendering context
SDL_Window * setupRC(SDL_GLContext &context) {
	SDL_Window * window;
	if (SDL_Init(SDL_INIT_VIDEO) < 0) // Initialize video
		rt3d::exitFatalError("Unable to initialize SDL");

	// Request an OpenGL 3.0 context.
	// Not able to use SDL to choose profile (yet), should default to core profile on 3.2 or later
	// If you request a context not supported by your drivers, no OpenGL context will be created

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);  // double buffering on
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // Turn on x4 multisampling anti-aliasing (MSAA)

	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8); // 8 bit alpha buffering

											   // Create 800x600 window
	window = SDL_CreateWindow("SDL/GLM/OpenGL Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (!window) // Check window was created OK
		rt3d::exitFatalError("Unable to create window");

	context = SDL_GL_CreateContext(window); // Create opengl context and attach to window
	SDL_GL_SetSwapInterval(1); // set swap buffers to sync with monitor's vertical refresh rate
	return window;
}

void init() {
	std::vector<char *> textures;
	std::vector<char *> meshes;
	textures.push_back("camouflage.bmp");
	textures.push_back("fabric.bmp");
	textures.push_back("fox.bmp");
	meshes.push_back("cube.obj");
	meshes.push_back("house.obj");
	meshes.push_back("foxOriginal.md2");

	scene = new Scene("phong-tex.vert", "phong-tex.frag", textures, meshes, "yoshi.bmp", "yoshi.md2");

	scene->addGameObject("house", glm::vec3(2.0f, 3.0f, -4.0f), glm::vec3(0.05f, 0.05f, 0.05f), "fabric.bmp", "house.obj");
}

void update() {
	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_W]) scene->movePlayerForward(0.1f);
	if (keys[SDL_SCANCODE_S]) scene->movePlayerForward(-0.1f);
	if (keys[SDL_SCANCODE_A]) scene->movePlayerRight(-0.1f);
	if (keys[SDL_SCANCODE_D]) scene->movePlayerRight(0.1f);
	//if (keys[SDL_SCANCODE_ESCAPE] )
}

void draw(SDL_Window * hWindow) {
	// clear the screen
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//render scene
	scene->renderScene();

	SDL_GL_SwapWindow(hWindow); // swap buffers
}

int main(int argc, char *argv[]) {
	SDL_Window * hWindow;
	SDL_GLContext glContext;
	hWindow = setupRC(glContext);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) { // glewInit failed, something is seriously wrong
		std::cout << "glewInit failed, aborting." << std::endl;
		exit(1);
	}
	std::cout << glGetString(GL_VERSION) << std::endl;

	init();

	bool running = true; // set running to true
	SDL_Event sdlEvent;  // variable to detect SDL events
	while (running) {	// the event loop
		while (SDL_PollEvent(&sdlEvent)) {
			if (sdlEvent.type == SDL_QUIT || /*sdlEvent.type == SDLK_ESCAPE*/ sdlEvent.type == SDL_KEYDOWN && sdlEvent.key.keysym.sym == SDLK_ESCAPE) 
			{
				running = false;
			}

			if (sdlEvent.type == SDL_MOUSEMOTION)
			{
				SDL_SetRelativeMouseMode(SDL_TRUE);
				SDL_WarpMouseInWindow(NULL, 800, 600);
				/* If the mouse is moving to the left */
				if (sdlEvent.motion.xrel < 0)
					scene->updatePlayerR(-1.5f);
				/* If the mouse is moving to the right */
				else if (sdlEvent.motion.xrel > 0)
					scene->updatePlayerR(1.5f);
			}
		}
		update();
		draw(hWindow); // call the draw function
	}

	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(hWindow);
	SDL_Quit();
	delete scene;
	return 0;
}