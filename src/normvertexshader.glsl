#version 330
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aOffset;
layout (location = 3) in vec3 aRandom;
#define PI 3.1415926538

out VS_OUT {
	vec3 normal;
} vs_out;

uniform mat4 view;
uniform mat4 model;
uniform float time;

void main()
{
	vec4 newPos = model * vec4(aPos, 1.0);
	if (gl_VertexID % 3 == 0)
	{
		newPos.x+=aRandom.y*sin(2*PI*aRandom.z*time+2*aRandom.x*PI);
		newPos.y*=(aRandom.x*.2+.5);
		newPos.z+=aRandom.y*cos(1*PI*aRandom.z*time+2*0.5*aRandom.x*PI);
	}

	gl_Position = view * vec4(newPos.x+aOffset.x, newPos.y, newPos.z+aOffset.z, newPos.a);
	mat3 normalMatrix = mat3(transpose(inverse(view * model)));
	vs_out.normal = normalize(vec3(vec4(normalMatrix * (aNormal), 0.0)));
}

// #version 330
// layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec3 aNormal;
// layout (location = 2) in vec3 aOffset;

// out VS_OUT {
// 	vec3 normal;
// 	// vec3 offset;
// } vs_out;

// uniform mat4 view;
// uniform mat4 model;

// void main()
// {
// 	vec3 temp = vec3(aPos.x+aOffset.x, aPos.y, aPos.z+aOffset.z);
// 	vec4 newPos = model * vec4(aPos, 1.0);
// 	fragPos=vec3(newPos);

// 	// normal=aNormal;
// 	// normal = vec3(model * vec4(aNormal+aOffset, 0));
// 	// normal = mat3(transpose(inverse(model))) * (aNormal+aOffset);
// 	// normal=normalize(aNormal+aOffset);

// 	gl_Position = view * vec4(newPos.x+aOffset.x, newPos.y, newPos.z+aOffset.z, newPos.a);
// 	// gl_Position = projection * view * newPos;

// 	// gl_Position = view * model * vec4(aPos, 1.0); 
// 	// mat3 normalMatrix = mat3(transpose(inverse(view * model)));
// 	// vs_out.normal = normalize(vec3(vec4(normalMatrix * aNormal, 0.0)));
// 	vs_out.normal=aNormal;
// 	// vs_out.offset=offset;
// }