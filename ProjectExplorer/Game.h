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
#include "FrustumC.h"

using namespace irrklang;

#define INPUT_PRESSED 0
#define INPUT_HOLD 1
#define INPUT_RELEASED 2

#define GRASS_ID 0
#define STONE_ID 1
#define SAND_ID  2


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



// GLFW Window
//GLfloat deltaTime = 0.0f;
//GLfloat lastFrame = 0.0f;


// FPS Counter
//int fps = 0;
//double fpsCounter = 0;
//const int fpsCounterLimit = 1000;
//int frames = 0;

// Input Handler

const glm::vec3 initialPosition(10.0f, 10.0f, 10.0f);

class Game
{
public:
	PLAYER_GRAVITY_STATE pgs;
	GAME_STATE gs;
	GAME_STATE lastGameState;

	//GLuint width, height;
	bool keys[1024];
	int  actions[1024];

	int fps;

	glm::mat4 view;
	glm::mat4 projection;

	Game(GLuint width, GLuint height);
	~Game();

	// Initialization
	void init(GLFWwindow* windowptr);

	// Game update
	void update(float dt);

	// Input Handler
	void treatInputs(float dt);
	void treatKeyboard(GLFWwindow* window, int key, int scancode, int action, int mode);
	void treatMouseMovement(GLFWwindow* window, double xpos, double ypos);
	void treatMouseButton(GLFWwindow* window, int button, int action, int mods);
	void treatMouseScroll(GLFWwindow* window, double xoffset, double yoffset);

	// Physics
	void treatPhysics(GLfloat dt);

	// Rendering
	void render();
private:
	GLFWwindow* windowptr;
	
	// Sound
	ISoundEngine* SoundEngine = createIrrKlangDevice();

	// Fonts
	Font* fontArial;

	// Chunks Handler
	ChunkHandler* chunkHandler;

	// Frustum Culling Handler
	FrustumC* frustumHandler;

	// Shaders
	Shader shaderGEO;
	Shader shaderLAMP;
	Shader shaderTXT;

	GLuint cubeVAO, cubeVBO;
	GLuint lightVAO;	// Use cubeVBO
	GLuint textVAO, textVBO;

	// Texture
	GLuint stoneTexID;
	GLuint grassTexID;
	GLuint sandTexID;
	GLuint waterTexID;

	// Camera
	Camera camera;

	// Player
	glm::vec3 playerPos;

	// General
	float dt;

	const float nearDistance =   0.1f;
	const float farDistance  = 750.0f;
	// todo - organize this vars
	const glm::vec3 stoneColor = glm::vec3(1.0, 1.0, 1.0);
	const glm::vec3 sandColor = glm::vec3(1.0, 1.0, 1.0);
	const glm::vec3 grassColor = glm::vec3(0.2, 0.7, 0.2);

	float gravityVelocity = 0.05f;
	const float maxGravityVelocity = 0.8f;
	const float gravityAcceleration = 0.5f;

	const float playerHeight = 2.2f;
	const float playerSpeed = 3.0f;
	const float runSpeedMultiplier = 4.0f;
	bool	playerJumpEnable = true;
	const int	playerJumpTime = 13;
	int   jumpTimeCounter = playerJumpTime;
	const float playerJumpAcceleration = 0.75f;

	bool spacePressed = false;
	bool spaceReleased = false;
	bool mouseClicked = false;
	bool mouseFlag = false;
	bool flyAscend = false;
	bool flyDescend = false;
	bool firstMouse = true;
	bool cursorFree = false;

	const float sunSize = 35.0f;
	// General
	int randomMin = 0;
	int randomMax = 100;

	const glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(1.0f, 250.0f, 1.0f);

	GLfloat playerLastY;
	GLfloat lastX = 400, lastY = 300;
	TextFormatting currentCharacterSet = PLAIN;

	// UI
	std::vector<Label*> welcomeLabels;
	std::vector<Label*> informationLabels;
	std::vector<Label*> menuLabels;

	Label lbWelcome;

	Label lbFPS;
	Label lbPlayerX;
	Label lbPlayerY;
	Label lbPlayerZ;
	Label lbYVelocity;
	Label lbMouseX;
	Label lbMouseY;
	Label lbWorldSize;
	Label lbVisibleChunks;
	Label lbNearChunks;

	Label lbSave;
	Label lbLoad;
	Label lbExit;

	const GLuint screenWidth, screenHeight;

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