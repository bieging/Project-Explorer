#include "Font.h"

Font::Font(std::string dirPath, std::string fontName)
{
	bool buildSuccess = true;

	std::string plainPath  = dirPath + "/" + fontName + ".ttf";
	std::string boldPath   = dirPath + "/" + fontName + "bd.ttf";
	std::string italicPath = dirPath + "/" + fontName + "i.ttf";

	const char * plainChar = plainPath.c_str();
	const char * boldChar = boldPath.c_str();
	const char * italicChar = italicPath.c_str();

	std::cout << boldChar << std::endl;
	std::cout << italicChar << std::endl;

	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	// Load font as face
	if (FT_New_Face(ft, plainChar, 0, &this->facePlain))
		std::cout << "ERROR::FREETYPE: Failed to load " << fontName << std::endl;
	else
		std::cout << "Successfully loaded " << fontName << std::endl;

	// Load font as face
	if (FT_New_Face(ft, boldChar, 0, &this->faceBold))
		std::cout << "ERROR::FREETYPE: Failed to load Arial Bold" << std::endl;
	else
		std::cout << "Successfully loaded " << fontName << " Bold" << std::endl;

	// Load font as face
	if (FT_New_Face(ft, italicChar, 0, &this->faceItalic))
		std::cout << "ERROR::FREETYPE: Failed to load Arial Italic" << std::endl;
	else
		std::cout << "Successfully loaded " << fontName << " Italic" << std::endl;

	// Set size to load glyphs as
	FT_Set_Pixel_Sizes(this->facePlain, 0, 48);

	// Set size to load glyphs as
	FT_Set_Pixel_Sizes(this->faceBold, 0, 48);

	// Set size to load glyphs as
	FT_Set_Pixel_Sizes(this->faceItalic, 0, 48);

	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Load first 128 characters of ASCII set
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(this->facePlain, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load plain Glyph" << std::endl;
			buildSuccess = false;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			facePlain->glyph->bitmap.width,
			facePlain->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			facePlain->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(facePlain->glyph->bitmap.width, facePlain->glyph->bitmap.rows),
			glm::ivec2(facePlain->glyph->bitmap_left, facePlain->glyph->bitmap_top),
			facePlain->glyph->advance.x
		};
		CharactersPlain.insert(std::pair<GLchar, Character>(c, character));
	}

	if (buildSuccess) std::cout << "Successfully builded " << fontName << std::endl;
	buildSuccess = true;

	// Load first 128 characters of ASCII set
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(faceBold, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load bold Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			faceBold->glyph->bitmap.width,
			faceBold->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			faceBold->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(faceBold->glyph->bitmap.width, faceBold->glyph->bitmap.rows),
			glm::ivec2(faceBold->glyph->bitmap_left, faceBold->glyph->bitmap_top),
			faceBold->glyph->advance.x
		};
		CharactersBold.insert(std::pair<GLchar, Character>(c, character));
	}

	if (buildSuccess) std::cout << "Successfully builded " << fontName << " Bold" << std::endl;
	buildSuccess = true;

	// Load first 128 characters of ASCII set
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(faceItalic, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load italic Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			faceItalic->glyph->bitmap.width,
			faceItalic->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			faceItalic->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(faceItalic->glyph->bitmap.width, faceItalic->glyph->bitmap.rows),
			glm::ivec2(faceItalic->glyph->bitmap_left, faceItalic->glyph->bitmap_top),
			faceItalic->glyph->advance.x
		};
		CharactersItalic.insert(std::pair<GLchar, Character>(c, character));
	}

	if (buildSuccess) std::cout << "Successfully builded " << fontName << " Italic" << std::endl;
	buildSuccess = true;

	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(faceBold);
	FT_Done_FreeType(ft);
}

Font::~Font()
{
}

std::map<GLchar, Character> *  Font::getCharacterSet(int formatting)
{
	switch (formatting)
	{
		case 0:
			return &CharactersPlain;
			break;

		case 1:
			return &CharactersBold;
			break;

		case 2:
			return &CharactersItalic;
			break;
	}
}