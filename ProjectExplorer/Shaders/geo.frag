#version 330 core
in vec2 TexCoords;
in vec3 Normal;  
in vec3 FragPos;

out vec4 color;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;
uniform sampler2D texture1;
uniform float ambStr;

void main()
{    
	vec3 ambient = ambStr * vec3(1.0, 1.0, 1.0);
	ambient = ambient * objectColor * lightColor;

	
    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
	
    vec3 result = (ambient + diffuse) * objectColor;

	vec4 tex = texture(texture1, TexCoords);
	color = tex * vec4(result, 1.0);
}