#ifndef LABEL_H
#define LABEL_H

#pragma once

#include <string>
#include <map>

#include "Shader.h"
#include "Character.h"

// GLEW
#define GLEW_STATIC
#include <c:/opengl/GLEW/glew.h>

// GLFW
#include <c:/opengl/GLFW/glfw3.h>

// GLM Mathemtics
#include <c:/opengl/glm/glm.hpp>

enum TextFormatting
{
	PLAIN,
	BOLD,
	ITALIC
};

enum TextPosition
{
	TXT_POS_TOP = -5,
	TXT_POS_CENTER = -4,
	TXT_POS_LEFT = -3,
	TXT_POS_RIGHT = -2,
	TXT_POS_BOTTOM = -1
};

class Label
{
private:
	enum FUNC_TYPE
	{
		VOID_VOID = 0,
		VOID_GLFWWINDOW
	};

	std::string text;

	GLuint textVAO, textVBO;

	GLfloat width, height;
	GLfloat xPos, yPos;
	GLfloat scale;

	std::map<GLchar, Character> charactersSet;

	TextFormatting tf;

	bool initialized;

	void calculateDimension();

	typedef void(*mouseClickFunction)(void);
public:
	bool colliding;

	static GLfloat windowWidth, windowHeight;
	
	Label();
	Label(std::string text, GLfloat xPos, GLfloat yPos, GLfloat scale, std::map<GLchar, Character> * charactersSet);
	~Label();

	// 0 - Plain / 1 - Bold / 2 - Italic
	void changeFormatting(std::map<GLchar, Character> * charactersSet);
	void setText(std::string newText);
	void render(Shader &shader, glm::vec3 color);
	
	// Checks if mouse is inside the label area
	void checkCollision(GLint mouseX, GLint mouseY);
	void setLeftMouseClickFunction(mouseClickFunction clickFunction);
	void setRightMouseClickFunction(mouseClickFunction clickFunction);

	void setHorizontalPosition(GLint newXPos);
	void setVerticalPosition(GLint newYPos);
	
	// VOID - VOID
	mouseClickFunction leftMouseClickFunction;
	mouseClickFunction rightMouseClickFunction;
};

#endif // LABEL_H
