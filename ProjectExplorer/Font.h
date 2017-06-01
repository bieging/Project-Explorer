#ifndef FONT_H
#define FONT_H

#pragma once

#include <iostream>
#include <string>
#include <map>

#include "Character.h"

// GLEW
#define GLEW_STATIC
#include <c:/opengl/GLEW/glew.h>

// GLM Mathemtics
#include <c:/opengl/glm/glm.hpp>

// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H

class Font
{
private:
	std::map<GLchar, Character> CharactersPlain;
	std::map<GLchar, Character> CharactersBold;
	std::map<GLchar, Character> CharactersItalic;

	// FreeType
	FT_Library ft;
	FT_Face facePlain;
	FT_Face faceBold;
	FT_Face faceItalic;
public:
	Font(std::string dirPath, std::string fontName);
	~Font();

	// 0 - Plain / 1 - Bold / 2 - Italic
	std::map<GLchar, Character> * getCharacterSet(int formatting);
};

#endif // FONT_H