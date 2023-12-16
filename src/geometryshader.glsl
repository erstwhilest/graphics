#version 330
layout (points) in;
layout (triangle_fan, max_vertices = 5) out;

vec3 getNormal()
{
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	return normalize(cross(a, b));
}

void main()
{
	vec3 normal = getNormal();

	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	EndPrimitive();
}