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
uniform mat4 objModel;
uniform mat4 lightModel;
uniform mat4 projection;
uniform float time;

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
	mat4 modelToWorld=buildTranslationMatrix(aOffset)*objModel;
	vec4 newPos = modelToWorld * vec4(aPos, 1.0);
	if (gl_VertexID % 3 == 0)
	{
		newPos.x+=aRandom.y*sin(2*PI*aRandom.z*time+2*aRandom.x*PI);
		newPos.y*=(aRandom.x*.2+.5);
		newPos.z+=aRandom.y*cos(1*PI*aRandom.z*time+2*0.5*aRandom.x*PI);
	}

	mat3 normalMatrix = mat3(transpose(inverse(view * modelToWorld)));
	vs_out.normal = normalize(vec3(vec4(normalMatrix * (aNormal), 0.0)));

	gl_Position=projection*view*modelToWorld*vec4(aPos, 1.0);
}