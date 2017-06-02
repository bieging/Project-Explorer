#include "Label.h"

Label::Label()
{
}

Label::Label(std::string text, GLfloat xPos, GLfloat yPos, GLfloat scale, std::map<GLchar, Character> * charactersSet)
{
	this->text = text;
	this->xPos = xPos;
	this->yPos = yPos;
	this->scale = scale;
	this->charactersSet = *charactersSet;

	this->tf = PLAIN;

	// Setup text VAO and VBO for text rendering
	glGenVertexArrays(1, &this->textVAO);
	glGenBuffers(1, &this->textVBO);
	glBindVertexArray(this->textVAO);
	glBindBuffer(GL_ARRAY_BUFFER, this->textVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	calculateDimension();
}

Label::~Label()
{
}

void Label::calculateDimension()
{
	Character ch;

	GLfloat charXPos;
	GLfloat w;

	// Create auxiliar variable to avoid updating the value of the original x position
	GLfloat auxXPos = this->xPos;

	// Iterate through all characters
	std::string::const_iterator c = text.begin();

	// Gets position of the first character to later calculate the word's total width
	ch = this->charactersSet[*c];

	this->width = auxXPos + ch.Bearing.x * scale;

	for (; c != text.end(); c++)
	{
		// Gets position of the first character to later calculate the word's total width
		ch = this->charactersSet[*c];

		charXPos = auxXPos + ch.Bearing.x * scale;
		w = ch.Size.x * scale;

		this->height = (ch.Size.x * scale > height) ? ch.Size.x * scale : height;

		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		auxXPos += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}

	this->width = charXPos + w - this->width;
}

void Label::changeFormatting(std::map<GLchar, Character> * charactersSet)
{
	if (this->initialized)
	{
		this->charactersSet = *charactersSet;

		calculateDimension();
	}
	else
	{
		std::cout << "Unable to process operation because object is not initialized" << std::endl;
	}
}

void Label::setText(std::string newText)
{
	if (this->initialized)
	{
		this->text = newText;
	}
	else
	{
		std::cout << "Unable to process operation because object is not initialized" << std::endl;
	}
}

void Label::render(Shader &shader, glm::vec3 color)
{
	if (this->initialized)
	{
		Character ch;

		GLfloat charXPos;
		GLfloat charYPos;
		GLfloat w;
		GLfloat h;

		GLfloat width;
		GLfloat height;

		// Create auxiliar variable to avoid updating the value of the original x position
		GLfloat auxXPos = this->xPos;

		// Activate corresponding render state	
		shader.Use();
		glUniform3f(glGetUniformLocation(shader.Program, "textColor"), color.x, color.y, color.z);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(this->textVAO);

		// Iterate through all characters
		std::string::const_iterator c = this->text.begin();

		// Gets position of the first character to later calculate the word's total width

		// Draw text
		for (; c != text.end(); c++)
		{
			ch = this->charactersSet[*c];

			charXPos = auxXPos + ch.Bearing.x * scale;
			charYPos = this->yPos - (ch.Size.y - ch.Bearing.y) * scale;

			w = ch.Size.x * scale;
			h = ch.Size.y * scale;

			// Update VBO for each character
			GLfloat vertices[6][4] = {
				{ charXPos,     charYPos + h,   0.0, 0.0 },
				{ charXPos,     charYPos,       0.0, 1.0 },
				{ charXPos + w, charYPos,       1.0, 1.0 },

				{ charXPos,     charYPos + h,   0.0, 0.0 },
				{ charXPos + w, charYPos,       1.0, 1.0 },
				{ charXPos + w, charYPos + h,   1.0, 0.0 }
			};
			// Render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			// Update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, this->textVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// Render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			auxXPos += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
		}

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		std::cout << "Unable to process operation because object is not initialized" << std::endl;
	}
}

bool Label::checkCollision(GLint mouseX, GLint mouseY)
{
	if (this->initialized)
	{
		if (mouseX <= this->xPos || mouseX >= (this->xPos + this->width))
		{
			return false;
		}

		if (mouseY <= this->yPos || mouseY >= (this->yPos + this->height))
		{
			return false;
		}

		return true;
	}
	else
	{
		std::cout << "Unable to process operation because object is not initialized" << std::endl;
	}

}