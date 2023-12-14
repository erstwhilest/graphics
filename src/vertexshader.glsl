
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aOffset;
layout (location = 3) in vec3 aRandom;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float time;

out vec3 color;
#define PI 3.1415926538

void main()
{
	vec4 newPos = model * vec4(aPos, 1.0f);

//	vec3 newPos = vec3(aPos.x+aOffset.x, aPos.y, aPos.z+aOffset.z);
//
//	if (gl_VertexID % 3 == 0)
//	{
//		// newPos.x+=sin(time);
//		newPos.x+=aRandom.y*sin(2*PI*aRandom.z*time+2*aRandom.x*PI);
//		newPos.z+=aRandom.y*cos(1*PI*aRandom.z*time+2*0.5*aRandom.x*PI);
//	}

	if (gl_VertexID % 3 == 0)
	{
		newPos.x+=aRandom.y*sin(2*PI*aRandom.z*time+2*aRandom.x*PI);
		newPos.y*=(aRandom.x*.2+.5);
		newPos.z+=aRandom.y*cos(1*PI*aRandom.z*time+2*0.5*aRandom.x*PI);
	}

	gl_Position = projection * view * vec4(newPos.x+aOffset.x, newPos.y, newPos.z+aOffset.z, newPos.a);

//	gl_Position = projection * view * model * vec4(newPos, 1.0f);
	color=aColor;
}