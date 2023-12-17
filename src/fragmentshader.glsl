#version 330 core

out vec4 FragColor;

in vec3 fragPos;
in vec3 normal;
in vec3 color;

uniform float ambientStrength;
uniform float diffuseStrength;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightModel;

uniform float constant;
uniform float linear;
uniform float quadratic;

void main()
{

	vec3 ambient = ambientStrength * lightColor;

	// vec3 lightPosWorld=vec3(lightModel*vec4(lightPos, 1.0));
	vec3 lightPosWorld=lightPos;

	float dist = length(lightPosWorld - fragPos);
	float attenuation=1.0/(constant+linear*dist+quadratic*(dist*dist));

	vec3 lightDirection = normalize(lightPosWorld - fragPos);
	float diff = max(dot(normalize(normal), lightDirection), 0.0);
	vec3 diffuse = diff * diffuseStrength * lightColor;

	vec3 result = (ambient + diffuse*attenuation) * color;

	FragColor = vec4(result, 1.0);
}