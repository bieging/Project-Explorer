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

// GLM Mathemtics
#include <c:/opengl/glm/glm.hpp>

enum TextFormatting
{
	PLAIN,
	BOLD,
	ITALIC
};

class Label
{
private:
	std::string text;

	GLuint textVAO, textVBO;

	GLfloat width, height;
	GLfloat xPos, yPos;
	GLfloat scale;

	std::map<GLchar, Character> charactersSet;

	TextFormatting tf;

	void calculateDimension();
public:
	Label(std::string text, GLfloat xPos, GLfloat yPos, GLfloat scale, std::map<GLchar, Character> * charactersSet);
	~Label();

	// 0 - Plain / 1 - Bold / 2 - Italic
	void changeFormatting(std::map<GLchar, Character> * charactersSet);
	void render(Shader &shader, glm::vec3 color);
};

#endif // LABEL_H