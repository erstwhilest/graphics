#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aOffset;
layout (location = 3) in vec3 aRandom;

out vec3 fragPos;
out vec3 normal;
out vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

uniform vec3 baseColor;
uniform vec3 tipColor;
#define PI 3.1415926538

void main()
{
	// apply scale/rotate/position before offsets
	vec3 temp = vec3(aPos.x+aOffset.x, aPos.y, aPos.z+aOffset.z);
	// fragPos = vec3(model * vec4(temp, 1.0));

	vec4 newPos = model * vec4(aPos, 1.0);
	fragPos=vec3(newPos);

	// moving the tip
	if (gl_VertexID % 3 == 0)
	{
		// newPos.x+=aRandom.y*sin(2*PI*aRandom.z*time+2*aRandom.x*PI);
		// newPos.y*=(aRandom.x*.2+.5);
		// newPos.z+=aRandom.y*cos(1*PI*aRandom.z*time+2*0.5*aRandom.x*PI);
		color = tipColor;
	}
	else
	{
		color = baseColor;
	}
	// color=tipColor;
	// normal=aNormal;
	normal = vec3(model * vec4(aNormal+aOffset, 0));
	// normal = mat3(transpose(inverse(model))) * (aNormal+aOffset);
	// normal=normalize(aNormal+aOffset);

	gl_Position = projection * view * vec4(newPos.x+aOffset.x, newPos.y, newPos.z+aOffset.z, newPos.a);
	// gl_Position = projection * view * newPos;
}