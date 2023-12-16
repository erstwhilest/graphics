#version 330 core

out vec4 FragColor;

in vec3 normal;
in vec3 fragPos;
in vec3 color;

uniform float ambientStrength;
uniform vec3 lightColor;
uniform vec3 lightPos;

void main()
{
	vec3 ambient = ambientStrength * lightColor;

	vec3 result = (ambient) * color;

	FragColor = vec4(result, 1.0);
}