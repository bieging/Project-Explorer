#ifndef RENDERER_H
#define RENDERER_H

#include <c:/opengl/GLEW/glew.h>
#include <c:/opengl/glm/glm.hpp>
#include <c:/opengl/glm/gtc/type_ptr.hpp>
#include <c:/opengl/glm/gtc/matrix_transform.hpp>

#include <c:/opengl/SOIL/SOIL.h>

class Renderer
{
public:
	Renderer();
	~Renderer();

	void Render(glm::vec3 position);
private:
	Shader shaderProg;
	GLuint cubeVAO;
	
	void InitRenderData();
};

#endif