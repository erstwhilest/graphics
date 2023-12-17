#version 330 core

out vec4 FragColor;

in vec3 fragPos;
in vec3 normal;
in vec3 color;

uniform float ambientStrength;
uniform vec3 lightColor;
uniform vec3 lightPos;

void main()
{
	vec3 ambient = ambientStrength * lightColor;

	vec3 lightDirection = normalize(lightPos - fragPos);
	float diff = dot(lightDirection, normalize(normal));
	vec3 diffuse = vec3(diff, diff, diff);

	vec3 result = (ambient + clamp(diffuse, 0, 1))*color;

	FragColor = vec4(result, 1.0);

	// vec3 ambient = ambientStrength * lightColor;

	// vec3 norm = normalize(normal);
	// vec3 lightDirection = normalize(lightPos - fragPos);
	// float diff = max(dot(norm, lightDirection), 0.0);
	// vec3 diffuse = diff * lightColor;

	// vec3 result = (ambient + diffuse) * color;

	// FragColor = vec4(result, 1.0);
}