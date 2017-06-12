#ifndef GAME_H
#define GAME_H

#pragma once

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

// GLM Mathemtics
#include <c:/opengl/glm/glm.hpp>
#include <c:/opengl/glm/gtc/matrix_transform.hpp>
#include <c:/opengl/glm/gtc/type_ptr.hpp>

// FreeType - Font Library
#include <ft2build.h>
#include FT_FREETYPE_H

// SOIL - Image Loading Library
#include <c:/opengl/SOIL/SOIL.h>

// IRRKLANG - Sound Library
#include <c:/opengl/IRRKLANG/irrKlang.h>

// Own includes
#include "Shader.h"
#include "Camera.h"
#include "Renderer.h"
#include "Font.h"
#include "Label.h"
#include "PerlinNoise.h"
#include "BMath.h"
#include "ChunkHandler.h"
#include "Defines.h"

using namespace irrklang;

#define INPUT_PRESSED 0
#define INPUT_HOLD 1
#define INPUT_RELEASED 2

#define GRASS_ID 0
#define STONE_ID 1

// UI
void lbSave_leftClick();
void lbLoad_leftClick();
void lbExit_leftClick();


// Texture
GLuint loadTexture(GLchar* path);

enum PLAYER_GRAVITY_STATE
{
	GRAVITY,
	FIXED_H_FLY,
	FREE_FLY
};

enum GAME_STATE
{
	WELCOME,
	START,
	MENU,
	ACTIVE,
	INFORMATION
};

// UI
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

TextFormatting currentCharacterSet = PLAIN;

// Physics
GLfloat playerLastY;

// GLFW Window
GLuint screenWidth = 800, screenHeight = 600;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// General
int randomMin = 0;
int randomMax = 100;

// FPS Counter
int fps = 0;
double fpsCounter = 0;
const int fpsCounterLimit = 1000;
int frames = 0;

// Input Handler
GLfloat lastX = 400, lastY = 300;
bool spacePressed = false;
bool spaceReleased = false;
bool mouseFlag = false;
bool flyAscend = false;
bool flyDescend = false;
bool firstMouse = true;
bool cursorFree = false;

// Camera
Camera camera(glm::vec3(10.0f, playerHeight, 10.0f));

// Player
glm::vec3 playerPos(glm::vec3(10.0f, 0.0f, 10.0f));

// Rendering
GLuint cubeVAO, cubeVBO;
GLuint lightVAO;	// Use cubeVBO
GLuint textVAO, textVBO;

// Texture
GLuint stoneTexID;
GLuint grassTexID;

class Game
{
public:
	PLAYER_GRAVITY_STATE pgs;
	GAME_STATE gs;
	GAME_STATE lastGameState;

	GLuint width, height;
	bool keys[1024];
	int  actions[1024];

	Game(GLuint width, GLuint height);
	~Game();

	// Initialization
	void init(GLFWwindow* windowptr);

	// Game update
	void update(float dt);

	// Input Handler
	void treatInputs(float dt);

	// Physics
	void treatPhysics(GLfloat dt);

	// Rendering
	void render();
private:
	GLFWwindow* windowptr;

	// Shaders
	Shader shaderGEO;
	Shader shaderLAMP;
	Shader shaderTXT;

	GLuint cubeVAO, cubeVBO;
	GLuint lightVAO;	// Use cubeVBO
	GLuint textVAO, textVBO;

	// Initialization
	void initRenderData();
	void initializeWorldmap();
	void initializeUI();
	GLuint loadTexture(GLchar* path);

	// Game update
	void updateUI();
	
	// Input Handler
	void treatPlayerMovementKeys(float dt);
	void treatGameControlKeys(float dt);
	void treatStateChangingKeys(float dt);
	void checkUICollision();

	// Physics
	void pgsGravity(GLfloat dt);
	void pgsFixedHFly(GLfloat dt);
	void pgsFreeFly(GLfloat dt);

	// Rendering
	void geometryRender(Shader &shader, GLint textureID, glm::vec3 color, GLint blockTypeRequest);
};

#endif