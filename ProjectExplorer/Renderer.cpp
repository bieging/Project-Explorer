#include "Renderer.h"

Renderer::Renderer()
{
	//this->shaderProg = shader;
	this->InitRenderData();
}

Renderer::~Renderer()
{
	delete this;
}

void Renderer::Render(glm::vec3 position)
{
	//this->shaderProg.Use();

	glm::mat4 model;
	model = glm::translate(model, position);
	glUniformMatrix4fv(glGetUniformLocation(this->shaderProg.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(this->cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void Renderer::InitRenderData()
{
}
