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

// Fonts
Font* fontArial;
TextFormatting currentCharacterSet = PLAIN;

// Labels
std::vector<Label*> welcomeLabels;
std::vector<Label*> informationLabels;
std::vector<Label*> menuLabels;

Label lbWelcome;

Label lbFPS;
Label lbPlayerX;
Label lbPlayerY;
Label lbPlayerZ;
Label lbLightStrength;
Label lbYVelocity;
Label lbMouseX;
Label lbMouseY;
Label lbWorldSize;

Label lbSave;
Label lbLoad;
Label lbExit;

// Chunks Handler
ChunkHandler* chunkHandler;

// Physics
GLfloat playerLastY;

// Properties
//GLuint mapSideSize = 50;
//GLuint mapBorderSize = 25;

int randomMin = 0;
int randomMax = 100;

int fps = 0;
double fpsCounter = 0;
const int fpsCounterLimit = 1000;
int frames = 0;

bool spacePressed = false;
bool spaceReleased = false;

bool flyAscend = false;
bool flyDescend = false;

bool mouseFlag = false;

void lbSave_leftClick();
void lbLoad_leftClick();
void lbExit_leftClick();

void pgsGravity(GLfloat dt);
void pgsFixedHFly(GLfloat dt);
void pgsFreeFly(GLfloat dt);

GLuint loadTexture(GLchar* path);
void Render();
void geometryRender(Shader &shader, GLint textureID, glm::vec3 color, GLint blockTypeRequest);

void checkUICollision();

void initRenderData();
void initializeWorldmap();
void initializeUI();

float map(float value,
	float start1, float stop1,
	float start2, float stop2);

// Camera
//Camera camera(glm::vec3(10.0f, playerHeight, 10.0f));
glm::vec3 playerPos(glm::vec3(10.0f, 0.0f, 10.0f));
bool keys[1024];
int  actions[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
bool cursorFree = false;

// Y is set to 0.0 because player will automatically get the Y position from the heightMap

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;



GLuint stoneTexID;
GLuint grassTexID;

// Global lighting vars
GLfloat timeValue;
GLfloat strength;


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
		treatInputs();

		timeValue = glfwGetTime();
		strength = abs(sin(timeValue / 20));

		strength = 0.3;

		explorer.treatInputs(deltaTime);

		explorer.update(deltaTime);

		explorer.treatPhysics(deltaTime);

		// Setup some OpenGL options
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS); // Set to always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))

		// Clear the colorbuffer
		glClearColor(strength, strength, strength, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		explorer.render();


		auto finish = std::chrono::high_resolution_clock::now();
		fpsCounter += std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
		frames++;
		
		if (fpsCounter > fpsCounterLimit)
		{
			fps = frames;

			fpsCounter -= 1000;
			frames = 0;

			// Update FPS label with new FPS values
			lbFPS.setText("FPS: " + std::to_string(fps));
		}
	}

	glfwTerminate();
	return 0;
}



// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{	
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		if (gs == ACTIVE || gs == INFORMATION)
		{
			switch (pgs)
			{
				case GRAVITY:
					if (playerJumpEnable)
					{
						spacePressed = true;
						playerJumpEnable = false;
					}
					break;

				case FIXED_H_FLY:
					if (keys[GLFW_KEY_LEFT_SHIFT] &&
						actions[GLFW_KEY_W] == INPUT_RELEASED)
					{
						flyAscend = true;
						flyDescend = false;
					}
					else
					{
						flyAscend = false;
						flyDescend = true;
					}
					break;

				case FREE_FLY:
					break;

				default:
					break;
			}
		}
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
	{
		if (gs == ACTIVE || gs == INFORMATION)
		{
			switch (pgs)
			{
				case GRAVITY:
					spacePressed = false;
					break;

				case FIXED_H_FLY:
					flyAscend = false;
					flyDescend = false;
					break;

				case FREE_FLY:
					break;

				default:
					break;
			}
		}
	}

	if (action == GLFW_PRESS)
	{
		keys[key] = true;
		actions[key] = INPUT_PRESSED;
	}
	else if (action == GLFW_RELEASE)
	{
		keys[key] = false;
		actions[key] = INPUT_RELEASED;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	// Update mouse position labels with new position values
	lbMouseX.setText("Mouse X: " + std::to_string(lastX));
	lbMouseY.setText("Mouse Y: " + std::to_string(lastY));

	if (gs == ACTIVE || gs == INFORMATION)
	{
		camera.ProcessMouseMovement(xoffset, yoffset);
	}
	else
	{
		checkUICollision();
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (gs == MENU)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			std::vector<Label*>::iterator lbIt = menuLabels.begin();
			GLint i = 0;

			for (; lbIt < menuLabels.end(); lbIt++, i++)
			{
				if (menuLabels.at(i)->colliding)
				{
					menuLabels.at(i)->leftMouseClickFunction();
				}
			}
		}

		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		{
			std::vector<Label*>::iterator lbIt = menuLabels.begin();
			GLint i = 0;

			for (; lbIt < menuLabels.end(); lbIt++, i++)
			{
				if (menuLabels.at(i)->colliding)
				{
					menuLabels.at(i)->rightMouseClickFunction();
				}
			}
		}
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void character_callback(GLFWwindow* window, unsigned int codepoint)
{
	//char c = codepoint;
	//std::cout << c;
}

#pragma endregion
