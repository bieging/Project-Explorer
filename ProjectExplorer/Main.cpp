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

using namespace irrklang;

ISoundEngine *SoundEngine = createIrrKlangDevice();

int aux = 0;

// Properties
GLuint screenWidth = 800, screenHeight = 600;
//GLuint mapSideSize = 50;
//GLuint mapBorderSize = 25;

int randomMin = 0;
int randomMax = 100;

GLint textXSize = 0;

int fps = 0;
double fpsCounter = 0;
const int fpsCounterLimit = 1000;
int frames = 0;

bool spacePressed = false;
bool shiftPressed = false;
bool spaceReleased = false;
bool shiftReleased = false;

int jumpTimeCounter = playerJumpTime;

GLfloat runSpeedMultiplier = 3.0f;

bool flyAscend = false;
bool flyDescend = false;

bool mouseFlag = false;

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

PLAYER_GRAVITY_STATE pgs = GRAVITY;
GAME_STATE gs = WELCOME;
GAME_STATE lastGameState = WELCOME;

// Player
GLfloat gravityVelocity = 0.05f;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void character_callback(GLFWwindow* window, unsigned int codepoint);
void treatInputs();
void treatPlayerMovementKeys();
void treatGameControlKeys();
void treatStateChangingKeys();
void updatePlayerVelocity(GLfloat dt);

void lbSave_leftClick();
void lbLoad_leftClick();
void lbExit_leftClick();

void pgsGravity(GLfloat dt);
void pgsFixedHFly(GLfloat dt);
void pgsFreeFly(GLfloat dt);

//void RenderText(Shader &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
GLuint loadTexture(GLchar* path);
void initRenderData();
//void initTextRenderData();
void RawRender(Shader &shader, GLint textureID, glm::vec3 color, GLint blockTypeRequest);
void Render();

//std::tuple<GLfloat, GLfloat> uiTextDimension(std::string text, GLfloat x, GLfloat y, GLfloat scale);
void uiCollision();

void initializePerlinNoise();
void initializeWorldVectors();
void initializeUI();

float map(float value,
	float start1, float stop1,
	float start2, float stop2);

