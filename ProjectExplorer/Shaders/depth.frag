// Standard color with texture

#version 330 core
in vec2 TexCoords;

out vec4 color;

uniform sampler2D texture1;

void main()
{    
	color = texture(texture1, TexCoords);
}



// Visualization of the fragments depth value using non-linear values

/*#version 330 core
in vec2 TexCoords;

out vec4 color;

uniform sampler2D texture1;

void main()
{     
    color = vec4(vec3(gl_FragCoord.z), 1.0f);   
}*/



// Visualization of the fragments depth value using linear values

/*#version 330 core
out vec4 color;

float near = 1.0; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{             
    float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
    color = vec4(vec3(depth), 1.0f);
}*/