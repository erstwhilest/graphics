#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aOffset;
layout (location = 3) in vec3 aRandom;
// layout (location = 4) in vec3 aColorCutTime;

out vec3 fragPos;
out vec3 normal;
out vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;
uniform float minCutHeight;
uniform vec3 lightPos;

uniform vec3 baseColor;
uniform vec3 tipColor;
#define PI 3.1415926538

mat4 buildTranslationMatrix(vec3 delta)
{
	return mat4(
		vec4(1.0, 0.0, 0.0, 0.0),
		vec4(0.0, 1.0, 0.0, 0.0),
		vec4(0.0, 0.0, 1.0, 0.0),
		vec4(delta, 1.0));
}

void main()
{
	mat4 modelToWorld=buildTranslationMatrix(aOffset)*model;
	vec4 newPos = modelToWorld * vec4(aPos, 1.0);
	if (gl_VertexID % 3 == 0)
	{
		// if (length(vec3(newPos)-lightPos)<100)
		// {
		// 	newPos.y=min(minCutHeight, (-1.0/(0.5*time+1)+1)*30.0);
		// }
		// else
		// {
		// 	newPos.y=(-1.0/(0.5*time+1)+1)*30.0;
		// }
		newPos.y=(-1.0/(0.5*time+1)+1)*30.0;
		newPos.x+=aRandom.y*sin(2*PI*aRandom.z*time+2*aRandom.x*PI);
		// newPos.y*=(aRandom.x*.2+.5);
		newPos.z+=aRandom.y*cos(1*PI*aRandom.z*time+2*0.5*aRandom.x*PI);
		color=tipColor;
	}
	else
	{
		color=baseColor;
	}

	normal=vec3(modelToWorld*vec4(aNormal,0));
	// fragPos=vec3(modelToWorld*vec4(newPos, 1.0));
	fragPos=vec3(newPos);
	gl_Position=projection*view*newPos;
}