// Camera
Camera camera(glm::vec3(10.0f, playerHeight, 10.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
bool cursorFree = false;

// Y is set to 0.0 because player will automatically get the Y position from the heightMap
glm::vec3 playerPos(glm::vec3(10.0f, 0.0f, 10.0f));

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

std::vector<glm::vec3> grassBlocks;
std::vector<glm::vec3> stoneBlocks;

std::vector<glm::vec3> heightMapPos;
//std::vector<GLint> heightValue;

GLuint cubeVAO, cubeVBO;
GLuint lightVAO;	// Use cubeVBO
GLuint textVAO, textVBO;

GLuint stoneTexID;
GLuint grassTexID;

glm::vec3 stoneColor  = glm::vec3(1.0, 1.0, 1.0);
glm::vec3 grassColor = glm::vec3(0.2, 0.7, 0.2);

// Shaders
Shader shaderGEO;
Shader shaderLAMP;
Shader shaderTXT;

// Global lighting vars
GLfloat timeValue;
GLfloat strength;

// GLFW Window
GLFWwindow* window;

// Fonts
Font fontArial;
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

// Perlin Terrain Generator
PerlinNoise perlin;
GLuint seed = 0;

// Chunks Handler
ChunkHandler chunkHandler;

// Physics
GLfloat playerLastY;

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

	// Setup and compile our shaders
	shaderGEO.init("Shaders/geo.vs", "Shaders/geo.frag");
	shaderLAMP.init("Shaders/lamp.vs", "Shaders/lamp.frag");
	shaderTXT.init("Shaders/text.vs", "Shaders/text.frag");

	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(screenWidth), 0.0f, static_cast<GLfloat>(screenHeight));
	
	shaderTXT.Use();
	glUniformMatrix4fv(glGetUniformLocation(shaderTXT.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	// Play audio
	//SoundEngine->play2D("Audio/breakout.mp3", GL_TRUE);

#pragma region "object_initialization"
	// Load fonts
	fontArial = Font("Fonts", "arial");

	// Configure terrain generator
	initializePerlinNoise();
	
	// Create map and heightMap
	initializeWorldVectors();

	// Configure all interface related objects
	initializeUI();

	// Initializes all vertices, VAOs and VBOs related to blocks
	initRenderData();

	// Initialize player's Y position
	GLint heightValue = chunkHandler.getHeightValue(playerPos.x, playerPos.z);
	playerPos.y = heightValue;
	gravityVelocity = 0.05f;
	playerLastY = playerPos.y;

	// Load textures
	stoneTexID = loadTexture("Textures/rockWire.png");
	grassTexID  = loadTexture("Textures/grassRealWire.png");

#pragma endregion

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

		//if (strength <= 0.1)
		//	strength = 0.1;
		//if (strength > 0.7)
			strength = 0.3;

		updatePlayerVelocity(deltaTime);

		lbLightStrength.setText("Ambient Light Strength: " + std::to_string(strength));
		lbYVelocity.setText("Y Velocity: " + std::to_string(gravityVelocity));

		// Setup some OpenGL options
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS); // Set to always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))

		// Clear the colorbuffer
		glClearColor(strength, strength, strength, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw objects
		view = camera.GetViewMatrix();
		projection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 750.0f);

		shaderGEO.Use();
		glUniformMatrix4fv(glGetUniformLocation(shaderGEO.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shaderGEO.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform1f(glGetUniformLocation(shaderGEO.Program, "ambStr"), strength);

		shaderLAMP.Use();
		glUniformMatrix4fv(glGetUniformLocation(shaderLAMP.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shaderLAMP.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform1f(glGetUniformLocation(shaderLAMP.Program, "ambStr"), strength);
		
		// Draw geometry and text
		Render();

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

void updatePlayerVelocity(GLfloat dt)
{
	switch (pgs)
	{
		case GRAVITY:
			pgsGravity(dt);
			break;
		case FIXED_H_FLY:
			pgsFixedHFly(dt);
			break;
		case FREE_FLY:
			pgsFreeFly(dt);
			break;
		default:
			break;
	}
}

void pgsGravity(GLfloat dt)
{
	if (spacePressed == false)
	{
		if (spaceReleased)
		{
			// Checks if player stopped ascending after releasing space key
			if (gravityVelocity >= 0)
			{
				// This softly stops the player after stops the jump command
				gravityVelocity -= playerJumpAcceleration * dt;
				playerPos.y += gravityVelocity;
			}
			else
			{
				// Once player stops, let gravity act by setting spaceReleased to false
				spaceReleased = false;
				gravityVelocity = 0;
			}
		}
		else
		{
			GLint heightValue = chunkHandler.getHeightValue(playerPos.x, playerPos.z);

			if (playerPos.y > heightValue)
			{
				if (gravityVelocity < maxGravityVelocity)
				{
					gravityVelocity += gravityAcceleration * dt;
				}
				else
				{
					gravityVelocity = maxGravityVelocity;
				}

				playerPos.y -= gravityVelocity;
			}
			else	// Player touched the ground
			{
				playerPos.y = heightValue;

				gravityVelocity = 0.05f;

				playerJumpEnable = true;
			}
		}
	}
	else
	{
		if (gravityVelocity < maxGravityVelocity)
		{
			gravityVelocity += 0.2 * dt;
		}
		else
		{
			gravityVelocity = maxGravityVelocity;
		}

		playerPos.y += gravityVelocity;

		jumpTimeCounter -= dt;

		if (jumpTimeCounter <= 0)
		{
			spacePressed = false;
			spaceReleased = true;

			jumpTimeCounter = playerJumpTime;
		}
	}

	camera.Position.y = playerPos.y + playerHeight;
}

void pgsFixedHFly(GLfloat dt)
{
	if (flyAscend)
	{
		playerPos.y += 2 * dt;
	}
	else if (flyDescend)
	{
		playerPos.y -= 2 * dt;
	}

	camera.Position.y = playerPos.y + playerHeight;
}

void pgsFreeFly(GLfloat dt)
{
}

// This function loads a texture from file. Note: texture loading functions like these are usually 
// managed by a 'Resource Manager' that manages all resources (like textures, models, audio). 
// For learning purposes we'll just define it as a utility function.
GLuint loadTexture(GLchar* path)
{
	//Generate texture ID and load texture data 
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;
}

#pragma region Initialization

void initializePerlinNoise()
{
	std::cout << "Please enter a seed number to generate your terrain: " << std::endl;
	std::cout << "Enter \"n\" to initialize with standard seed" << std::endl;
	std::string seedInput = "";
	std::cin >> seedInput;
	if (seedInput != "n")
	{
		seed = std::stoi(seedInput);
		//perlin = PerlinNoise(seed);
	}
}

void initializeWorldVectors()
{
	std::cout << "Initializing map, please wait" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();

	chunkHandler = ChunkHandler(playerPos.x, playerPos.z, seed);

	auto finish = std::chrono::high_resolution_clock::now();
	int initializeTime = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
	std::cout << "Map took: " << std::to_string(initializeTime) << " miliseconds to initialize" << std::endl;
	std::cout << std::endl;
}

void initializeUI()
{
	std::cout << "Initializing UI, please wait" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();

	// Create Welcome Label
	lbWelcome = Label("Welcome to Project Explorer", 100.0f, 300.0f, 1.0f, fontArial.getCharacterSet(PLAIN));

	// Add Welcome Labels to their Label vector
	welcomeLabels.push_back(&lbWelcome);

	// Create Information Labels
	lbFPS			= Label("FPS: "						+ std::to_string(fps),							25.0f, 570.0f, 0.3f, fontArial.getCharacterSet(PLAIN));
	lbPlayerX		= Label("Player X: "				+ std::to_string(camera.Position.x),			25.0f, 550.0f, 0.3f, fontArial.getCharacterSet(PLAIN));
	lbPlayerY		= Label("Player Y: "				+ std::to_string(camera.Position.y),			25.0f, 530.0f, 0.3f, fontArial.getCharacterSet(PLAIN));
	lbPlayerZ		= Label("Player Z: "				+ std::to_string(camera.Position.z),			25.0f, 510.0f, 0.3f, fontArial.getCharacterSet(PLAIN));
	lbLightStrength = Label("Ambient Light Strength: "	+ std::to_string(strength),						25.0f, 490.0f, 0.3f, fontArial.getCharacterSet(PLAIN));
	lbYVelocity		= Label("Y Velocity: "				+ std::to_string(gravityVelocity),				25.0f, 470.0f, 0.3f, fontArial.getCharacterSet(PLAIN));
	lbMouseX		= Label("Mouse X: "					+ std::to_string(lastX),						25.0f, 450.0f, 0.3f, fontArial.getCharacterSet(PLAIN));
	lbMouseY		= Label("Mouse Y: "					+ std::to_string(lastY),						25.0f, 430.0f, 0.3f, fontArial.getCharacterSet(PLAIN));
	lbWorldSize		= Label("World Size: "				+ std::to_string(chunkHandler.chunks.size()),	25.0f, 410.0f, 0.3f, fontArial.getCharacterSet(PLAIN));

	// Add Information Labels to their Label vector
	informationLabels.push_back(&lbFPS);
	informationLabels.push_back(&lbPlayerX);
	informationLabels.push_back(&lbPlayerY);
	informationLabels.push_back(&lbPlayerZ);
	informationLabels.push_back(&lbLightStrength);
	informationLabels.push_back(&lbYVelocity);
	informationLabels.push_back(&lbMouseX);
	informationLabels.push_back(&lbMouseY);
	informationLabels.push_back(&lbWorldSize);

	// Create Menu Labels
	lbSave = Label("Save", 350.0f, 570.0f, 0.5f, fontArial.getCharacterSet(currentCharacterSet));
	lbLoad = Label("Load", 350.0f, 510.0f, 0.5f, fontArial.getCharacterSet(currentCharacterSet));
	lbExit = Label("Exit", 350.0f, 450.0f, 0.5f, fontArial.getCharacterSet(currentCharacterSet));

	// Set function for each button
	lbSave.setLeftMouseClickFunction(lbSave_leftClick);
	lbLoad.setLeftMouseClickFunction(lbLoad_leftClick);
	lbExit.setLeftMouseClickFunction(lbExit_leftClick);

	// Add Menu Labels to their Label vector
	menuLabels.push_back(&lbSave);
	menuLabels.push_back(&lbLoad);
	menuLabels.push_back(&lbExit);

	auto finish = std::chrono::high_resolution_clock::now();
	int initializeTime = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
	std::cout << "UI took: " << std::to_string(initializeTime) << " miliseconds to initialize" << std::endl;
	std::cout << std::endl;
}

void initRenderData()
{
	std::cout << "Initializing render data, please wait" << std::endl;
	auto start = std::chrono::high_resolution_clock::now();

	GLfloat cubeVertices[] =
	{
		// Positions          // Tex	   // Normals
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f
	};

	// Setup cube VAO
	//glGenVertexArrays(1, &cubeVAO);
	//glGenBuffers(1, &cubeVBO);

	//glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);

	//glBindVertexArray(cubeVAO);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//glBindVertexArray(0);


	glGenVertexArrays(1, &cubeVAO);
	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &cubeVBO);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	glBindVertexArray(lightVAO);
	// we only need to bind to the VBO, the container's VBO's data already contains the correct data.
	// set the vertex attributes (only position data for our lamp)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	//// Instancing test
	//unsigned int buffer;
	//glGenBuffers(1, &buffer);
	//glBindBuffer(GL_ARRAY_BUFFER, buffer);
	//glBufferData(GL_ARRAY_BUFFER, chunkHandler.numberOfVisibleCubes * sizeof(glm::mat4), &chunkHandler.translations[0], GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//// vertex Attributes
	//GLsizei vec4Size = sizeof(glm::vec4);
	//glEnableVertexAttribArray(2);
	//glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
	//glEnableVertexAttribArray(3);
	//glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(vec4Size));
	//glEnableVertexAttribArray(4);
	//glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
	//glEnableVertexAttribArray(5);
	//glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

	//glVertexAttribDivisor(2, 1);
	//glVertexAttribDivisor(3, 1);
	//glVertexAttribDivisor(4, 1);
	//glVertexAttribDivisor(5, 1);
	//glBindVertexArray(0);

	auto finish = std::chrono::high_resolution_clock::now();
	int initializeTime = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
	std::cout << "Render data took: " << std::to_string(initializeTime) << " microseconds to initialize" << std::endl;
	std::cout << std::endl;
}

#pragma endregion

#pragma region "Render"

void RawRender(Shader &shader, GLint textureID, glm::vec3 color, GLint blockTypeRequest)
{
	int blockX;
	int blockY;
	int blockZ;
	int blockType;

	Chunk * chunkPtr;

	// Draw sun
	shaderLAMP.Use();
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(lightVAO);

	GLfloat sunX = sin(timeValue / 10);
	GLfloat sunY = cos(timeValue / 10);

	sunX = map(sunX, 0, 1, 0, 500);
	sunY = map(sunY, 0, 1, 0, 500);

	lightPos = glm::vec3(sunX + playerPos.x, sunY + playerPos.y, playerPos.z);

	glm::mat4 model = glm::mat4();
	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(25.0f));
	glUniformMatrix4fv(glGetUniformLocation(shaderLAMP.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

	shader.Use();
	// Draw worldmap
	glBindVertexArray(cubeVAO);
	glBindTexture(GL_TEXTURE_2D, textureID);  // We omit the glActiveTexture part since TEXTURE0 is already the default active texture unit. (sampler used in fragment is set to 0 as well as default)		

	GLint shObjectColor = glGetUniformLocation(shader.Program, "objectColor");
	glUniform3f(shObjectColor, color.r, color.g, color.b);
	GLint shLightColor = glGetUniformLocation(shader.Program, "lightColor");
	glUniform3f(shLightColor, lightColor.r, lightColor.g, lightColor.b);
	GLint lightPosLoc = glGetUniformLocation(shader.Program, "lightPos");
	glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
	GLint viewPosLoc = glGetUniformLocation(shader.Program, "viewPos");
	glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

	//glDrawArraysInstanced(GL_TRIANGLES, 0, 36, chunkHandler.numberOfVisibleCubes);
	//glBindVertexArray(0);

	for (int i = 0; i < chunkHandler.visibleChunks.size(); i++)
	{
		chunkPtr = &chunkHandler.chunks.at(chunkHandler.visibleChunks.at(i));
		
		for (int x = 0; x < chunkSize; x++)
		{
			for (int z = 0; z < chunkSize; z++)
			{
				int randNum = randomMin + (rand() % (int)(randomMax - randomMin + 1));

				blockX = chunkPtr->x + x;
				blockY = chunkPtr->blocksHeight[x][z];
				blockZ = chunkPtr->z + z;
				blockType = chunkPtr->blocksType[x][z];

				if (blockType == blockTypeRequest)
				{
					glm::mat4 model;
					//model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
					model = glm::translate(model, glm::vec3(blockX + 0.5f, blockY, blockZ + 0.5f));
					//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
					glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
					glDrawArrays(GL_TRIANGLES, 0, 36);
				}
			}
		}
	}

	glBindVertexArray(0);
}

// todo - create comment for this
float map(float value,
	float start1, float stop1,
	float start2, float stop2)
{
	return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
}

void Render()
{
	if (gs == ACTIVE)
	{
		// Used to render as wireframe
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		RawRender(shaderGEO, grassTexID, grassColor, GRASS_ID);
		RawRender(shaderGEO, stoneTexID, stoneColor, STONE_ID);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// Set OpenGL options
		//glEnable(GL_CULL_FACE);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//std::vector<Label>::iterator lbIt = activeLabels.begin();
		//GLint i = 0;

		//for (; lbIt < activeLabels.end(); lbIt++, i++)
		//{
		//	activeLabels.at(i).render(shaderTXT, glm::vec3(1.0f, 1.0f, 1.0f));
		//}

		//glDisable(GL_BLEND);
		//glDisable(GL_CULL_FACE);

	}
	else if (gs == WELCOME)
	{
		RawRender(shaderGEO, grassTexID, grassColor, GRASS_ID);
		RawRender(shaderGEO, stoneTexID, stoneColor, STONE_ID);

		// Set OpenGL options
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		std::vector<Label*>::iterator lbIt = welcomeLabels.begin();
		GLint i = 0;

		for (; lbIt < welcomeLabels.end(); lbIt++, i++)
		{
			welcomeLabels.at(i)->render(shaderTXT, glm::vec3(1.0f, 1.0f, 1.0f));
		}

		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
	}
	else if (gs == INFORMATION)
	{
		RawRender(shaderGEO, grassTexID, grassColor, GRASS_ID);
		RawRender(shaderGEO, stoneTexID, stoneColor, STONE_ID);

		// Set OpenGL options
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		std::vector<Label*>::iterator lbIt = informationLabels.begin();
		GLint i = 0;

		for (; lbIt < informationLabels.end(); lbIt++, i++)
		{
			informationLabels.at(i)->render(shaderTXT, glm::vec3(1.0f, 0.0f, 0.0f));
		}

		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
	}
	else if (gs == MENU)
	{
		RawRender(shaderGEO, grassTexID, grassColor, GRASS_ID);
		RawRender(shaderGEO, stoneTexID, stoneColor, STONE_ID);

		// Set OpenGL options
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		std::vector<Label*>::iterator lbIt = menuLabels.begin();
		GLint i = 0;

		for (; lbIt < menuLabels.end(); lbIt++, i++)
		{
			menuLabels.at(i)->render(shaderTXT, glm::vec3(1.0f, 0.0f, 0.0f));
		}

		lbIt = informationLabels.begin();
		i = 0;

		for (; lbIt < informationLabels.end(); lbIt++, i++)
		{
			informationLabels.at(i)->render(shaderTXT, glm::vec3(1.0f, 0.0f, 0.0f));
		}

		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
	}



	// Swap the buffers
	glfwSwapBuffers(window);
}

void uiCollision()
{
	if (cursorFree)
	{
		if (gs == MENU)
		{
			std::vector<Label*>::iterator lbIt = menuLabels.begin();
			GLint i = 0;

			for (; lbIt < menuLabels.end(); lbIt++, i++)
			{
				menuLabels.at(i)->checkCollision(lastX, screenHeight - lastY);
			}
		}
	}
}

#pragma endregion

#pragma region "User input"

// Moves/alters the camera positions based on user input
void treatInputs()
{
	treatPlayerMovementKeys();
	treatGameControlKeys();
	treatStateChangingKeys();
}

void treatPlayerMovementKeys()
{
	GLfloat lastCameraX = camera.Position.x;
	GLfloat lastCameraZ = camera.Position.z;

	// Camera controls
	if (keys[GLFW_KEY_W])
	{
		if (shiftPressed)	camera.ProcessKeyboard(FORWARD, deltaTime * runSpeedMultiplier);
		else				camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (keys[GLFW_KEY_S])
	{
		if (shiftPressed)	camera.ProcessKeyboard(BACKWARD, deltaTime * runSpeedMultiplier);
		else				camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (keys[GLFW_KEY_A])
	{
		if (shiftPressed)	camera.ProcessKeyboard(LEFT, deltaTime * runSpeedMultiplier);
		else				camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (keys[GLFW_KEY_D])
	{
		if (shiftPressed)	camera.ProcessKeyboard(RIGHT, deltaTime * runSpeedMultiplier);
		else				camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	GLint heightValue = chunkHandler.getHeightValue(camera.Position.x, camera.Position.z);

	// Player Wall Collision Physics Treatment
	// todo - explain what is going on here
	if (heightValue < (camera.Position.y - playerHeight + 0.5f))
	{
		playerPos.x = camera.Position.x;
		playerPos.z = camera.Position.z;

		chunkHandler.updatePlayerPosition(playerPos.x, playerPos.z);

		// Update player position labels with new position values
		lbPlayerX.setText("Player X: " + std::to_string(camera.Position.x));
		lbPlayerY.setText("Player Y: " + std::to_string(camera.Position.y));
		lbPlayerZ.setText("Player Z: " + std::to_string(camera.Position.z));

		// Update world size label in case new chunks were added
		lbWorldSize.setText("World Size: " + std::to_string(chunkHandler.chunks.size()));
	}
	else
	{
		camera.Position.x = lastCameraX;
		camera.Position.z = lastCameraZ;
	}
}

void treatGameControlKeys()
{

}

void treatStateChangingKeys()
{

}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		if (gs != MENU)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			cursorFree = true;
			lastGameState = gs;
			gs = MENU;
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			cursorFree = false;
			if (lastGameState == gs)
			{
				gs = ACTIVE;
			}
			else
			{
				gs = lastGameState;
			}
		}
	}

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		if (gs == WELCOME)
		{
			gs = ACTIVE;
		}
	}

	if (key == GLFW_KEY_F3 && action == GLFW_PRESS)
	{
		if (gs != INFORMATION)
		{
			lastGameState = gs;
			gs = INFORMATION;
		}
		else
		{
			if (lastGameState == gs)
			{
				gs = ACTIVE;
			}
			else
			{
				gs = lastGameState;
			}
		}
	}
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		if (gs == ACTIVE)
		{
			playerPos = glm::vec3(25.0, 25.0, 25.0);
			initializeWorldVectors();
		}
	}
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{
		if (gs == ACTIVE)
		{
			switch (pgs)
			{
				case GRAVITY:
					pgs = FIXED_H_FLY;
					break;
				case FIXED_H_FLY:
					pgs = FREE_FLY;
					break;
				case FREE_FLY:
					pgs = GRAVITY;
					break;
				default:
					break;
			}
		}
	}
	
	if		(key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS  )	shiftPressed = true;
	else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)	shiftPressed = false;
	
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
					if (!shiftPressed)
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
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;
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
		uiCollision();
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

void lbSave_leftClick()
{
	std::cout << "Saved map" << std::endl;
}

void lbLoad_leftClick()
{
	std::cout << "Loaded new map" << std::endl;
}

void lbExit_leftClick()
{
	glfwSetWindowShouldClose(window, GL_TRUE);
}

#pragma endregion
