// Standard color with texture

#version 330 core
in vec2 TexCoords;

out vec4 color;

uniform sampler2D texture1;
uniform float ambStr;
uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{    
	vec3 ambient = ambStr * vec3(1.0, 1.0, 1.0);

	ambient = ambient * objectColor * lightColor;

	vec4 tex = texture(texture1, TexCoords);

	color = tex * vec4(ambient, 1.0);
}