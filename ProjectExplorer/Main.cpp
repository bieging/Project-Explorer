// Std. Includes
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <chrono>
#include <algorithm>

// GLEW
#define GLEW_STATIC
#include <c:/opengl/GLEW/glew.h>

// GLFW
#include <c:/opengl/GLFW/glfw3.h>

// IRRKLANG
#include <c:/opengl/IRRKLANG/irrKlang.h>

// GL includes
#include "Game.h"
#include "Main.h"
#include "Shader.h"
#include "Camera.h"
#include "Renderer.h"
#include "Font.h"
#include "Label.h"
#include "PerlinNoise.h"
//#include "BMath.h"
#include "ChunkHandler.h"
#include "Defines.h"

// GLM Mathemtics
#include <c:/opengl/glm/glm.hpp>
#include <c:/opengl/glm/gtc/matrix_transform.hpp>
#include <c:/opengl/glm/gtc/type_ptr.hpp>

// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

// Other Libs
#include <c:/opengl/SOIL/SOIL.h>

// GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void character_callback(GLFWwindow* window, unsigned int codepoint);

using namespace irrklang;
ISoundEngine *SoundEngine = createIrrKlangDevice();

// GLFW Window
GLFWwindow* window;
GLuint screenWidth = 800;
GLuint screenHeight = 600;

// Game instance
Game explorer(screenWidth, screenHeight);

int fps = 0;
double fpsCounter = 0;
const int fpsCounterLimit = 1000;
int frames = 0;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// The MAIN function, from here we start our application and run our Game loop
int main()
{
	// Init GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(screenWidth, screenHeight, "Project Explorer", nullptr/*glfwGetPrimaryMonitor()*/, nullptr); // Windowed
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCharCallback(window, character_callback);

	// Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, screenWidth, screenHeight);

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); // Set to always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))

	explorer.init(window);

	// Play audio
	//SoundEngine->play2D("Audio/breakout.mp3", GL_TRUE);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		auto start = std::chrono::high_resolution_clock::now();
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check and call events
		glfwPollEvents();

		explorer.treatInputs(deltaTime);

		explorer.update(deltaTime);

		explorer.treatPhysics(deltaTime);
		
		explorer.render();

		auto finish = std::chrono::high_resolution_clock::now();
		fpsCounter += std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
		frames++;
		
		if (fpsCounter > fpsCounterLimit)
		{
			fps = frames;
			explorer.fps = fps;

			fpsCounter -= 1000;
			frames = 0;
		}
	}

	glfwTerminate();
	return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{	
	explorer.treatKeyboard(window, key, scancode, action, mode);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	explorer.treatMouseMovement(window, xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	explorer.treatMouseButton(window, button, action, mods);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	explorer.treatMouseScroll(window, xoffset, yoffset);
}

void character_callback(GLFWwindow* window, unsigned int codepoint)
{
	//char c = codepoint;
	//std::cout << c;
}

#pragma endregion
